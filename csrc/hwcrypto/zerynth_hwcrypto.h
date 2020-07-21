
#ifndef ZERYNTH_HWCRYPTO_H_
#define ZERYNTH_HWCRYPTO_H_

#include <stddef.h>
#include <stdint.h>

typedef struct _zhwcrypto_api_pointers
{
    int (*ec_get_pubkey)( uint8_t* pubkey_buff );
    int (*ecdsa_secp256r1_sign)(const unsigned char *hash, size_t hash_len, unsigned char *sig, size_t *sig_len);
    int (*digest_sha256)( const unsigned char *input, size_t ilen, unsigned char *output );
    void (*digest_sha256_starts)(void *ctx);
    void (*digest_sha256_update)(void *ctx, const unsigned char *input, size_t ilen);
    void (*digest_sha256_finish)(void *ctx, unsigned char output[32]);
    void*(*digest_sha256_ctx_alloc)(void);
    void (*digest_sha256_ctx_free)(void *ctx);
} ZHWCryptoAPIPointers;

typedef enum {
    ZHWCRYPTO_KEY_NONE=0,
    ZHWCRYPTO_KEY_RSA,
    ZHWCRYPTO_KEY_ECKEY,
    ZHWCRYPTO_KEY_ECKEY_DH,
    ZHWCRYPTO_KEY_ECDSA,
    ZHWCRYPTO_KEY_RSA_ALT,
    ZHWCRYPTO_KEY_RSASSA_PSS,
} zhwcrypto_key_type_t;

typedef struct _zhwcrypto_info
{
    zhwcrypto_key_type_t key_type;
} ZHWCryptoInfo;

typedef struct _zhwcrypto_ {
    //fields used by mbedtls
    ZHWCryptoInfo *nfo;
    ZHWCryptoAPIPointers *api;
    //fields used to store api when crypto disabled
    ZHWCryptoInfo *cnfo;
    ZHWCryptoAPIPointers *capi;
} ZHWCrypto;

extern ZHWCrypto zhwcrypto;


#define ZERYNTH_HWCRYPTO_ENABLED()  (zhwcrypto.api)
#define ZERYNTH_HWCRYPTO_HAS_PUB()  (zhwcrypto.api->ec_get_pubkey)
#define ZERYNTH_HWCRYPTO_HAS_SIGN() (zhwcrypto.api->ecdsa_secp256r1_sign)
#define ZERYNTH_HWCRYPTO_HAS_HASH() (zhwcrypto.api->digest_sha256)
#define ZERYNTH_HWCRYPTO_API() (zhwcrypto.api)
#define ZERYNTH_HWCRYPTO_NFO() (zhwcrypto.nfo)

void gzcrypto_hw_init(ZHWCryptoAPIPointers *pointers, ZHWCryptoInfo *nfo);
void gzcrypto_hw_disable();
void gzcrypto_hw_enable();




#endif
