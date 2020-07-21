/*
 * Implementation of zssl_ functions for MBEDTLS stack
 */

#include "zerynth_sockets.h"
#include "zerynth.h" // last position since some defines might cause conflicts (ERR_OK with lwIP, opcodes)
#include "zerynth_sockets_debug.h"
#pragma GCC diagnostic ignored "-Wpointer-sign"

#if defined(ZERYNTH_SSL) && defined(ZERYNTH_SSL_MBEDTLS)


SSLSock sslsocks[MAX_SSLSOCKS];



int zssl_init(){
#if !defined(ZERYNTH_SSL_STATIC_MEMORY)
    //set zerynth calloc/free
#if !defined(ZERYNTH_SSL_EXTERNAL_STACK)
    DEBUG(LVL0,"Initializing MBEDTLS memory functions","");
    mbedtls_platform_set_calloc_free(mbedtls_gc_calloc,mbedtls_gc_free);
#endif
#else
    //static memory
    DEBUG(LVL0,"Initializing MBEDTLS static memory","");
    mbedtls_memory_buffer_alloc_init( _ssl_memory_buf, sizeof(_ssl_memory_buf) );
#endif
#if defined(ZERYNTH_SSL_DEBUG)
    #if !defined(ZERYNTH_SSL_EXTERNAL_STACK)
    mbedtls_debug_set_threshold( ZERYNTH_SSL_DEBUG );
    mbedtls_platform_set_snprintf(snprintf);
    #endif
#endif
#if !defined(ZERYNTH_SSL_EXTERNAL_STACK)
    DEBUG(LVL0,"Initializing MBEDTLS threading","");
    mbedtls_threading_set_alt(
            mbedtls_mutex_init_alt,
            mbedtls_mutex_free_alt,
            mbedtls_mutex_lock_alt,
            mbedtls_mutex_unlock_alt);
#endif
    return 0;
}

mbedtls_x509_crt_profile mbedtls_x509_crt_profile_custom =
{
// #if defined(ZERYNTH_SSL_ALLOW_SHA1_IN_CERTIFICATES)
    /* Allow SHA-1 (weak, but still safe in controlled environments) */
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA1 ) |
// #endif
    /* Only SHA-2 hashes */
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA224 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA256 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA384 ) |
    MBEDTLS_X509_ID_FLAG( MBEDTLS_MD_SHA512 ),
    0xFFFFFFF, /* Any PK alg    */
    0xFFFFFFF, /* Any curve     */
    ZERYNTH_SSL_PROFILE_RSA_MIN_BITS,
};


int zssl_socket(int domain, int type, int protocol, void *ctx) {
    int i;
    int sslsock_idx;
    int err;
    SSLInfo *sinfo = (SSLInfo*)ctx;
    SSLSock* sslsock = NULL;

    DEBUG(LVL0,"args %i %i %i %x",domain,type,protocol,ctx);
    DEBUG0("crypto %x %x %x %x %x",&zhwcrypto,zhwcrypto.api,zhwcrypto.nfo,zhwcrypto.capi,zhwcrypto.cnfo);
    if (!sinfo) {
        ERROR("No SSL context given","");
        return -1;
    }

    //search free SSL
    for (i = 0; i < MAX_SSLSOCKS; i++) {
        if (!sslsocks[i].assigned) {
            sslsock = &sslsocks[i];
            sslsock_idx = i;
            break;
        }
    }
    if (!sslsock) {
        //No free ssl sockets: set errno to EMFILE
        ERROR("No more SSL sockets, max %i",MAX_SSLSOCKS);
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
#if defined(ZERYNTH_SSL_DEBUG)
#if !defined(ZERYNTH_SSL_EXTERNAL_STACK)
        mbedtls_ssl_conf_dbg(&sslsock->conf,mbedtls_f_dbg,NULL);
#else
#endif
#endif
        if ((err = mbedtls_ctr_drbg_seed(&sslsock->ctr_drbg, mbedtls_entropy_func, &sslsock->entropy, vhalNfoGetUIDStr(), vhalNfoGetUIDLen())) != 0) {
            ERROR("Can't initialized drbg seed %i %x",err,err);
            return err;
        }

        gzcrypto_hw_disable();

        DEBUG(LVL0,"CA certificate length of %i bytes",sinfo->cacert_len);
        if (sinfo->cacert_len) {
            err = mbedtls_x509_crt_parse(&sslsock->cacert, sinfo->cacert, sinfo->cacert_len);
            if (err!=0) {
                ERROR("Can't parse CA certificate %i %x",err,err);
                return err;
            }
        }

        DEBUG(LVL0,"hostname length of %i bytes",sinfo->hostname_len);
        if (sinfo->hostname_len && sinfo->cacert_len) {
            /* Hostname set here should match CN in server certificate */
            char *temphost = (char*)gc_malloc(sinfo->hostname_len+1);
            __memcpy(temphost,sinfo->hostname,sinfo->hostname_len);
            temphost[sinfo->hostname_len]=0;
            err = mbedtls_ssl_set_hostname(&sslsock->ssl, temphost);
            gc_free(temphost);
            if (err!=0){
                ERROR("Can't parse CA hostname %i %x",err,err);
                return err;
            }
        }

        if ((err = mbedtls_ssl_config_defaults(&sslsock->conf,
                 (sinfo->options&_CLIENT_AUTH) ? MBEDTLS_SSL_IS_SERVER:MBEDTLS_SSL_IS_CLIENT,
                 MBEDTLS_SSL_TRANSPORT_STREAM,
                 MBEDTLS_SSL_PRESET_DEFAULT))
            != 0) {
            ERROR("Can't set SSL defaults %i %x",err,err);
            return err;
        }
        mbedtls_ssl_conf_cert_profile(&sslsock->conf, &mbedtls_x509_crt_profile_custom);
        mbedtls_ssl_conf_authmode(
                &sslsock->conf,
                (sinfo->options&_CERT_NONE) ? MBEDTLS_SSL_VERIFY_NONE: ((sinfo->options&_CERT_OPTIONAL) ? MBEDTLS_SSL_VERIFY_OPTIONAL:MBEDTLS_SSL_VERIFY_REQUIRED));

        if (!(sinfo->options&_CERT_NONE)) mbedtls_ssl_conf_ca_chain(&sslsock->conf, &sslsock->cacert, NULL);
        if (sinfo->clicert_len) {
            DEBUG(LVL0,"Client certificate length of %i bytes",sinfo->clicert_len);
            DEBUG(LVL0,"Private key length of %i bytes",sinfo->pvkey_len);
            if (sinfo->pvkey_len == 0) {
                gzcrypto_hw_enable();
                if( zhwcrypto.api == NULL || zhwcrypto.nfo == NULL) {
                    ERROR("HW Crypto not configured","");
                    return -1;
                }
                if( zhwcrypto.nfo->key_type != ZHWCRYPTO_KEY_ECKEY)  {
                    ERROR("HW Crypto does not support ECC crypto","");
                    return -1;
                }
                //Tell MBEDTLS that we are using an EC key
                const mbedtls_pk_info_t *pk_info;
                pk_info = mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY );
                sslsocks->pkey.pk_info = NULL; // to make pk_setup work
                mbedtls_pk_setup(&sslsock->pkey, pk_info);
            } else {
                err = mbedtls_pk_parse_key(&sslsock->pkey,sinfo->pvkey,sinfo->pvkey_len,NULL,0);
                if (err) {
                    ERROR("Can't parse private key %i %x",err,err);
                    return err;
                }   
            }
            err = mbedtls_x509_crt_parse(&sslsock->clicert, sinfo->clicert, sinfo->clicert_len);
            if (err) {
                ERROR("Can't parse client certificate %i %x",err,err);
                return err;
            }

            err = mbedtls_ssl_conf_own_cert(&sslsock->conf,&sslsock->clicert,&sslsock->pkey);
            if (err) {
                ERROR("Can't set own certificate %i %x",err,err);
                return err;
            }
        }

        mbedtls_ssl_conf_rng(&sslsock->conf, mbedtls_ctr_drbg_random, &sslsock->ctr_drbg);
        if ((err = mbedtls_ssl_setup(&sslsock->ssl, &sslsock->conf)) != 0) {
            ERROR("Can't setup SSL %i %x",err,err);
            return err;
        }
        sslsock->initialized = 1;
    }
    int llsock = zsock_socket(domain,type,protocol);
    if (llsock<0) {
        //No free sockets
        mbedtls_uninit(sslsock);
        ERROR("Can't create socket %i %x",llsock,llsock);
        return -1;
    } else {
        mbedtls_net_init(&sslsock->ctx);
        sslsock->ctx.fd = llsock;
        SET_SECURE_SOCKET(llsock,sslsock_idx);
        DEBUG(LVL0,"Created secure socket %i at index %i",llsock,sslsock_idx);
    }

    sslsock->assigned = 1;
    return llsock;
}

int zssl_connect(int s, const struct sockaddr *name, socklen_t namelen){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    DEBUG(LVL0,"args %i %x %i",s,name,namelen);
    if(mbedtls_full_connect(sslsock,name,namelen)){
        return -1;
    } else {
        return 0;
    }
}

int zssl_close(int s){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    UNSET_SECURE_SOCKET(s);
    DEBUG(LVL0,"args %i",s);
    return mbedtls_full_close(sslsock);
}

int zssl_recv(int s, void *mem, size_t len, int flags){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    int rc = mbedtls_ssl_read(&sslsock->ssl,mem,len);
    DEBUG(LVL0,"args %i %x %i %i => %i %x",s,mem,len,flags,rc,rc);
    if (rc == MBEDTLS_ERR_SSL_TIMEOUT) rc=-ETIMEDOUT;
    return rc;
}

int zssl_read(int s, void *mem, size_t len){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    int rc = mbedtls_ssl_read(&sslsock->ssl,mem,len);
    DEBUG(LVL0,"args %i %x %i => %i %x",s,mem,len,rc,rc);
    if (rc == MBEDTLS_ERR_SSL_TIMEOUT) rc=-ETIMEDOUT;
    return rc;
}

int zssl_send(int s, const void *dataptr, size_t size, int flags){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    int rc = mbedtls_ssl_write(&sslsock->ssl,dataptr,size);
    DEBUG(LVL0,"args %i %x %i %i => %i %x",s,dataptr,size,flags,rc,rc);
    return rc;
}

int zssl_write(int s, const void *dataptr, size_t size){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    int rc = mbedtls_ssl_write(&sslsock->ssl,dataptr,size);
    DEBUG(LVL0,"args %i %x %i => %i %x",s,dataptr,size,rc,rc);
    return rc;
}

int zssl_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen){
    SSLSock *sslsock = GET_SECURE_SOCKET(s);
    DEBUG(LVL0,"args %i %i %i %x %i",s,level,optname,optval,optlen);
    if (optname == SO_RCVTIMEO) {
        //handle timeout for sslsockets
        struct timeval *tms=(struct timeval*)optval;
        DEBUG(LVL0,"setting SO_RCVTIMEO of %i sec and %i musec",tms->tv_sec,tms->tv_usec);
        mbedtls_ssl_conf_read_timeout(&sslsock->conf, tms->tv_sec*1000+tms->tv_usec/1000);
        return 0;
    } else {
        //set underlying
        return zsock_setsockopt(s,level,optname,optval,optlen);
    }
    return 0;
}

int zssl_select(int maxfdp1, void *readset, void *writeset, void *exceptset, struct timeval *timeout){
    SSLSock *sslsock = GET_SECURE_SOCKET(maxfdp1-1);
    
    DEBUG(LVL0,"args %i %x %x %x %x",maxfdp1, readset,writeset,exceptset,timeout);
    //are there bytes already read and not consumed in the ssl buffer? -> return sock ready
    if (sslsock->ssl.in_offt!=NULL) {
        DEBUG(LVL0,"Returning on SSL buffer not empty","");
        return 1;
    }

    //let's wait for real...
    return zsock_select(maxfdp1,readset,writeset,exceptset,timeout);
}


#endif //ZERYNTH_SSL



