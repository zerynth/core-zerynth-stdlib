#ifndef __LANGDEFS__
#define __LANGDEFS__


#include "vbl.h"

/* Numbers */

#define Z_DOUBLE_FP 1
#define Z_INT64_VM  1

#if !defined(Z_DOUBLE_FP)
//float VM
#define FLOAT_TYPE  float
#define FLOAT_ZERO 0.0f
#define FLOAT_HALF 0.5f
#define FLOAT_ONE 1.0f
#define FLOAT_FLOOR(a) floorf(a)
#define FLOAT_POW(a,b) powf(a,b)
#define FLOAT_TRUNC(a) truncf(a)
#define FLOAT_ROUND(a) FLOAT_TRUNC(a+(FLOAT_HALF))
#define FLOAT_SQRT(a) sqrtf(a)
#define FLOAT_MAX 3.40282347e+38f
#define ATOF(str) strtof()
#else
//double VM
#define FLOAT_TYPE  double
#define FLOAT_ZERO 0.0
#define FLOAT_HALF 0.5
#define FLOAT_ONE 1.0
#define FLOAT_FLOOR(a) floor(a)
#define FLOAT_POW(a,b) pow(a,b)
#define FLOAT_TRUNC(a) trunc(a)
#define FLOAT_ROUND(a) FLOAT_TRUNC(a+(FLOAT_HALF))
#define FLOAT_SQRT(a) sqrt(a)
#define FLOAT_MAX 1.79769313486231571e+308
#define ATOF(str) strtod(str,NULL)
#endif


#define LONG_SIZE 4

#if defined(Z_INT64_VM)
//int64 VM
#define INT_TYPE int64_t
#define UINT_TYPE uint64_t
#define INT_TYPE_MAX INT64_MAX
#define INT_TYPE_MIN INT64_MIN

#else
//int32_t VM
#define INT_TYPE int32_t
#define UINT_TYPE uint32_t
#define INT_TYPE_MAX INT32_MAX
#define INT_TYPE_MIN INT32_MIN

#endif

//does it fit in a VM 32 bit number? ==> is a int32_t?
#define IS_I32B(i) (((i)>=INT32_MIN)&&(i<=0x7fffffff))
//does it fit in a 32 bit unsigned? ==> is a uint32_t  Warning: this fits in VM 64 bit number!
#define IS_U32B(i) (((i)<=INT32_MAX)&&(i>=0))





/* Internal Types */
typedef uint16_t Name;
typedef INT_TYPE zint_t;
typedef UINT_TYPE zuint_t;
typedef FLOAT_TYPE zfloat_t;
#endif

