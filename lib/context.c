#include <stdlib.h>

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
