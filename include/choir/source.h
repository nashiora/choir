#ifndef CHOIR_SOURCE_H_
#define CHOIR_SOURCE_H_

#include <choir/extern_c.h>
#include <choir/types.h>

CHOIR_EXTERN_C_BEGIN

/// @brief Returns the name of the given source.
/// The length of the name is returned through the @c name_length parameter if it is not null.
const char* choir_source_name_get(choir_source_ref source, ssize_t* name_length);

/// @brief Returns the text of the given source.
/// The length of the text is returned through the @c text_length parameter if it is not null.
const char* choir_source_text_get(choir_source_ref source, ssize_t* text_length);

/// @brief Seeks through the given source to find the line and column information at a given byte position in its text.
/// Returns true if the byte position is within the range of the source text, and returns false otherwise.
/// When the byte position is within range each of the following parameters, if not null, is populated with its respective information.
/// When false is returned none of the output parameters will be populated, even if not null.
///
/// @param line_begin Byte offset into the source text representing the first character in the line containing the queried byte position.
/// @param line_end Byte offset into the source text representing the last character in the line containing the queried byte position.
///                 The last character is the newline character which terminates the line unless it is the last line and the source file does not end with a newline character.
///                 When the end of the line is not a newline character, the byte position returned through @c line_end will be one past the last valid character in the source text.
/// @param line_number The 1-based number of the line containing the queried byte position.
/// @param column_number The 1-based number of the column within the line containing the queried byte position.
/// @return True if the byte position is within the range of the source text; false otherwise.
bool choir_source_seek(choir_source_ref source, ssize_t byte_position, ssize_t* line_begin, ssize_t* line_end, int64_t* line_number, int64_t* column_number);

/// @brief Create a Choir source from the contents of a file.
/// @param file_path_length The length of the @c file_path string, or 0 if the string is NUL-terminated.
choir_source_ref choir_source_read_from_file(choir_context_ref context, const char* file_path, ssize_t file_path_length);

/// @brief Create a Choir source from the given name and text.
/// @param name_length The length of the @c name string, or 0 if the string is NUL-terminated.
/// @param text_length The length of the @c text string, or 0 if the string is NUL-terminated.
choir_source_ref choir_source_create(choir_context_ref context, const char* name, ssize_t name_length, const char* text, ssize_t text_length);

CHOIR_EXTERN_C_END

#endif /* CHOIR_SOURCE_H_ */
