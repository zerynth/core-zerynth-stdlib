#include "zerynth.h"

C_NATIVE(__builtin_id){
  NATIVE_UNWARN();
  *res = PSMALLINT_NEW((IS_TAGGED(args[0]) ? &args[0] : args[0]));
  return ERR_OK;
}

C_NATIVE(_mcu_reset){
  NATIVE_UNWARN();
  vosSysReset();
  return ERR_OK;
}