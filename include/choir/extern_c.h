#ifndef CHOIR_EXTERN_C_H_
#define CHOIR_EXTERN_C_H_

#ifdef __cplusplus
#  define CHOIR_EXTERN_C_BEGIN extern "C" {
#  define CHOIR_EXTERN_C_END }
#else /* !__cplusplus */
#  define CHOIR_EXTERN_C_BEGIN
#  define CHOIR_EXTERN_C_END
#endif

#endif /* CHOIR_EXTERN_C_H_ */
