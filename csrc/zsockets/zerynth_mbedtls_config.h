#ifndef _ZERYNTH_SSL_MBEDTLS_CONFIG
#define _ZERYNTH_SSL_MBEDTLS_CONFIG


//define ZERYNTH_SSL_CUSTOM_CONFIG_INCLUDE to allow custom configuration
//of tls stack with the below header
#if defined(ZERYNTH_SSL_CUSTOM_CONFIG_INCLUDE)
#include "zerynth_mbedtls_custom_config.h"
#endif

//The minimum allowed number of bits for RSA keys  (default 2048)
#ifndef ZERYNTH_SSL_PROFILE_RSA_MIN_BITS
#define ZERYNTH_SSL_PROFILE_RSA_MIN_BITS 2048
#endif

//Control if sha256 is performed with hwcrypto when available (default yes)
// #define ZERYNTH_HWCRYPTO_ENABLE_SHA256
#if defined(ZERYNTH_HWCRYPTO_DISABLE_SHA256)
#undef  ZERYNTH_HWCRYPTO_ENABLE_SHA256
#endif

//Enabled to get MBEDTLS stack inner debug
//Not valid for external tls stack
//It must have a value between 0 and 4, corresponding to the desired level of debug
#if defined(ZERYNTH_SSL_DEBUG)
#define MBEDTLS_DEBUG_C
#endif

//static memory: tbd
#if defined(ZERYNTH_SSL_STATIC_MEMORY)
#define MBEDTLS_MEMORY_BUFFER_ALLOC_C
#endif


#if defined(ZERYNTH_SSL_MAX_CONTENT_LEN)
#define MBEDTLS_SSL_MAX_CONTENT_LEN ZERYNTH_SSL_MAX_CONTENT_LEN
#else
#define MBEDTLS_SSL_MAX_CONTENT_LEN             8192 /**< Maxium fragment length in bytes, determines the size of each of the two internal I/O buffers */
#endif

#if defined(ZERYNTH_SSL_CIPHERSUITES_SHA256)
#define MBEDTLS_SSL_CIPHERSUITES \
MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, \
MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,  \
MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,   \
MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,    \
MBEDTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA256, \
MBEDTLS_TLS_ECDH_ECDSA_WITH_AES_128_CBC_SHA256,  \
MBEDTLS_TLS_ECDHE_RSA_WITH_AES_128_CBC_SHA256,   \
MBEDTLS_TLS_ECDH_RSA_WITH_AES_128_CBC_SHA256,    \
MBEDTLS_TLS_RSA_WITH_AES_128_GCM_SHA256,         \
MBEDTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,     \
MBEDTLS_TLS_DHE_RSA_WITH_AES_128_CBC_SHA256,     \
MBEDTLS_TLS_DHE_RSA_WITH_AES_256_CBC_SHA256
#endif


#endif

