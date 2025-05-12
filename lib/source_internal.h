#ifndef CHOIR_SOURCE_INTERNAL_H_
#define CHOIR_SOURCE_INTERNAL_H_

#include <choir/source.h>
#include "support.h"

typedef struct choir_source_line_info {
    ssize_t begin;
    ssize_t end;
} choir_source_line_info;

struct choir_opaque_source {
    const char* name;
    ssize_t name_length;

    const char* text;
    ssize_t text_length;

    choir_source_line_info* cached_line_infos;
    ssize_t line_info_count;
};

typedef struct choir_source_refs {
    CHOIR_DYNAMIC_ARRAY_FIELDS(choir_source_ref);
} choir_source_refs;

#endif /* CHOIR_SOURCE_INTERNAL_H_ */
