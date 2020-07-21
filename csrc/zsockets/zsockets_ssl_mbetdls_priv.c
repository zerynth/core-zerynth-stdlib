/*
 * Implementation of private functions for MBEDTLS stack used by zssl_ api
 */
#include "zerynth_sockets.h"
#include "zerynth.h" // last position since some defines might cause conflicts (ERR_OK with lwIP, opcodes)
#include "zerynth_sockets_debug.h"
#pragma GCC diagnostic ignored "-Wpointer-sign"

#if defined(ZERYNTH_SSL) && defined(ZERYNTH_SSL_MBEDTLS)


#if defined(ZERYNTH_SSL_DEBUG)
void mbedtls_f_dbg(void *ctx, int lvl, const char *file, int line, const char* message){
    vbl_printf_stdout("%s:%i:%i %s",file,line,lvl,message);
}
#endif

#if defined(ZERYNTH_SSL_STATIC_BUFFERS) && defined(ZERYNTH_SSL_STATIC_MEMORY)
#error "Can't use ZERYNTH_SSL_STATIC_BUFFERS and ZERYNTH_SSL_STATIC_MEMORY together"
#endif

#if defined(ZERYNTH_SSL_STATIC_MEMORY)
#if !defined(ZERYNTH_SSL_STATIC_MEMORY_BYTES)
#define ZERYNTH_SSL_STATIC_MEMORY_BYTES 32768
#endif
unsigned char _ssl_memory_buf[ZERYNTH_SSL_STATIC_MEMORY_BYTES];
#endif //STATIC MEMORY


int mbedtls_full_connect(SSLSock* ssock, const struct sockaddr* name, socklen_t namelen)
{
    int ret = MBEDTLS_ERR_NET_UNKNOWN_HOST;
    int tt =0;
    mbedtls_net_context* ctx = &ssock->ctx;

    if ((tt=zsock_connect(ctx->fd, name, namelen)) != 0) {
        ERROR("creating socket %i => %i",ctx->fd,tt);
        UNSET_SECURE_SOCKET(ctx->fd);
        mbedtls_net_free(ctx);
        mbedtls_uninit(ssock);
        return MBEDTLS_ERR_NET_CONNECT_FAILED;
    }

    mbedtls_ssl_set_bio(&ssock->ssl, ctx, mbedtls_net_send, mbedtls_net_recv, mbedtls_net_recv_timeout);

    while ((ret = mbedtls_ssl_handshake(&ssock->ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ && ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
            ERROR("in SSL handshake %i %x",ret,ret);
            UNSET_SECURE_SOCKET(ctx->fd);
            mbedtls_ssl_session_reset(&ssock->ssl);
            mbedtls_net_free(ctx);
            mbedtls_uninit(ssock);
            return ret;
        }
    }

    return 0;
}

#if !defined(ZERYNTH_SSL_EXTERNAL_STACK)

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
#endif

#if defined(ZERYNTH_SSL_STATIC_BUFFERS)
#if defined(ZERYNTH_SSL_STATIC_BUFFERS_NUM)
#define SSL_BUFFERS_NUM (2*(ZERYNTH_SSL_STATIC_BUFFERS_NUM))
#else
#define SSL_BUFFERS_NUM (2*MAX_SSLSOCKS)
#endif
typedef struct _sslbuf {
    uint8_t buffer[MBEDTLS_SSL_BUFFER_LEN];
    uint8_t taken;
} SSLBuffer;
SSLBuffer _sslbuffers[SSL_BUFFERS_NUM];
#endif

void * mbedtls_gc_calloc( size_t n, size_t m){
#if defined(ZERYNTH_SSL_STATIC_BUFFERS)
    if((n*m)==MBEDTLS_SSL_BUFFER_LEN) {
        int i;
        for (i=0;i<SSL_BUFFERS_NUM;i++){
            if(!_sslbuffers[i].taken) {
                //it's free
                _sslbuffers[i].taken=1;
                memset(_sslbuffers[i].buffer,0,MBEDTLS_SSL_BUFFER_LEN);
                return _sslbuffers[i].buffer;
            }
        }
        return NULL;
    }
#endif
    uint8_t *res = gc_malloc(n*m);
    return res;
}

void mbedtls_gc_free( void *pnt){
    if (pnt) {
#if defined(ZERYNTH_SSL_STATIC_BUFFERS)
        //TODO: remove for, use mem comparison  pnt>=_sslbuffers && pnt<_sslbuffer+sizeof()
        int i;
        for (i=0;i<SSL_BUFFERS_NUM;i++){
            if (pnt==(void*)_sslbuffers[i].buffer && _sslbuffers[i].taken==1) {
                _sslbuffers[i].taken=0;
                return;
            }
        }
#endif
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


int mbedtls_full_close(SSLSock* ssock){

    mbedtls_ssl_close_notify(&ssock->ssl);
    mbedtls_ssl_session_reset(&ssock->ssl);
    mbedtls_net_free(&ssock->ctx);
    mbedtls_uninit(ssock);

    return 0;
}

void mbedtls_uninit(SSLSock* ssock){
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


#endif
