#ifndef CHOIR_CONTEXT_INTERNAL_H_
#define CHOIR_CONTEXT_INTERNAL_H_

#include <choir/context.h>
#include "source_internal.h"

struct choir_opaque_context {
    choir_source_refs sources;
};

#endif /* CHOIR_CONTEXT_INTERNAL_H_ */
