#include "zerynth.h"


//#define printf(...) vbl_printf_stdout(__VA_ARGS__)
//#define printf(...)

#if !defined(VBOARD_SLEEP_MICRO_COMPENSATION)
#define VBOARD_SLEEP_MICRO_COMPENSATION 0
#endif

int32_t hwtimer = -1;


C_NATIVE(hwtimers_init) {
    C_NATIVE_UNWARN();
    vhalInitHTM(NULL);
    hwtimer = vhalHtmGetFreeTimer();
    if (hwtimer < 0)
        return ERR_UNSUPPORTED_EXC;
    *res = MAKE_NONE();
    return ERR_OK;
}


C_NATIVE(hwtimers_sleep_micros) {
    C_NATIVE_UNWARN();
    if (!IS_PSMALLINT(args[0]))
        return ERR_TYPE_EXC;
    int32_t micros = PSMALLINT_VALUE(args[0]);
    *res = MAKE_NONE();
    int ret=ERR_OK;
    //printf("wait %i\n",micros);
    //compensate: 
    //micros=micros-(80-((_system_frequency/1000000-84)*80)/84);
    micros-=VBOARD_SLEEP_MICRO_COMPENSATION;
    if (micros > 0) {
        RELEASE_GIL();
        ret = vhalSleepMicros(hwtimer, micros);
        ACQUIRE_GIL();
    }
    return ERR_OK;
}