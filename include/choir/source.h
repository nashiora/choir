#ifndef CHOIR_CORE_H_
#define CHOIR_CORE_H_

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

CHOIR_EXTERN_C_END

#endif /* CHOIR_CORE_H_ */
