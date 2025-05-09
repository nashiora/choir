#ifndef CHOIR_CONTEXT_H_
#define CHOIR_CONTEXT_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

/// @brief Create a new Choir context.
choir_context_ref choir_context_create();

/// @brief Destroy the given Choir context, freeing all of the memory it owns.
void choir_context_destroy(choir_context_ref context);

/// @brief Get a reference to the global Choir context.
choir_context_ref choir_context_get_global();

/// @brief Create a Choir source from the contents of a file.
/// @param file_path_length The length of the @c file_path string, or 0 if the string is NUL-terminated.
choir_source_ref choir_context_source_read_from_file(choir_context_ref context, const char* file_path, ssize_t file_path_length);

/// @brief Create a Choir source from the given name and text.
/// @param name_length The length of the @c name string, or 0 if the string is NUL-terminated.
/// @param text_length The length of the @c text string, or 0 if the string is NUL-terminated.
choir_source_ref choir_context_source_create(choir_context_ref context, const char* name, ssize_t name_length, const char* text, ssize_t text_length);

CHOIR_EXTERN_C_END

#endif /* CHOIR_CONTEXT_H_ */
