#ifndef __ZTHREADING_ALT__
#define __ZTHREADING_ALT__

#include "zerynth.h"

typedef struct _mbedtls_mutex {
    VSemaphore mtx;
} mbedtls_threading_mutex_t;

#endif 
