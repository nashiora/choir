#include <assert.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>

#include <choir/diag.h>
#include <choir/types.h>

#include "source_internal.h"

static void choir_diag_impl(
    choir_context_ref context,
    choir_diagnostic_level level,
    choir_source_ref source,
    bool use_location,
    ssize_t byte_position,
    int64_t line_number,
    int64_t column_number,
    const char* format,
    va_list v
);

const char* choir_diag_level_name_get(choir_diagnostic_level level) {
    switch (level) {
        default: return "<unknown-diagnostic-level>";
        case CHOIR_DIAG_IGNORED: return "Ignored";
        case CHOIR_INFO: return "Info";
        case CHOIR_REMARK: return "Remark";
        case CHOIR_WARNING: return "Warning";
        case CHOIR_ERROR: return "Error";
        case CHOIR_FATAL: return "Fatal";
    }
}

void choir_diag_issue(
    choir_context_ref context,
    choir_diagnostic_level level,
    const char* format,
    ...
) {
    assert(context != nullptr);

    va_list v;
    va_start(v, format);
    choir_diag_impl(context, level, nullptr, false, 0, 0, 0, format, v);
    va_end(v);
}

void choir_diag_issue_source_bytes(
    choir_context_ref context,
    choir_diagnostic_level level,
    choir_source_ref source,
    ssize_t byte_position,
    const char* format,
    ...
) {
    assert(context != nullptr);

    va_list v;
    va_start(v, format);
    choir_diag_impl(context, level, source, false, byte_position, 0, 0, format, v);
    va_end(v);
}

void choir_diag_issue_source_location(
    choir_context_ref context,
    choir_diagnostic_level level,
    choir_source_ref source,
    int64_t line_number,
    int64_t column_number,
    const char* format,
    ...
) {
    assert(context != nullptr);

    va_list v;
    va_start(v, format);
    choir_diag_impl(context, level, source, true, 0, line_number, column_number, format, v);
    va_end(v);
}

static const char* level_tags[] = {
    "[(Ignored)]",
    "[Info]",
    "[Remark]",
    "[Warning]",
    "[Error]",
    "[Fatal]",
    nullptr,
};

static void choir_diag_impl(
    choir_context_ref context,
    choir_diagnostic_level level,
    choir_source_ref source,
    bool use_location,
    ssize_t byte_position,
    int64_t line_number,
    int64_t column_number,
    const char* format,
    va_list v
) {
    if (level == CHOIR_DIAG_IGNORED) {
        return;
    }

    if (level >= 0 && level <= CHOIR_FATAL) {
        fprintf(stderr, "%s ", level_tags[(int)level]);
    }

    if (source != nullptr) {
        fprintf(stderr, "%.*s", (int)source->name_length, source->name);
        if (use_location) {
            fprintf(stderr, "(%"PRIi64",%"PRIi64"): ", line_number, column_number);
        } else {
            fprintf(stderr, "[%"PRIi64"]: ", (int64_t)byte_position);
        }
    }

    vfprintf(stderr, format, v);
    fprintf(stderr, "\n");
    fflush(stderr);

    if (level == CHOIR_FATAL) {
        abort();
    }
}
