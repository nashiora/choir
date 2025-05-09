#include "assert.h"
#include "support.h"

void choir_dynamic_array_ensure_capacity(ssize_t element_size, void** da_data_ptr, ssize_t* da_capacity_ptr, ssize_t required_capacity) {
    assert(element_size > 0);
    if (*da_capacity_ptr >= required_capacity) {
        return;
    }

    ssize_t old_capacity = *da_capacity_ptr;
    ssize_t new_capacity = *da_capacity_ptr;

    if (new_capacity == 0) new_capacity = CHOIR_DA_INITIAL_CAPACITY;
    while (new_capacity < required_capacity) new_capacity *= 2;

    assert(new_capacity > 0);

    *da_capacity_ptr = new_capacity;
    *da_data_ptr = realloc(*da_data_ptr, new_capacity * element_size);
}
