#ifndef CHOIR_CORE_H_
#define CHOIR_CORE_H_

#include <choir/extern_c.h>
#include <choir/types.h>

#include <choir/block.h>
#include <choir/builder.h>
#include <choir/context.h>
#include <choir/diag.h>
#include <choir/instruction.h>
#include <choir/intrinsic.h>
#include <choir/module.h>
#include <choir/source.h>
#include <choir/type.h>
#include <choir/value.h>

CHOIR_EXTERN_C_BEGIN

/// @brief Return the four-component Choir library version into each of the out parameters.
/// If a parameter is null, it is simply ignored and skipped.
void choir_version_get(int32_t* release, int32_t* major, int32_t* minor, int32_t* patch);

/// @brief Free string memory allocated by the Choir API.
/// This is only required in some rare circumstances, so check the documentation of each string-returning function to see if an explicit free is required.
/// Most string memory is copied into the Choir library or otherwise owned by its systems, and so does not need freed with this function.
void choir_string_free(const char* string);

CHOIR_EXTERN_C_END

#endif /* CHOIR_CORE_H_ */
