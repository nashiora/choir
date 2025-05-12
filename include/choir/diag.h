#ifndef CHOIR_DIAG_H_
#define CHOIR_DIAG_H_

#include <choir/extern_c.h>
#include <choir/types.h>
#include <choir/context.h>

CHOIR_EXTERN_C_BEGIN

const char* choir_diag_level_name_get(choir_diagnostic_level level);

void choir_diag_issue(
    choir_context_ref context,
    choir_diagnostic_level level,
    const char* format,
    ...
);

void choir_diag_issue_source_bytes(
    choir_context_ref context,
    choir_diagnostic_level level,
    choir_source_ref source,
    ssize_t byte_position,
    const char* format,
    ...
);

void choir_diag_issue_source_location(
    choir_context_ref context,
    choir_diagnostic_level level,
    choir_source_ref source,
    int64_t line_number,
    int64_t column_number,
    const char* format,
    ...
);

CHOIR_EXTERN_C_END

#endif /* CHOIR_DIAG_H_ */
