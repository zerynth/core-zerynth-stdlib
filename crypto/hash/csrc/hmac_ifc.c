#include "zerynth.h"
#include "hash_common.h"
#include "hmac.h"
#include "chash.h"

//#define printf(...) vbl_printf_stdout(__VA_ARGS__)

C_NATIVE(zs_hmac_init){
    NATIVE_UNWARN();

    uint8_t *hash_ctx;
    uint32_t hash_ctx_len;   
    uint8_t *key;
    uint32_t key_len;

    if (parse_py_args("ss", nargs, args, &key, &key_len,&hash_ctx, &hash_ctx_len)!= 2) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    PBytes *bres = pbytes_new(sizeof(cf_chash)+sizeof(cf_hmac_ctx),NULL);

    cf_hmac_ctx *hmac_ctx = (cf_hmac_ctx*)(PSEQUENCE_BYTES(bres)+sizeof(cf_chash));
    cf_chash     *hash_ifc = (cf_chash*) PSEQUENCE_BYTES(bres);
    HashCommonContext *hcc = (HashCommonContext*)hash_ctx;
    hash_ifc->hashsz = hcc->result_size;
    hash_ifc->blocksz = hcc->block_size;
    hash_ifc->init = hcc->init_hash;
    hash_ifc->update = hcc->update_hash;
    hash_ifc->digest = hcc->finish_hash;

    cf_hmac_init(hmac_ctx,hash_ifc,key,key_len); 
    *res = bres;
    ACQUIRE_GIL();

    return ERR_OK;
}


C_NATIVE(zs_hmac_update){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;

    if (parse_py_args("ss", nargs, args, &pctx, &ctx_len, &data, &data_len)!= 2) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    cf_hmac_ctx *hmac_ctx = (cf_hmac_ctx*) (pctx+sizeof(cf_chash));
    cf_hmac_update(hmac_ctx,data,data_len);
    ACQUIRE_GIL();
    *res = args[0];

    return ERR_OK;
}


C_NATIVE(zs_hmac_digest){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;
    

    if (parse_py_args("s", nargs, args, &pctx, &ctx_len)!= 1) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    cf_hmac_ctx *hmac_ctx = (cf_hmac_ctx*) (pctx+sizeof(cf_chash));
    cf_chash     *hash_ifc = (cf_chash*) pctx;
    PBytes *bres = pbytes_new(hash_ifc->hashsz,NULL);
   
    cf_hmac_finish(hmac_ctx, PSEQUENCE_BYTES(bres));
    ACQUIRE_GIL();
    *res = bres;
    return ERR_OK;
}




