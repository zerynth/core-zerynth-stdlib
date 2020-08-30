
#if defined(ZERYNTH_DEBUG)

#if ZERYNTH_DEBUG>=0
#define ZDEBUG(tag,lvl,maxlvl,fmt,...) do { \
    if ((lvl)<=(maxlvl) &&(lvl)>=0) { \
        vbl_printf_stdout("[DBG]:%i:" tag " @%s:%i | " fmt "\n",lvl,__func__,__LINE__ __VA_OPT__(,) __VA_ARGS__); \
    } else { \
    }} while(0)

#define ZERROR(tag,fmt,...) do { \
        vbl_printf_stdout("[ERR]::" tag " @%s:%i | " fmt "\n",__func__,__LINE__ __VA_OPT__(,) __VA_ARGS__); \
    } while(0)

#else

#define ZDEBUG(tag,lvl,maxlvl,fmt,...) do {} while(0)
#define ZERROR(tag,fmt,...) do {} while(0)

#endif

#else

#define ZDEBUG(tag,lvl,maxlvl,fmt,...) do {} while(0)
#define ZERROR(tag,fmt,...) do {} while(0)

#endif

#define LVL0 0
#define LVL1 1
#define LVL2 2
#define LVL3 3


