#include "zerynth.h"


C_NATIVE(_mcu_reset){
  NATIVE_UNWARN();
  vosSysReset();
  return ERR_OK;
}