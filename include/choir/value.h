#ifndef CHOIR_VALUE_H_
#define CHOIR_VALUE_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

choir_value_ref choir_value_const_poison(choir_type_ref type);
choir_value_ref choir_value_const_nullptr(choir_context_ref context);
choir_value_ref choir_value_const_zeroes(choir_type_ref type);
choir_value_ref choir_value_const_ones(choir_type_ref type);
choir_value_ref choir_value_const_int(choir_type_ref type, uint64_t value, bool sign_extend);
choir_value_ref choir_value_const_array(choir_type_ref element_type, choir_value_ref* element_values, ssize_t element_values_count);
const char* choir_value_print_to_string(choir_value_ref value, ssize_t* string_length);
choir_value_kind choir_value_kind_get(choir_value_ref value);
choir_type_ref choir_value_type_get(choir_value_ref value);
void choir_value_global_delete(choir_value_ref value_global);
choir_value_ref choir_value_global_initializer_get(choir_value_ref value_global);
void choir_value_global_initializer_set(choir_value_ref value_global, choir_value_ref initializer);
bool choir_value_global_is_externally_initialized(choir_value_ref value_global);
void choir_value_global_is_externally_initialized_set(choir_value_ref value_global, bool is_externally_initialized);
bool choir_value_global_is_constant(choir_value_ref value_global);
void choir_value_global_is_constant_set(choir_value_ref value_global, bool is_constant);
choir_type_ref choir_value_global_actual_type_get(choir_value_ref value_global);
choir_linkage choir_value_global_linkage_get(choir_value_ref value_global);
void choir_value_global_linkage_set(choir_value_ref value_global, choir_linkage linkage);
const char* choir_value_global_section_get(choir_value_ref value_global, ssize_t* linkage_length);
void choir_value_global_section_set(choir_value_ref value_global, const char* section, ssize_t linkage_length);
choir_calling_convention choir_value_function_calling_convention_get(choir_value_ref value_function);
void choir_value_function_calling_convention_set(choir_value_ref value_function, choir_calling_convention calling_convention);
ssize_t choir_value_function_block_count_get(choir_value_ref value_function);
choir_block_ref choir_value_function_block_get_at_index(choir_value_ref value_function, ssize_t block_index);
void choir_value_function_blocks_get(choir_value_ref value_function, choir_block_ref* blocks, ssize_t block_count);
choir_block_ref choir_value_function_entry_block_get(choir_value_ref value_function);
ssize_t choir_value_size_get(choir_value_ref value);
ssize_t choir_value_align_get(choir_value_ref value);
void choir_value_align_set(choir_value_ref value, ssize_t preferred_align);
ssize_t choir_value_user_count_get(choir_value_ref value);
choir_instruction_ref choir_value_user_get_at_index(choir_value_ref value, ssize_t user_index);
void choir_value_replace_all_uses(choir_value_ref old_value, choir_value_ref new_value);

CHOIR_EXTERN_C_END

#endif /* CHOIR_VALUE_H_ */
