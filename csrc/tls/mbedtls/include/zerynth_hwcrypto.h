
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

extern ZHWCryptoAPIPointers *zhwcrypto_api_pointers;
extern ZHWCryptoInfo        *zhwcrypto_info;

// #define zerynth_hwcrypto_ec_get_pubkey(pkey) (zhwcrypto_api_pointers->ec_get_pubkey)(pkey)
#define zerynth_hwcrypto_ec_get_pubkey (zhwcrypto_api_pointers->ec_get_pubkey)
// #define zerynth_hwcrypto_digest_sha256(input, len, output) (zhwcrypto_api_pointers->digest_sha256)(input, len, output)
#define zerynth_hwcrypto_digest_sha256_starts (zhwcrypto_api_pointers->digest_sha256_starts)
#define zerynth_hwcrypto_digest_sha256_update (zhwcrypto_api_pointers->digest_sha256_update)
#define zerynth_hwcrypto_digest_sha256_finish (zhwcrypto_api_pointers->digest_sha256_finish)
#define zerynth_hwcrypto_digest_sha256_ctx_alloc (zhwcrypto_api_pointers->digest_sha256_ctx_alloc)
#define zerynth_hwcrypto_digest_sha256_ctx_free (zhwcrypto_api_pointers->digest_sha256_ctx_free)
#define zerynth_hwcrypto_digest_sha256 (zhwcrypto_api_pointers->digest_sha256)
// #define zerynth_hwcrypto_ecdsa_secp256r1_sign(hash, hashlen, sig, siglen) (zhwcrypto_api_pointers->ecdsa_secp256r1_sign)(hash, hashlen, sig, siglen)
#define zerynth_hwcrypto_ecdsa_secp256r1_sign (zhwcrypto_api_pointers->ecdsa_secp256r1_sign)

#define ZERYNTH_HWCRYPTO_HASPUB()  (zhwcrypto_api_pointers->ec_get_pubkey)
#define ZERYNTH_HWCRYPTO_HASSIGN() (zhwcrypto_api_pointers->ecdsa_secp256r1_sign)
#define ZERYNTH_HWCRYPTO_HASHASH() (zhwcrypto_api_pointers->digest_sha256)

#endif
