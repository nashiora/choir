#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <choir/source.h>
#include "context_internal.h"
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

choir_source_ref choir_source_read_from_file(choir_context_ref context, const char* file_path, ssize_t file_path_length) {
    if (file_path == NULL) {
        // TODO(local): Report no file_path error.
        fprintf(stderr, "TEMP :: Choir Context: error: No file path provided.\n");
        return NULL;
    }

    if (file_path_length == 0) {
        file_path_length = strlen(file_path);
    }

    char* file_path_copy = calloc((size_t)file_path_length + 1, sizeof *file_path_copy);
    memcpy(file_path_copy, file_path, (size_t)file_path_length);

    // NOTE(local): Windows fread converts \r\n for us already when we read in text mode.
    // TODO(local): Test that the newline conversion happens correctly on other platforms, and do it manually when it doesn't.
    FILE* f = fopen(file_path_copy, "r");
    free(file_path_copy);

    if (f == NULL) {
        // TODO(local): Report fopen error.
        fprintf(stderr, "TEMP :: Choir Context: error: Failed to fread source file '%.*s': %s\n", (int)file_path_length, file_path, strerror(errno));
        return NULL;
    }

    if (0 != fseek(f, 0, SEEK_END)) {
        fclose(f);
        // TODO(local): Report fseek error.
        fprintf(stderr, "TEMP :: Choir Context: error: Failed to fseek source file '%.*s': %s\n", (int)file_path_length, file_path, strerror(errno));
        return NULL;
    }

    int64_t max_file_length = ftell(f);
    if (max_file_length < 0) {
        fclose(f);
        // TODO(local): Report ftell error.
        fprintf(stderr, "TEMP :: Choir Context: error: Failed to ftell source file '%.*s': %s\n", (int)file_path_length, file_path, strerror(errno));
        return NULL;

    }

    if (0 != fseek(f, 0, SEEK_SET)) {
        fclose(f);
        // TODO(local): Report fseek error.
        fprintf(stderr, "TEMP :: Choir Context: error: Failed to fseek source file '%.*s': %s\n", (int)file_path_length, file_path, strerror(errno));
        return NULL;
    }

    char* text_buffer = calloc((size_t)max_file_length + 1, 1);
    size_t actual_read_bytes = fread(text_buffer, (size_t)max_file_length, 1, f);

    if (0 != ferror(f)) {
        fclose(f);
        // TODO(local): Report fread error.
        fprintf(stderr, "TEMP :: Choir Context: error: Failed to fread source file '%.*s': %s\n", (int)file_path_length, file_path, strerror(errno));
        return NULL;
    }

    ssize_t text_buffer_length = (ssize_t)strlen(text_buffer);
    return choir_source_create(context, file_path, file_path_length, text_buffer, text_buffer_length);
}

choir_source_ref choir_source_create(choir_context_ref context, const char* name, ssize_t name_length, const char* text, ssize_t text_length) {
    if (name == NULL) name = "";
    if (name_length == 0) name_length = (ssize_t)strlen(name);

    if (text == NULL) text = "";
    if (text_length == 0) text_length = (ssize_t)strlen(text);

    choir_source_ref source = calloc(1, sizeof *source);
    *source = (struct choir_opaque_source) {
        .name = name,
        .name_length = name_length,
        .text = text,
        .text_length = text_length,
    };

    choir_da_push(&context->sources, source);
    return source;
}
