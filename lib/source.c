#include <stdlib.h>

#include <choir/source.h>
#include "source_internal.h"

static void choir_source_ensure_line_info_cached(choir_source_ref source);
static ssize_t choir_source_line_info_binary_search(choir_source_ref source, ssize_t byte_position);

const char* choir_source_name_get(choir_source_ref source, ssize_t* name_length) {
    if (name_length != NULL) *name_length = source->name_length;
    return source->name;
}

const char* choir_source_text_get(choir_source_ref source, ssize_t* text_length) {
    if (text_length != NULL) *text_length = source->text_length;
    return source->text;
}

bool choir_source_seek(choir_source_ref source, ssize_t byte_position, ssize_t* line_begin, ssize_t* line_end, int64_t* line_number, int64_t* column_number) {
    ssize_t line_info_index = choir_source_line_info_binary_search(source, byte_position);
    if (line_info_index < 0) {
        return false;
    }

    choir_source_line_info line_info = source->cached_line_infos[line_info_index];

    if (line_begin != NULL) *line_begin = line_info.begin;
    if (line_end != NULL) *line_end = line_info.end;

    if (line_number != NULL) *line_number = 1 + (int64_t)line_info_index;
    // TODO(local): Take UTF-8 multi-byte codepoints into account when calculating a column number.
    if (column_number != NULL) *column_number = 1 + (int64_t)(byte_position - line_info.begin);

    return true;
}

static void choir_source_ensure_line_info_cached(choir_source_ref source) {
    if (source->cached_line_infos != NULL) return;

    const char* text = source->text;
    ssize_t text_length = source->text_length;

    ssize_t line_info_count = 1;

    // first pass: collect the number of line infos to track
    for (ssize_t i = 0; i < text_length; i++) {
        char c = text[i];
        if (c == '\n' || c == '\r') {
            line_info_count++;

            char other_newline = c == '\n' ? '\r' : '\n';
            if (i + 1 < text_length && text[i + 1] == other_newline) {
                i++; // skip ahead one more character to consume a two-character newline
            }
        }
    }

    // second pass: populate the line infos array
    choir_source_line_info* cached_line_infos = calloc((size_t)line_info_count, sizeof *cached_line_infos);
    source->cached_line_infos = cached_line_infos;
    source->line_info_count = line_info_count;

    ssize_t line_begin = 0;
    for (ssize_t i = 0; i < text_length; i++) {
        char c = text[i];
        if (c == '\n' || c == '\r') {
            ssize_t line_end = i;

            char other_newline = c == '\n' ? '\r' : '\n';
            if (i + 1 < text_length && text[i + 1] == other_newline) {
                i++; // skip ahead one more character to consume a two-character newline
            }

            cached_line_infos[i] = (choir_source_line_info){ .begin = line_begin, .end = line_end };
            line_begin = i + 1;
        }
    }

    cached_line_infos[line_info_count] = (choir_source_line_info){ .begin = line_begin, .end = text_length };
}

static ssize_t choir_source_line_info_binary_search(choir_source_ref source, ssize_t byte_position) {
    choir_source_ensure_line_info_cached(source);

    if (byte_position < 0 || byte_position >= source->text_length) {
        return -1;
    }

    ssize_t window_begin = 0;
    ssize_t window_end = source->line_info_count;

    while (window_begin < window_end) {
        ssize_t index = window_begin + (window_end - window_begin) / 2;

        choir_source_line_info line_info = source->cached_line_infos[index];
        if (byte_position < line_info.begin) {
            window_end = index;
        } else if (byte_position > line_info.end) {
            window_begin = index + 1;
        } else {
            return index;
        }
    }

    return -1;
}
