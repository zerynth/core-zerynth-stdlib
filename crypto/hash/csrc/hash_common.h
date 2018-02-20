#ifndef __HASH_COMMON__
#define __HASH_COMMON__

typedef struct _hash_common_context {
    void (*init_hash)(void *context);
    void (*update_hash)(void *context, const uint8_t *message, unsigned message_size);
    void (*finish_hash)(void *context, uint8_t *hash_result);
    uint32_t block_size;
    uint32_t result_size;
    uint8_t ctx[];   
} HashCommonContext;


#endif
