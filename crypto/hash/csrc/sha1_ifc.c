#include "zerynth.h"
#include "sha1.h"
#include "hash_common.h"

//#define printf(...) vbl_printf_stdout(__VA_ARGS__)

C_NATIVE(zs_sha1_init){
    NATIVE_UNWARN();
    
    RELEASE_GIL();
    PBytes *bres = pbytes_new(sizeof(cf_sha1_context),NULL);

    cf_sha1_init(PSEQUENCE_BYTES(bres));
    ACQUIRE_GIL();
    *res = bres;

    return ERR_OK;
}


C_NATIVE(zs_sha1_update){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;

    if (parse_py_args("ss", nargs, args, &pctx, &ctx_len, &data, &data_len)!= 2) {
        return ERR_TYPE_EXC;
    }
    RELEASE_GIL();
    cf_sha1_update(pctx,data,data_len);
    ACQUIRE_GIL();
    
    *res = args[0];

    return ERR_OK;
}


C_NATIVE(zs_sha1_digest){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;

    if (parse_py_args("s", nargs, args, &pctx, &ctx_len)!= 1) {
        return ERR_TYPE_EXC;
    }
    RELEASE_GIL();
    PBytes *bres = pbytes_new(CF_SHA1_HASHSZ,NULL);
    cf_sha1_digest(pctx, PSEQUENCE_BYTES(bres));
    ACQUIRE_GIL();
    *res = bres;
    return ERR_OK;
}



C_NATIVE(zs_sha1_context){
    NATIVE_UNWARN();
    uint8_t *pctx;
    uint32_t ctx_len;

    if (parse_py_args("s", nargs, args, &pctx, &ctx_len)!= 1) {
        return ERR_TYPE_EXC;
    }

    PBytes *bres = pbytes_new(sizeof(HashCommonContext)+ctx_len,NULL);
    HashCommonContext *hcc = (HashCommonContext*)PSEQUENCE_BYTES(bres);
    hcc->block_size = CF_SHA1_BLOCKSZ;
    hcc->result_size = CF_SHA1_HASHSZ;
    hcc->init_hash = cf_sha1_init;
    hcc->update_hash = cf_sha1_update;
    hcc->finish_hash = cf_sha1_digest;

    memcpy(hcc->ctx,pctx,ctx_len);

    *res = bres;
    return ERR_OK;
}
