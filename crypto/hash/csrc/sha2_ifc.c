#include "zerynth.h"
#include "sha2.h"


//#define printf(...) vbl_printf_stdout(__VA_ARGS__)

#define SHA224 0
#define SHA256 1
#define SHA384 2
#define SHA512 3


C_NATIVE(zs_sha2_init){
    NATIVE_UNWARN();
    uint32_t hashtype;
    if (parse_py_args("i", nargs, args, &hashtype)!= 1) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    if (hashtype<=SHA256) {
        PBytes *bres = pbytes_new(sizeof(cf_sha256_context),NULL);
        if(hashtype==SHA224) cf_sha224_init(PSEQUENCE_BYTES(bres));
        else cf_sha256_init(PSEQUENCE_BYTES(bres));
        *res=bres;
    } else {
        PBytes *bres = pbytes_new(sizeof(cf_sha512_context),NULL);
        if(hashtype==SHA384) cf_sha384_init(PSEQUENCE_BYTES(bres));
        else cf_sha512_init(PSEQUENCE_BYTES(bres));
        *res=bres;
    }
    ACQUIRE_GIL();
    
    return ERR_OK;
}


C_NATIVE(zs_sha2_update){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;
    uint32_t hashtype;

    if (parse_py_args("iss", nargs, args, &hashtype, &pctx, &ctx_len, &data, &data_len)!= 3) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    if (hashtype<=SHA256) {
        if(hashtype==SHA224) cf_sha224_update(pctx,data,data_len);
        else cf_sha256_update(pctx,data,data_len);

    } else {
        if(hashtype==SHA384) cf_sha384_update(pctx,data,data_len);
        else cf_sha512_update(pctx,data,data_len);
    }
    ACQUIRE_GIL();
    *res = args[1];

    return ERR_OK;
}


C_NATIVE(zs_sha2_digest){
    NATIVE_UNWARN();
    
    uint8_t* pctx;
    uint32_t ctx_len;
    uint8_t *data;
    uint32_t data_len;
    uint32_t hashtype;

    if (parse_py_args("is", nargs, args, &hashtype, &pctx, &ctx_len)!= 2) {
        return ERR_TYPE_EXC;
    }

    RELEASE_GIL();
    PBytes *bres;
    if (hashtype<=SHA256) {
        if(hashtype==SHA224) {
            bres = pbytes_new(CF_SHA224_HASHSZ,NULL);
            cf_sha224_digest(pctx,PSEQUENCE_BYTES(bres));
        } else {
            bres = pbytes_new(CF_SHA256_HASHSZ,NULL);
            cf_sha256_digest(pctx,PSEQUENCE_BYTES(bres));
        }

    } else {
        if(hashtype==SHA384) {
            bres = pbytes_new(CF_SHA384_HASHSZ,NULL);
            cf_sha384_digest(pctx,PSEQUENCE_BYTES(bres));
        } else {
            bres = pbytes_new(CF_SHA512_HASHSZ,NULL);
            cf_sha512_digest(pctx,PSEQUENCE_BYTES(bres));
        }
    }
   ACQUIRE_GIL();
   *res = bres; 
    return ERR_OK;
}

#include "hash_common.h"

C_NATIVE(zs_sha2_context){
    NATIVE_UNWARN();
    uint32_t hashtype;
    uint8_t *pctx;
    uint32_t ctx_len;

    if (parse_py_args("is", nargs, args, &hashtype, &pctx, &ctx_len)!= 2) {
        return ERR_TYPE_EXC;
    }

    PBytes *bres = pbytes_new(sizeof(HashCommonContext)+ctx_len,NULL);
    HashCommonContext *hcc = (HashCommonContext*)PSEQUENCE_BYTES(bres);
    switch(hashtype){
        case SHA224:{ 
            hcc->block_size = CF_SHA224_BLOCKSZ;
            hcc->result_size = CF_SHA224_HASHSZ;
            hcc->init_hash = cf_sha224_init;
            hcc->update_hash = cf_sha224_update;
            hcc->finish_hash = cf_sha224_digest;
        }
        break;
        case SHA256:{ 
            hcc->block_size = CF_SHA256_BLOCKSZ;
            hcc->result_size = CF_SHA256_HASHSZ;
            hcc->init_hash = cf_sha256_init;
            hcc->update_hash = cf_sha256_update;
            hcc->finish_hash = cf_sha256_digest;
        }
        break;
        case SHA384:{ 
            hcc->block_size = CF_SHA384_BLOCKSZ;
            hcc->result_size = CF_SHA384_HASHSZ;
            hcc->init_hash = cf_sha384_init;
            hcc->update_hash = cf_sha384_update;
            hcc->finish_hash = cf_sha384_digest;
        }
        break;
        case SHA512:{ 
            hcc->block_size = CF_SHA512_BLOCKSZ;
            hcc->result_size = CF_SHA512_HASHSZ;
            hcc->init_hash = cf_sha512_init;
            hcc->update_hash = cf_sha512_update;
            hcc->finish_hash = cf_sha512_digest;
        }
        break;
    }

    memcpy(hcc->ctx,pctx,ctx_len);

    *res = bres;
    return ERR_OK;
}
