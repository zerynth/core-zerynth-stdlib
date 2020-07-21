#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/ssl_internal.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/threading.h"


typedef struct _sslsock {
    int32_t family;
    int32_t socktype;
    int32_t proto;
    uint8_t assigned;
    uint8_t initialized;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
    mbedtls_ssl_config conf;
    mbedtls_net_context ctx;
} SSLSock;

int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen );
void mbedtls_gc_free( void *pnt);
void * mbedtls_gc_calloc( size_t n, size_t m);
int mbedtls_full_connect(SSLSock* ssock, const struct sockaddr* name, socklen_t namelen);
int mbedtls_full_close(SSLSock* ssock);
void mbedtls_uninit(SSLSock* ssock);
void mbedtls_f_dbg(void *ctx, int lvl, const char *file, int line, const char* message);
#if !defined(ZERYNTH_SSL_EXTERNAL_STACK)
void mbedtls_mutex_init_alt(mbedtls_threading_mutex_t *mutex);
void mbedtls_mutex_free_alt(mbedtls_threading_mutex_t *mutex);
int mbedtls_mutex_lock_alt( mbedtls_threading_mutex_t *mutex);
int mbedtls_mutex_unlock_alt( mbedtls_threading_mutex_t *mutex );
#endif

extern SSLSock sslsocks[MAX_SSLSOCKS];

#include "zerynth_mbedtls_config.h"
