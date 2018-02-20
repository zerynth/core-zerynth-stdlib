#include "zerynth.h"
#include "sha3.h"


//#define printf(...) vbl_printf_stdout(__VA_ARGS__)

#define SHA224 0
#define SHA256 1
#define SHA384 2
#define SHA512 3


C_NATIVE(zs_keccak_init){
    NATIVE_UNWARN();
    uint32_t hashtype;
    if (parse_py_args("i", nargs, args, &hashtype)!= 1) {
        return ERR_TYPE_EXC;
    }
    RELEASE_GIL();
    PBytes *bres = pbytes_new(sizeof(cf_sha3_context),NULL);
    switch(hashtype){
        case SHA224: cf_keccak_224_init(PSEQUENCE_BYTES(bres)); break;
        case SHA256: cf_keccak_256_init(PSEQUENCE_BYTES(bres)); break;
        case SHA384: cf_keccak_384_init(PSEQUENCE_BYTES(bres)); break;
        case SHA512: cf_keccak_512_init(PSEQUENCE_BYTES(bres)); break;
    }
    ACQUIRE_GIL();
    *res = bres;
    
    return ERR_OK;
}


C_NATIVE(zs_keccak_update){
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
    switch(hashtype){
        case SHA224: cf_sha3_224_update(pctx,data,data_len); break;
        case SHA256: cf_sha3_256_update(pctx,data,data_len); break;
        case SHA384: cf_sha3_384_update(pctx,data,data_len); break;
        case SHA512: cf_sha3_512_update(pctx,data,data_len); break;
    }
    ACQUIRE_GIL();
    *res = args[1];

    return ERR_OK;
}


C_NATIVE(zs_keccak_digest){
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
    switch(hashtype){
        case SHA224: bres = pbytes_new(CF_SHA3_224_HASHSZ,NULL); cf_sha3_224_digest(pctx,PSEQUENCE_BYTES(bres)); break;
        case SHA256: bres = pbytes_new(CF_SHA3_256_HASHSZ,NULL); cf_sha3_256_digest(pctx,PSEQUENCE_BYTES(bres)); break;
        case SHA384: bres = pbytes_new(CF_SHA3_384_HASHSZ,NULL); cf_sha3_384_digest(pctx,PSEQUENCE_BYTES(bres)); break;
        case SHA512: bres = pbytes_new(CF_SHA3_512_HASHSZ,NULL); cf_sha3_512_digest(pctx,PSEQUENCE_BYTES(bres)); break;
    }
    ACQUIRE_GIL();
    *res=bres;
    return ERR_OK;
}

#include "hash_common.h"

C_NATIVE(zs_keccak_context){
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
            hcc->block_size = CF_SHA3_224_BLOCKSZ;
            hcc->result_size = CF_SHA3_224_HASHSZ;
            hcc->init_hash = cf_keccak_224_init;
            hcc->update_hash = cf_sha3_224_update;
            hcc->finish_hash = cf_sha3_224_digest;
        }
        break;
        case SHA256:{ 
            hcc->block_size = CF_SHA3_256_BLOCKSZ;
            hcc->result_size = CF_SHA3_256_HASHSZ;
            hcc->init_hash = cf_keccak_256_init;
            hcc->update_hash = cf_sha3_256_update;
            hcc->finish_hash = cf_sha3_256_digest;
        }
        break;
        case SHA384:{ 
            hcc->block_size = CF_SHA3_384_BLOCKSZ;
            hcc->result_size = CF_SHA3_384_HASHSZ;
            hcc->init_hash = cf_keccak_384_init;
            hcc->update_hash = cf_sha3_384_update;
            hcc->finish_hash = cf_sha3_384_digest;
        }
        break;
        case SHA512:{ 
            hcc->block_size = CF_SHA3_512_BLOCKSZ;
            hcc->result_size = CF_SHA3_512_HASHSZ;
            hcc->init_hash = cf_keccak_512_init;
            hcc->update_hash = cf_sha3_512_update;
            hcc->finish_hash = cf_sha3_512_digest;
        }
        break;
    }

    memcpy(hcc->ctx,pctx,ctx_len);

    *res = bres;
    return ERR_OK;
}
