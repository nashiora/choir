#ifndef CHOIR_TYPES_H_
#define CHOIR_TYPES_H_

#include <choir/data_types.h>
#include <choir/extern_c.h>

CHOIR_EXTERN_C_BEGIN

typedef struct choir_opaque_buffer* choir_buffer_ref;

typedef struct choir_opaque_context* choir_context_ref;

typedef struct choir_opaque_source* choir_source_ref;

typedef struct choir_opaque_module* choir_module_ref;

typedef struct choir_opaque_type* choir_type_ref;

typedef struct choir_opaque_value* choir_value_ref;

typedef struct choir_opaque_instruction* choir_instruction_ref;

typedef struct choir_opaque_inst* choir_inst_ref;

typedef struct choir_opaque_block* choir_block_ref;

typedef struct choir_opaque_builder* choir_builder_ref;

typedef struct choir_opaque_attribute* choir_attribute_ref;

typedef enum choir_diagnostic_level {
    CHOIR_DIAG_IGNORED,
    CHOIR_INFO,
    CHOIR_REMARK,
    CHOIR_WARNING,
    CHOIR_ERROR,
    CHOIR_FATAL,
} choir_diagnostic_level;

typedef enum choir_opcode {
    CHOIR_OPCODE_INVALID,

    CHOIR_RETURN_VOID,
    CHOIR_RETURN,
    CHOIR_JUMP,
    CHOIR_BRANCH,
} choir_opcode;

typedef enum choir_type_kind {
    CHOIR_VOID_TYPE,
    CHOIR_INTEGER_TYPE,
    CHOIR_POINTER_TYPE,
    CHOIR_ARRAY_TYPE,
    CHOIR_FUNCTION_TYPE,
    CHOIR_STRUCT_TYPE,
} choir_type_kind;

typedef enum choir_value_kind {
    CHOIR_VALUE_POISON,
    CHOIR_VALUE_INSTRUCTION_RESULT,
    CHOIR_VALUE_FUNCTION,
    CHOIR_VALUE_FUNCTION_PARAMETER,
    CHOIR_VALUE_BLOCK,
    CHOIR_VALUE_BLOCK_PARAMETER,
    CHOIR_VALUE_FRAME_SLOT,
} choir_value_kind;

typedef enum choir_linkage {
    CHOIR_LINKAGE_EXTERNAL,
    CHOIR_LINKAGE_INTERNAL,
    CHOIR_LINKAGE_PRIVATE,
} choir_linkage;

typedef enum choir_calling_convention {
    CHOIR_CALLCONV_C,
    CHOIR_CALLCONV_X86_STDCALL,
    CHOIR_CALLCONV_X86_FASTCALL,
} choir_calling_convention;

typedef enum choir_integer_predicate {
    CHOIR_INT_EQ,
} choir_integer_predicate;

typedef enum choir_tail_call_kind {
    CHOIR_TAIL_NONE,
} choir_tail_call_kind;

typedef enum choir_intrinsic_kind {
    CHOIR_MEMSET,
} choir_intrinsic_kind;

typedef int32_t choir_attribute_index;

CHOIR_EXTERN_C_END

#endif /* CHOIR_TYPES_H_ */
