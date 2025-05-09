#ifndef CHOIR_BLOCK_H_
#define CHOIR_BLOCK_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

void choir_block_delete(choir_block_ref block);
choir_value_ref choir_block_as_value(choir_block_ref block);
choir_block_ref choir_value_as_block(choir_value_ref value_block);
const char* choir_block_name_get(choir_block_ref block, ssize_t* name_length);
choir_value_ref choir_block_parent_get(choir_block_ref block);
bool choir_block_is_terminated(choir_block_ref block);
choir_instruction_ref choir_block_terminator_get(choir_block_ref block);
void choir_block_terminator_set(choir_block_ref block, choir_instruction_ref instruction_terminator);
choir_block_ref choir_block_append(choir_value_ref value_function, const char* block_name, ssize_t block_name_length);
choir_block_ref choir_block_insert_before(choir_value_ref value_function, choir_block_ref relative_block, const char* block_name, ssize_t block_name_length);
choir_block_ref choir_block_insert_after(choir_value_ref value_function, choir_block_ref relative_block, const char* block_name, ssize_t block_name_length);
void choir_block_move_before(choir_block_ref block, choir_block_ref relative_block);
void choir_block_move_after(choir_block_ref block, choir_block_ref relative_block);
void choir_block_remove(choir_block_ref block);
ssize_t choir_block_instruction_count_get(choir_block_ref block);
choir_instruction_ref choir_block_instruction_get_at_index(choir_block_ref block);
void choir_block_instructions_get(choir_block_ref block, choir_instruction_ref* instructions, ssize_t instructions_count);
choir_value_ref choir_block_parameter_add(choir_block_ref block, const char* parameter_name, ssize_t parameter_name_length, choir_type_ref parameter_type);
ssize_t choir_block_parameter_count_get(choir_block_ref block);
choir_value_ref choir_block_parameter_get_at_index(choir_block_ref block);
void choir_block_parameters_get(choir_block_ref block, choir_value_ref* parameters, ssize_t parameters_count);

CHOIR_EXTERN_C_END

#endif /* CHOIR_BLOCK_H_ */
