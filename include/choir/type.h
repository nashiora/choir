#ifndef CHOIR_TYPE_H_
#define CHOIR_TYPE_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

choir_context_ref choir_type_context_get(choir_type_ref type);
const char* choir_type_print_to_string(choir_type_ref type, ssize_t* string_length);
choir_type_kind choir_type_kind_get(choir_type_ref type);
bool choir_type_is_sized(choir_type_ref type);
choir_type_ref choir_type_void(choir_context_ref context);
choir_type_ref choir_type_int(choir_context_ref context, int32_t bit_width);
choir_type_ref choir_type_int1(choir_context_ref context);
choir_type_ref choir_type_int8(choir_context_ref context);
choir_type_ref choir_type_int16(choir_context_ref context);
choir_type_ref choir_type_int32(choir_context_ref context);
choir_type_ref choir_type_int64(choir_context_ref context);
int32_t choir_type_int_width_get(choir_type_ref int_type);
choir_type_ref choir_type_pointer(choir_context_ref context);
choir_type_ref choir_type_function(choir_context_ref context, choir_type_ref return_type, choir_type_ref* parameter_types, ssize_t parameter_types_count, bool is_variadic);
bool choir_type_function_is_variadic(choir_type_ref type_function);
choir_type_ref choir_type_function_return_type_get(choir_type_ref type_function);
ssize_t choir_type_function_parameter_count_get(choir_type_ref type_function);
choir_type_ref choir_type_function_parameter_type_get_at_index(choir_type_ref type_function, ssize_t parameter_index);
void choir_type_function_parameter_types_get(choir_type_ref type_function, choir_type_ref* parameter_types, ssize_t parameter_types_count);
choir_type_ref choir_type_struct(choir_context_ref context, choir_type_ref* field_types, ssize_t field_count, bool is_packed);
void choir_type_struct_set_body(choir_type_ref type_struct, choir_type_ref* field_types, ssize_t field_count, bool is_packed);
bool choir_type_struct_is_packed(choir_type_ref type_struct);
ssize_t choir_type_struct_field_count_get(choir_type_ref type_struct);
choir_type_ref choir_type_struct_field_type_get_at_index(choir_type_ref type_struct, ssize_t field_index);
void choir_type_struct_field_types_get(choir_type_ref type_struct, choir_type_ref* field_types, ssize_t field_types_count);
choir_type_ref choir_type_array(choir_context_ref context, choir_type_ref element_type, ssize_t element_count);
ssize_t choir_type_array_element_count_get(choir_type_ref type_array);
choir_type_ref choir_type_array_element_type_get(choir_type_ref type_array);

CHOIR_EXTERN_C_END

#endif /* CHOIR_TYPE_H_ */
