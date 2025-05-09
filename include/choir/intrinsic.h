#ifndef CHOIR_INTRINSIC_H_
#define CHOIR_INTRINSIC_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

const char* choir_intrinsic_name_get(choir_intrinsic_kind intrinsic, ssize_t* intrinsic_name_length);
bool choir_intrinsic_is_overloaded(choir_intrinsic_kind intrinsic);
choir_type_ref choir_intrinsic_type_get(choir_context_ref context, choir_intrinsic_kind intrinsic, choir_type_ref* parameter_types, ssize_t parameter_types_count);

CHOIR_EXTERN_C_END

#endif /* CHOIR_INTRINSIC_H_ */
