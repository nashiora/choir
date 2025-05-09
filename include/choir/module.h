#ifndef CHOIR_MODULE_H_
#define CHOIR_MODULE_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

choir_module_ref choir_module_create(choir_context_ref context, const char* id, ssize_t id_length);
void choir_module_destroy(choir_module_ref module);
choir_context_ref choir_module_context_get(choir_module_ref module);
const char* choir_module_print_to_string(choir_module_ref module, ssize_t* string_length);
const char* choir_module_identifier_get(choir_module_ref module, ssize_t* id_length);
void choir_module_identifier_set(choir_module_ref module, const char* id, ssize_t id_length);
const char* choir_module_source_name_get(choir_module_ref module, ssize_t* source_name_length);
void choir_module_source_name_set(choir_module_ref module, const char* source_name, ssize_t source_name_length);
choir_value_ref choir_module_function_add(choir_module_ref module, const char* function_name, ssize_t function_name_length, choir_type_ref function_type);
choir_value_ref choir_module_named_function_get(choir_module_ref module, const char* function_name, ssize_t function_name_length);
ssize_t choir_module_function_count_get(choir_module_ref module);
choir_value_ref choir_module_function_get_at_index(choir_module_ref module, ssize_t index);
choir_value_ref choir_module_global_add(choir_module_ref module, const char* global_name, ssize_t global_name_length, choir_type_ref type);
choir_value_ref choir_module_named_global_get(choir_module_ref module, const char* global_name, ssize_t global_name_length);
ssize_t choir_module_global_count_get(choir_module_ref module);
choir_value_ref choir_module_global_get_at_index(choir_module_ref module, ssize_t index);
choir_value_ref choir_module_intrinsic_decl_get(choir_module_ref module, choir_intrinsic_kind intrinsic, choir_type_ref* parameter_types, ssize_t parameter_types_count);

CHOIR_EXTERN_C_END

#endif /* CHOIR_MODULE_H_ */
