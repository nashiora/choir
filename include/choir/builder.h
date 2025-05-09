#ifndef CHOIR_INSTRUCTION_H_
#define CHOIR_INSTRUCTION_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

choir_builder_ref choir_builder_create(choir_context_ref context);
void choir_builder_destroy(choir_builder_ref builder);
void choir_builder_position_at_end(choir_builder_ref builder, choir_block_ref block);

choir_instruction_ref choir_build_return_void(choir_builder_ref builder);
choir_instruction_ref choir_build_return(choir_builder_ref builder, choir_value_ref value);
choir_instruction_ref choir_build_jump(choir_builder_ref builder, choir_block_ref block_target);
choir_instruction_ref choir_build_branch(choir_builder_ref builder, choir_value_ref value_condition, choir_block_ref block_target_true, choir_block_ref block_target_false);

CHOIR_EXTERN_C_END

#endif /* CHOIR_INSTRUCTION_H_ */
