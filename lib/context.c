#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <choir/context.h>
#include "context_internal.h"

#ifdef _MSC_VER
#    define ftell _ftelli64
#endif

struct choir_opaque_context global_context = {0};

choir_context_ref choir_context_create() {
    choir_context_ref context = calloc(1, sizeof *context);
    return context;
}

void choir_context_destroy(choir_context_ref context) {
    choir_da_free(&context->sources);

    *context = (struct choir_opaque_context){0};
    if (context != &global_context) {
        free(context);
    }
}

choir_context_ref choir_context_get_global() {
    return &global_context;
}

choir_source_ref choir_context_source_read_from_file(choir_context_ref context, const char* file_path, ssize_t file_path_length) {
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
    return choir_context_source_create(context, file_path, file_path_length, text_buffer, text_buffer_length);
}

choir_source_ref choir_context_source_create(choir_context_ref context, const char* name, ssize_t name_length, const char* text, ssize_t text_length) {
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
