/*
* @Author: Lorenzo
* @Date:   2018-05-30 09:45:20
* @Last Modified by:   Lorenzo
* @Last Modified time: 2018-06-04 14:17:21
*/

#include "zerynth_hwcrypto.h"

ZHWCryptoAPIPointers null_api_pointers = {NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL};

ZHWCryptoAPIPointers *zhwcrypto_api_pointers = &null_api_pointers;
ZHWCryptoInfo *zhwcrypto_info = NULL;
