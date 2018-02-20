#include "zerynth.h"

typedef unsigned int MD5_u32plus;

typedef struct {
    MD5_u32plus lo, hi;
    MD5_u32plus a, b, c, d;
    unsigned char buffer[64];
    MD5_u32plus block[16];
} MD5_CTX;

extern void MD5_Init(MD5_CTX *ctx);
extern void MD5_Update(MD5_CTX *ctx, const void *data, unsigned long size);
extern void MD5_Final(unsigned char *result, MD5_CTX *ctx);

void MD5_Digest(MD5_CTX *ctx, uint8_t*result){
    MD5_Final(result,ctx);
}

C_NATIVE(zs_md5_init){
    NATIVE_UNWARN();

    RELEASE_GIL();    
    PBytes *bres = pbytes_new(sizeof(MD5_CTX),NULL);
    MD5_Init(PSEQUENCE_BYTES(bres));
    ACQUIRE_GIL();
    *res = bres;

    return ERR_OK;
}


C_NATIVE(zs_md5_update){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;

    if (parse_py_args("ss", nargs, args, &pctx, &ctx_len, &data, &data_len)!= 2) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    MD5_Update(pctx,data,data_len);
    ACQUIRE_GIL();
    *res = args[0];

    return ERR_OK;
}


C_NATIVE(zs_md5_digest){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;
    uint8_t hash[16];

    if (parse_py_args("s", nargs, args, &pctx, &ctx_len)!= 1) {
        return ERR_TYPE_EXC;
    }
    RELEASE_GIL();
    MD5_Final(hash, pctx);
    ACQUIRE_GIL();
    *res = pbytes_new(16,hash);
    return ERR_OK;
}

#include "hash_common.h"

C_NATIVE(zs_md5_context){
    NATIVE_UNWARN();
    uint8_t *pctx;
    uint32_t ctx_len;

    if (parse_py_args("s", nargs, args, &pctx, &ctx_len)!= 1) {
        return ERR_TYPE_EXC;
    }

    PBytes *bres = pbytes_new(sizeof(HashCommonContext)+ctx_len,NULL);
    HashCommonContext *hcc = (HashCommonContext*)PSEQUENCE_BYTES(bres);
    hcc->block_size = 64;;
    hcc->result_size = 16;
    hcc->init_hash = MD5_Init;
    hcc->update_hash = MD5_Update;
    hcc->finish_hash = MD5_Digest;

    memcpy(hcc->ctx,pctx,ctx_len);

    *res = bres;
    return ERR_OK;
}
