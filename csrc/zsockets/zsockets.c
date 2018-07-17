#include "zerynth.h"
#include "vhal.h"
#include "vbl.h"
#include "zerynth_sockets.h"
#pragma GCC diagnostic ignored "-Wpointer-sign"

#if defined(ZERYNTH_SSL)

#include "mbedtls/platform.h"
#include "mbedtls/net.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/error.h"
#include "mbedtls/certs.h"
#include "mbedtls/threading.h"

#define _CERT_NONE 1
#define _CERT_OPTIONAL 2
#define _CERT_REQUIRED 4
#define _CLIENT_AUTH 8
#define _SERVER_AUTH 16

typedef struct _sslsock {
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_ssl_context ssl;
    mbedtls_x509_crt cacert;
    mbedtls_x509_crt clicert;
    mbedtls_pk_context pkey;
    mbedtls_ssl_config conf;
    mbedtls_net_context ctx;
    int32_t family;
    int32_t socktype;
    int32_t proto;
    uint8_t assigned;
    uint8_t initialized;
} SSLSock;

#define MAX_SSLSOCKS 2
#define SSLSOCK_OFFSET 8192

SSLSock sslsocks[MAX_SSLSOCKS];


int mbedtls_full_connect(SSLSock* ssock, const struct sockaddr* name, socklen_t namelen);
int mbedtls_full_close(SSLSock* ssock);


void _tls_uninit(SSLSock* ssock){
    if (ssock->initialized) {
        //destroy and recreate
        mbedtls_entropy_free(&ssock->entropy);
        mbedtls_ssl_config_free(&ssock->conf);
        mbedtls_ctr_drbg_free(&ssock->ctr_drbg);
        mbedtls_x509_crt_free(&ssock->cacert);
        mbedtls_x509_crt_free(&ssock->clicert);
        mbedtls_pk_free(&ssock->pkey);
        mbedtls_ssl_free(&ssock->ssl);
        ssock->initialized = 0;
    }
    ssock->assigned=0;
}

#endif //ZERYNTH_SSL

int gzsock_socket(int domain, int type, int protocol, void *info){
    int32_t i;
    int32_t sslsock_fd=0;
    int32_t err=0;
#if defined(ZERYNTH_SSL)
    SSLSock* sslsock = NULL;
#endif

    printf("gzsock_socket %i %i %i %x\n",domain,type,protocol,info);
#if defined(ZERYNTH_SSL)
    if (info) {
        SSLInfo *sinfo = (SSLInfo*)info;
        //search free SSL
        for (i = 0; i < MAX_SSLSOCKS; i++) {
            if (!sslsocks[i].assigned) {
                sslsock = &sslsocks[i];
                sslsock_fd = SSLSOCK_OFFSET + i;
                break;
            }
        }
        printf("sslsock %i %x\n",sslsock_fd,sslsock);
        if (!sslsock) {
            //No free ssl sockets: set errno to EMFILE
            return -1;
        }

        sslsock->family = domain;
        sslsock->socktype = type;
        sslsock->proto = protocol;


        if (!sslsock->initialized) {
            mbedtls_ssl_init(&sslsock->ssl);
            mbedtls_x509_crt_init(&sslsock->cacert);
            mbedtls_x509_crt_init(&sslsock->clicert);
            mbedtls_pk_init(&sslsock->pkey);
            mbedtls_ctr_drbg_init(&sslsock->ctr_drbg);
            mbedtls_ssl_config_init(&sslsock->conf);
            mbedtls_entropy_init(&sslsock->entropy);
            if ((err = mbedtls_ctr_drbg_seed(&sslsock->ctr_drbg, mbedtls_entropy_func, &sslsock->entropy, vhalNfoGetUIDStr(), vhalNfoGetUIDLen())) != 0) {
                printf("-%i\n", err);
                return err;
            }

            printf("cacert len %i\n",sinfo->cacert_len);
            if (sinfo->cacert_len) {
                err = mbedtls_x509_crt_parse(&sslsock->cacert, sinfo->cacert, sinfo->cacert_len);
                if (err!=0) {
                    return err;
                }
            }

            printf("hostname len %i\n",sinfo->hostname_len);
            if (sinfo->hostname_len && sinfo->cacert_len) {
                /* Hostname set here should match CN in server certificate */
                char *temphost = (char*)gc_malloc(sinfo->hostname_len+1);
                __memcpy(temphost,sinfo->hostname,sinfo->hostname_len);
                temphost[sinfo->hostname_len]=0;
                err = mbedtls_ssl_set_hostname(&sslsock->ssl, temphost);
                gc_free(temphost);
                if (err!=0){
                    printf("-+%i\n", err);
                    return err;
                }
            }

            if ((err = mbedtls_ssl_config_defaults(&sslsock->conf,
                     (sinfo->options&_CLIENT_AUTH) ? MBEDTLS_SSL_IS_SERVER:MBEDTLS_SSL_IS_CLIENT,
                     MBEDTLS_SSL_TRANSPORT_STREAM,
                     MBEDTLS_SSL_PRESET_DEFAULT))
                != 0) {
                printf("--%i\n", err);
                return err;
            }

            mbedtls_ssl_conf_authmode(
                    &sslsock->conf,
                    (sinfo->options&_CERT_NONE) ? MBEDTLS_SSL_VERIFY_NONE: ((sinfo->options&_CERT_OPTIONAL) ? MBEDTLS_SSL_VERIFY_OPTIONAL:MBEDTLS_SSL_VERIFY_REQUIRED));

            if (!(sinfo->options&_CERT_NONE)) mbedtls_ssl_conf_ca_chain(&sslsock->conf, &sslsock->cacert, NULL);
            if (sinfo->clicert_len) {
                if (sinfo->pvkey_len == 0) {
                    // only ec dummy key supported at the moment
                    if (zhwcrypto_info != NULL && zhwcrypto_info->key_type != ZHWCRYPTO_KEY_ECKEY)
                        return ERR_UNSUPPORTED_EXC;
                    if (zhwcrypto_info == NULL)
                        return ERR_UNSUPPORTED_EXC;
                    const mbedtls_pk_info_t *pk_info;
                    pk_info = mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY );
                    sslsocks->pkey.pk_info = NULL; // to make pk_setup work
                    mbedtls_pk_setup(&sslsock->pkey, pk_info);
                }
                else {
                    err = mbedtls_pk_parse_key(&sslsock->pkey,sinfo->pvkey,sinfo->pvkey_len,NULL,0);
                    if (err) {
                        return err;
                    }   
                }
                err = mbedtls_x509_crt_parse(&sslsock->clicert, sinfo->clicert, sinfo->clicert_len);
                if (err) {
                    return err;
                }

                err = mbedtls_ssl_conf_own_cert(&sslsock->conf,&sslsock->clicert,&sslsock->pkey);
                if (err) {
                    return err;
                }
            }

            mbedtls_ssl_conf_rng(&sslsock->conf, mbedtls_ctr_drbg_random, &sslsock->ctr_drbg);

            if ((err = mbedtls_ssl_setup(&sslsock->ssl, &sslsock->conf)) != 0) {
                printf("---%i\n", err);
                return err;
            }
            sslsock->initialized = 1;
        }
        int llsock = zsock_socket(domain,type,protocol);
        if (llsock<0) {
            //No free sockets
            _tls_uninit(sslsock);
            return -1;
        } else {
            mbedtls_net_init(&sslsock->ctx);
            sslsock->ctx.fd = llsock;
        }

        sslsock->assigned = 1;
        return sslsock_fd;
    } else
#endif //ZERYNTH_SSL
    {
        //standard socket
        return zsock_socket(domain,type,protocol);
    }
}


#if defined(ZERYNTH_SSL)
int mbedtls_full_connect(SSLSock* ssock, const struct sockaddr* name, socklen_t namelen)
{
    int ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    int tt =0;
    mbedtls_net_context* ctx = &ssock->ctx;


    if ((tt=zsock_connect(ctx->fd, name, namelen)) != 0) {
        mbedtls_net_free(ctx);
        printf("full connect %x\n",tt);
        return MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    mbedtls_ssl_set_bio(&ssock->ssl, ctx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    while ((ret = mbedtls_ssl_handshake(&ssock->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            mbedtls_ssl_session_reset(&ssock->ssl);
            mbedtls_net_free(ctx);
            printf("full connect 2 %x\n",ret);
            return ret;
        }
    }

    return 0;
}


void mbedtls_mutex_init_alt(mbedtls_threading_mutex_t *mutex){
    mutex->mtx = vosSemCreate(1);
}
void mbedtls_mutex_free_alt(mbedtls_threading_mutex_t *mutex){
    vosSemDestroy(mutex->mtx);

}
int mbedtls_mutex_lock_alt( mbedtls_threading_mutex_t *mutex){
    vosSemWait(mutex->mtx);
    return 0;
}
int mbedtls_mutex_unlock_alt( mbedtls_threading_mutex_t *mutex ){
    vosSemSignalCap(mutex->mtx,1);
    return 0;
}



void * mbedtls_gc_calloc( size_t n, size_t m){
    uint8_t *res = gc_malloc(n*m);
    return res;
}

void mbedtls_gc_free( void *pnt){
    if (pnt) {
        gc_free(pnt);
    }
}


int mbedtls_hardware_poll( void *data, unsigned char *output, size_t len, size_t *olen )
{
    (void) data;
    (void) output;
    (void) len;
    (void) olen;
    /* Fill output buffer */
    for (uint32_t i = 0, tmp_rnd = 0; i < len; i++)
    {
        /* Get 4B random number */
        if ((i & 0x3) == 0)
            tmp_rnd = vhalRngGenerate();
        output[i] = ((uint8_t*)&tmp_rnd)[i & 0x3];
    }
    *olen = len;
    return 0;

}
#endif //ZERYNTH_SSL

SocketAPIPointers *socket_api_pointers;

SocketAPIPointers *gzsock_init(SocketAPIPointers *pointers) {
    SocketAPIPointers *old = socket_api_pointers;
    socket_api_pointers = pointers;
#if defined(ZERYNTH_SSL)
    mbedtls_platform_set_calloc_free(mbedtls_gc_calloc,mbedtls_gc_free);
    mbedtls_threading_set_alt(
            mbedtls_mutex_init_alt,
            mbedtls_mutex_free_alt,
            mbedtls_mutex_lock_alt,
            mbedtls_mutex_unlock_alt);
#endif
    return old;
}



#define IS_SECURE_SOCKET(s) ((s)>=SSLSOCK_OFFSET)
#define GET_SECURE_SOCKET(s) (&sslsocks[((s)-SSLSOCK_OFFSET)])

int gzsock_connect(int s, const struct sockaddr *name, socklen_t namelen){
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        if(mbedtls_full_connect(sslsock,name,namelen)){
            return -1;
        } else {
            return 0;
        }
    } else
#endif
    {
        return zsock_connect(s,name,namelen);
    }
}

// int gzsock_accept(int s, struct sockaddr *addr, socklen_t *addrlen){
//     if (IS_SECURE_SOCKET(s)) {
//         SSLSock *sslsock = GET_SECURE_SOCKET(s);
//         return mbedtls_net_accept(sslsock,addr);
//     } else {
//         return zsock_connect(s,name,namelen);
//     }
// }
// int gzsock_bind(int s, const struct sockaddr *name, socklen_t namelen);
// int gzsock_shutdown(int s, int how);
// int gzsock_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
#if defined(ZERYNTH_SSL)
int mbedtls_full_close(SSLSock* ssock){

    mbedtls_ssl_close_notify(&ssock->ssl);
    mbedtls_ssl_session_reset(&ssock->ssl);
    mbedtls_net_free(&ssock->ctx);
    _tls_uninit(ssock);

    return 0;
}
#endif

int gzsock_close(int s){
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        return mbedtls_full_close(sslsock);
    } else
#endif
    {
        return zsock_close(s);
    }
}
// int gzsock_listen(int s, int backlog);
int gzsock_recv(int s, void *mem, size_t len, int flags){
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        return mbedtls_ssl_read(&sslsock->ssl,mem,len);
    } else
#endif
    {
        return zsock_recv(s,mem,len,flags);
    }
}
int gzsock_read(int s, void *mem, size_t len){
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        return mbedtls_ssl_read(&sslsock->ssl,mem,len);
    } else
#endif
    {
        return zsock_read(s,mem,len);
    }
}
// int gzsock_recvfrom(int s, void *mem, size_t len, int flags,struct sockaddr *from, socklen_t *fromlen);
int gzsock_send(int s, const void *dataptr, size_t size, int flags){
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        return mbedtls_ssl_write(&sslsock->ssl,dataptr,size);
    } else
#endif
    {
        return zsock_send(s,dataptr,size,flags);
    }

}
// int gzsock_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
int gzsock_write(int s, const void *dataptr, size_t size){
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        return mbedtls_ssl_write(&sslsock->ssl,dataptr,size);
    } else
#endif
    {
        return zsock_write(s,dataptr,size);
    }

}

int gzsock_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen){
    int thesock=-1;
#if defined(ZERYNTH_SSL)
    SSLSock *sslsock = NULL;
    printf("setsockopt %i %x %x\n",s,level,optname);
    if (IS_SECURE_SOCKET(s)) {
        sslsock = GET_SECURE_SOCKET(s);
        thesock = sslsock->ctx.fd;
    } else
#endif
    {
        thesock=s;
    }

#if defined(ZERYNTH_SSL)
    if (optname == SO_RCVTIMEO && sslsock) {
        //handle timeout for sslsockets
        struct timeval *tms=(struct timeval*)optval;
        printf("tm %i %i\n",tms->tv_sec,tms->tv_usec);
        mbedtls_ssl_conf_read_timeout(&sslsock->conf, tms->tv_sec*1000+tms->tv_usec/1000);
        return 0;
    }
#endif
    return zsock_setsockopt(thesock, level, optname, optval, optlen);
}


