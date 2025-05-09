#include <assert.h>
#include <stdlib.h>

#include <choir/types.h>

#define CHOIR_DA_INITIAL_CAPACITY 256

#define CHOIR_DYNAMIC_ARRAY_FIELDS(ElemType) ElemType* data; ssize_t capacity, count

#define choir_da_ensure_capacity(DynArr, MinCap) do { \
        choir_dynamic_array_ensure_capacity((ssize_t)sizeof(*(DynArr)->data), \
            (void**)&(DynArr)->data, &(DynArr)->capacity, (MinCap)); \
    } while (0)

#define choir_da_push(DynArr, Elem) do { \
        choir_da_ensure_capacity((DynArr), (DynArr)->count + 1); \
        assert((DynArr)->data != NULL); \
        (DynArr)->data[(DynArr)->count++] = (Elem); \
    } while (0)

#define choir_da_push_many(DynArr, Elems, ElemCount) do { \
        choir_da_ensure_capacity((DynArr), (DynArr)->count + (ElemCount));  \
        memcpy((DynArr)->data + (DynArr)->count, Elems, sizeof(*(DynArr)->data) * (ElemCount)); \
        (DynArr)->count += (ElemCount); \
    } while (0)

#define choir_da_pop(DynArr) (assert((DynArr)->data != NULL), (DynArr)->data[--(DynArr)->count])

#define choir_da_free(DynArr) do { free((DynArr)->data); } while (0)

void choir_dynamic_array_ensure_capacity(ssize_t element_size, void** da_data_ptr, ssize_t* da_capacity_ptr, ssize_t required_capacity);
