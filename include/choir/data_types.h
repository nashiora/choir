#ifndef CHOIR_DATA_TYPES_H_
#define CHOIR_DATA_TYPES_H_

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

#ifdef _MSC_VER
#  ifdef _WIN64
typedef signed long long int ssize_t;
#  else
typedef signed int ssize_t;
#  endif /* _WIN64 */
#endif /* _MSC_VER */

#endif /* CHOIR_DATA_TYPES_H_ */
