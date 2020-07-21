#if !defined(ZERYNTH_HWCRYPTO_DISABLED)
#include "zerynth_hwcrypto.h"

#if !defined(ZERYNTH_HWCRYPTO_EXTERNAL)
ZHWCrypto zhwcrypto={0};
#endif

void gzcrypto_hw_init(ZHWCryptoAPIPointers *pointers, ZHWCryptoInfo *nfo) {
    memset(&zhwcrypto,0,sizeof(ZHWCrypto));
    zhwcrypto.capi = pointers;
    zhwcrypto.cnfo = nfo;
}

void gzcrypto_hw_disable(){
    zhwcrypto.api = NULL;
    zhwcrypto.nfo = NULL;
};

void gzcrypto_hw_enable(){
    zhwcrypto.api = zhwcrypto.capi;
    zhwcrypto.nfo = zhwcrypto.cnfo;
}
#else

#endif


