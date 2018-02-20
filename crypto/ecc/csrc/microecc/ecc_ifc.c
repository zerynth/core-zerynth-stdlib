#include "uECC.h"
#include "hash_common.h"
#include "zerynth.h"

//#define printf(...) vbl_printf_stdout(__VA_ARGS__)


uint8_t curves_initialized = 0;
const struct uECC_Curve_t* curves[5];

#define ECC_SECP160R1 0
#define ECC_SECP192R1 1
#define ECC_SECP224R1 2
#define ECC_SECP256R1 3
#define ECC_SECP256K1 4
#define ECC_MAX_CURVE 5

int _zc_rng(uint8_t *dest, unsigned size){
    int i;
    for(i=0;i<size;i++){
        dest[i] = vhalRngGenerate()%256;
    }
    return 1;
}

void init_curves()
{

    if (!curves_initialized) {
#if uECC_SUPPORTS_secp160r1
        curves[ECC_SECP160R1] = uECC_secp160r1();
#endif
#if uECC_SUPPORTS_secp192r1
        curves[ECC_SECP192R1] = uECC_secp192r1();
#endif
#if uECC_SUPPORTS_secp224r1
        curves[ECC_SECP224R1] = uECC_secp224r1();
#endif
#if uECC_SUPPORTS_secp256r1
        curves[ECC_SECP256R1] = uECC_secp256r1();
#endif
#if uECC_SUPPORTS_secp256k1
        curves[ECC_SECP256K1] = uECC_secp256k1();
#endif
        uECC_set_rng(_zc_rng);
        curves_initialized = 1;
    }
}

#define CHECK_CURVE() do {                                  \
    init_curves();                                          \
                                                            \
    if ((thecurve < 0 || thecurve >= ECC_MAX_CURVE)         \
        || (curves[thecurve] == NULL)) {                    \
        return ERR_UNSUPPORTED_EXC;                         \
    }                                                       \
} while (0)

C_NATIVE(zc_curve_verify)
{
    NATIVE_UNWARN();
    uint8_t* pbkey;
    uint32_t pbkey_len;
    uint8_t* message;
    uint32_t message_len;
    uint8_t* signature;
    uint32_t signature_len;
    uint32_t thecurve;

    if (parse_py_args("isss", nargs, args, &thecurve, &message, &message_len,
            &signature, &signature_len, &pbkey, &pbkey_len)
        != 4) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];
    if (!uECC_verify(pbkey, message, message_len, signature, curve)) {
        *res = PBOOL_FALSE();
    } else {
        *res = PBOOL_TRUE();
    }
    ACQUIRE_GIL();
    return ERR_OK;
}

C_NATIVE(zc_curve_generate_keys)
{
    NATIVE_UNWARN();
    uint32_t thecurve;
    uint8_t public[64];
    uint8_t private[32];

    if (parse_py_args("i", nargs, args, &thecurve) != 1) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];

    uECC_make_key(public,private,curve);
    PTuple *tpl = ptuple_new(2,NULL);
    PBytes *pbkey = pbytes_new(uECC_curve_public_key_size(curve),public);
    PBytes *pvkey = pbytes_new(uECC_curve_private_key_size(curve),private);
    PTUPLE_SET_ITEM(tpl,0,pbkey);
    PTUPLE_SET_ITEM(tpl,1,pvkey);
    ACQUIRE_GIL();
    *res = tpl;

    return ERR_OK;
}

C_NATIVE(zc_curve_check_pbkey){
    NATIVE_UNWARN();
    uint32_t thecurve;
    uint32_t pblen;
    uint8_t* pbkey;

    if (parse_py_args("is", nargs, args, &thecurve,&pbkey,&pblen) != 2) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];

    if(!uECC_valid_public_key(pbkey,curve)) {
        *res = PBOOL_FALSE();
    } else {
        *res = PBOOL_TRUE();
    }
    ACQUIRE_GIL();
    return ERR_OK;
}


C_NATIVE(zc_curve_derive_pbkey){
    NATIVE_UNWARN();
    uint32_t thecurve;
    uint32_t pvlen;
    uint8_t* pvkey;
    uint8_t public[64];

    if (parse_py_args("is", nargs, args, &thecurve,&pvkey,&pvlen) != 2) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];

    if(!uECC_compute_public_key(pvkey,public,curve)){
        ACQUIRE_GIL();
        return ERR_VALUE_EXC;
    }
    *res = pbytes_new(uECC_curve_public_key_size(curve),public);
    ACQUIRE_GIL();
    return ERR_OK;
}

C_NATIVE(zc_curve_compress_key){
    NATIVE_UNWARN();
    uint32_t thecurve;
    uint32_t pblen;
    uint8_t* pbkey;
    uint8_t public[33];

    if (parse_py_args("is", nargs, args, &thecurve,&pbkey,&pblen) != 2) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];

    uECC_compress(pbkey,public,curve);
    *res = pbytes_new(uECC_curve_public_key_size(curve)/2+1,public);
    ACQUIRE_GIL();
    return ERR_OK;
}

C_NATIVE(zc_curve_decompress_key){
    NATIVE_UNWARN();
    uint32_t thecurve;
    uint32_t pblen;
    uint8_t* pbkey;
    uint8_t public[64];

    if (parse_py_args("is", nargs, args, &thecurve,&pbkey,&pblen) != 2) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];

    uECC_decompress(pbkey,public,curve);
    *res = pbytes_new(uECC_curve_public_key_size(curve),public);
    ACQUIRE_GIL();
    return ERR_OK;
}

typedef struct _generic_hash {
    uECC_HashContext uECC;
    HashCommonContext *ctx;
} GenericHashContext;

void init_generic_hash(uECC_HashContext *base) {
    GenericHashContext *context = (GenericHashContext *)base;
    context->ctx->init_hash(context->ctx->ctx);
}

void update_generic_hash(uECC_HashContext *base,
                   const uint8_t *message,
                   unsigned message_size) {
    GenericHashContext *context = (GenericHashContext *)base;
    context->ctx->update_hash(context->ctx->ctx, message, message_size);
}

void finish_generic_hash(uECC_HashContext *base, uint8_t *hash_result) {
    GenericHashContext *context = (GenericHashContext *)base;
    context->ctx->finish_hash(context->ctx->ctx,hash_result);
}


C_NATIVE(zc_curve_sign){
    NATIVE_UNWARN();
    uint8_t* pvkey;
    uint32_t pvkey_len;
    uint8_t* message;
    uint32_t message_len;
    uint32_t thecurve;
    uint8_t* deterministic;
    uint32_t deterministic_len;
    uint8_t recid;
    uint8_t signature[64];

    if (parse_py_args("isss", nargs, args, &thecurve, &message, &message_len, &pvkey, &pvkey_len,&deterministic,
                &deterministic_len)
            != 4) {
        return ERR_TYPE_EXC;
    }

    CHECK_CURVE();
    RELEASE_GIL();
    const struct uECC_Curve_t* curve = curves[thecurve];

    if (deterministic_len) {
        HashCommonContext * hcc = (HashCommonContext *)deterministic;
        GenericHashContext gECC;
        uint8_t *tmp = gc_malloc(hcc->result_size*2+hcc->block_size);
        gECC.uECC.block_size = hcc->block_size;
        gECC.uECC.result_size = hcc->result_size;
        gECC.uECC.tmp = tmp;
        gECC.uECC.init_hash = init_generic_hash;
        gECC.uECC.update_hash = update_generic_hash;
        gECC.uECC.finish_hash = finish_generic_hash;
        gECC.ctx = hcc;
        if (!uECC_sign_deterministic_recoverable(pvkey,message,message_len,&gECC.uECC,&recid,signature,curve)){
            gc_free(tmp);
            ACQUIRE_GIL();
            return ERR_VALUE_EXC;
        }
        gc_free(tmp);
    } else {
        if(!uECC_sign_recoverable(pvkey,message,message_len,&recid,signature,curve)) {
            ACQUIRE_GIL();
            return ERR_VALUE_EXC;
        }
    }
    ACQUIRE_GIL();
    PTuple *tpl = ptuple_new(2,NULL);
    PTUPLE_SET_ITEM(tpl,0,PSMALLINT_NEW(recid));
    PBytes *pbs = pbytes_new(uECC_curve_public_key_size(curve),signature);
    PTUPLE_SET_ITEM(tpl,1,pbs);
    *res = tpl;
    return ERR_OK;
}
