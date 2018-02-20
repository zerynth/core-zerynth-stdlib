
#define XZMALLOC zbn_zmalloc
#define XMALLOC zbn_malloc
#define XCALLOC zbn_calloc
#define XREALLOC zbn_realloc
#define XFREE zbn_free

extern void *XZMALLOC(void *p, size_t size);

//to avoid having mp_word of 64 bit
#define MP_16BIT

