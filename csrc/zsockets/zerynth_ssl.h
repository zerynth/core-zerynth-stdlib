#ifndef ZERYNTH_SSL_H
#define ZERYNTH_SSL_H



#if defined(ZERYNTH_SSL) || defined(NATIVE_MBEDTLS)
#include "mbedtls/ssl.h"
#endif
#include "../hwcrypto/zerynth_hwcrypto.h"

#if !defined(ZERYNTH_SSL_MAX_SOCKS)
#define MAX_SSLSOCKS 2
#else
#define MAX_SSLSOCKS ZERYNTH_SSL_MAX_SOCKS
#endif

#define _CERT_NONE 1
#define _CERT_OPTIONAL 2
#define _CERT_REQUIRED 4
#define _CLIENT_AUTH 8
#define _SERVER_AUTH 16


typedef struct _sslinfo {
    uint8_t* cacert;
    uint8_t* clicert;
    uint8_t* pvkey;
    uint8_t* hostname;
    uint16_t cacert_len;
    uint16_t clicert_len;
    uint16_t pvkey_len;
    uint16_t hostname_len;
    uint32_t options;
} SSLInfo;



#if defined(ZERYNTH_SSL)
#if !defined(ZERYNTH_SSL_MBEDTLS) && !defined(ZERYNTH_SSL_WOLFSSL)
//set default tls stack to mbedtls
#define ZERYNTH_SSL_MBEDTLS 1
#endif

#if defined(ZERYNTH_SSL_MBEDTLS)
#include "zerynth_ssl_mbedtls.h"
#endif


#endif


#if defined(ZERYNTH_SSL)

#define IS_SECURE_SOCKET(s) (_zsocket_info[s].idx>=0)
#define GET_SECURE_SOCKET(s) (&sslsocks[_zsocket_info[s].idx])
#define SET_SECURE_SOCKET(s,sslsock_idx) (_zsocket_info[s].idx=sslsock_idx)
#define UNSET_SECURE_SOCKET(s) (_zsocket_info[s].idx=-1)

//ZSSL API
int zssl_init(void);
int zssl_socket(int domain, int type, int protocol, void *ctx);
int zssl_connect(int s, const struct sockaddr *name, socklen_t namelen);
int zssl_close(int s);
int zssl_recv(int s, void *mem, size_t len, int flags);
int zssl_read(int s, void *mem, size_t len);
int zssl_send(int s, const void *dataptr, size_t size, int flags);
int zssl_write(int s, const void *dataptr, size_t size);
int zssl_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int zssl_select(int maxfdp1, void *readset, void *writeset, void *exceptset, struct timeval *timeout);

#endif

#endif
