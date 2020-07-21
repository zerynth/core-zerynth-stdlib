/*
* @Author: Lorenzo
* @Date:   2018-05-30 10:15:30
* @Last Modified by:   Lorenzo
* @Last Modified time: 2018-06-27 10:00:24
*/

#define ZERYNTH_PRINTF
#include "zerynth.h"
#include "zerynth_hwcrypto.h"
#include "mbedtls/x509_csr.h"
#include "mbedtls/pk_internal.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"

#define CSR_BUFSIZE 2048

static int generate_csr_for_key(char *subject, PString **csr, mbedtls_pk_context *key,
                            int (*f_rng)(void *, unsigned char *, size_t), void *p_rng) {
    int ret = 0;
    uint8_t* buf = gc_malloc(CSR_BUFSIZE);

    mbedtls_x509write_csr req;
    mbedtls_x509write_csr_init(&req);

    mbedtls_x509write_csr_set_md_alg(&req, MBEDTLS_MD_SHA256);
    mbedtls_x509write_csr_set_key(&req, key);
    ret = mbedtls_x509write_csr_set_subject_name(&req, subject);
    if (ret < 0) {
        goto exit;
    }

    ret = mbedtls_x509write_csr_pem(&req, buf, CSR_BUFSIZE, f_rng, p_rng);
    if (ret != 0) {
        goto exit;
    }
    else {
        *csr = pstring_new(strlen((const char *)buf) + 1, buf);
    }

exit:

    gc_free(buf);
    mbedtls_x509write_csr_free(&req);
    return ret;
}

#if !defined(HAS_BUILTIN_MBEDTLS)
void * mbedtls_gc_calloc( size_t n, size_t m);
void mbedtls_gc_free( void *pnt);
void mbedtls_mutex_init_alt(mbedtls_threading_mutex_t *mutex);
void mbedtls_mutex_free_alt(mbedtls_threading_mutex_t *mutex);
int mbedtls_mutex_lock_alt( mbedtls_threading_mutex_t *mutex);
int mbedtls_mutex_unlock_alt( mbedtls_threading_mutex_t *mutex);
#endif

#if !defined(ZERYNTH_SSL) && !defined(HAS_BUILTIN_MBEDTLS)
// when ZERYNTH_SSL is defined these functions are defined in zsockets.c
// when HAS_BUILTIN_MBEDTLS there is no need for these functions
void * mbedtls_gc_calloc( size_t n, size_t m){
    uint8_t *res = gc_malloc(n*m);
    return res;
}
void mbedtls_gc_free( void *pnt){
    if (pnt) {
        gc_free(pnt);
    }
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
#endif



C_NATIVE(_generate_csr_for_key) {
    NATIVE_UNWARN();

    PString *csr;
    uint8_t* subject;
    uint32_t subject_len;
    uint8_t* key_str;
    uint32_t key_len;

    const mbedtls_pk_info_t *pk_info;
    mbedtls_pk_context key;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;

    mbedtls_pk_init(&key);

    int (*f_rng)(void *, unsigned char *, size_t) = NULL;
    void *p_rng = NULL;

    int ret = ERR_OK;

    if (parse_py_args("ss", nargs, args, &key_str, &key_len, &subject, &subject_len) != 2) {
        return ERR_TYPE_EXC;
    }

    if (key_len == 0) {
        if (ZERYNTH_HWCRYPTO_ENABLED() && ZERYNTH_HWCRYPTO_NFO()->key_type != ZHWCRYPTO_KEY_ECKEY){
            return ERR_UNSUPPORTED_EXC;
        }
        if (!ZERYNTH_HWCRYPTO_ENABLED())
            return ERR_UNSUPPORTED_EXC;
    }

#ifndef HAS_BUILTIN_MBEDTLS
    mbedtls_platform_set_calloc_free(mbedtls_gc_calloc,mbedtls_gc_free);
#endif
    if (key_len != 0) {
        // not a dummy key

#ifndef HAS_BUILTIN_MBEDTLS
        mbedtls_threading_set_alt(
                mbedtls_mutex_init_alt,
                mbedtls_mutex_free_alt,
                mbedtls_mutex_lock_alt,
                mbedtls_mutex_unlock_alt);
#endif
        mbedtls_pk_parse_key(&key, key_str, key_len, NULL, 0);

        mbedtls_ctr_drbg_init(&ctr_drbg);
        mbedtls_entropy_init(&entropy);

        mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, vhalNfoGetUIDStr(), vhalNfoGetUIDLen());
        mbedtls_ctr_drbg_set_prediction_resistance(&ctr_drbg, MBEDTLS_CTR_DRBG_PR_ON);

        f_rng = &mbedtls_ctr_drbg_random;
        p_rng = &ctr_drbg;
    }
    else {
        // prepare dummy key
        pk_info = mbedtls_pk_info_from_type( MBEDTLS_PK_ECKEY );
        mbedtls_pk_setup(&key, pk_info);
    }

    if (generate_csr_for_key(subject, &csr, &key, f_rng, p_rng) != 0) {
        ret = ERR_IOERROR_EXC;
    }

    *res = csr;

    if (key_len != 0) {
        mbedtls_entropy_free(&entropy);
        mbedtls_ctr_drbg_free(&ctr_drbg);
    }

    return ret;
}
