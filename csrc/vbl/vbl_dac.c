#include "vosal.h"
#include "vhal.h"
#include "vbl.h"
#include "lang.h"


//#define printf(...) vbl_printf_stdout(__VA_ARGS__)
#define printf(...)


#define _DACDRIVER_START    0
#define _DACDRIVER_STOP     1
#define _DACDRIVER_WRITE    2
#define _DACDRIVER_LOCK     3
#define _DACDRIVER_UNLOCK   4

err_t _dac_ctl(int nargs, PObject *self, PObject **args, PObject **res) {
    (void)self;
    int32_t code;
    // int32_t drvid;
    *res = MAKE_NONE();
    PARSE_PY_ARGCODE(code);
    // PARSE_PY_INT(drvid); //number of vm spi prph: SPI0, SPI1, etcc...

    switch (code) {
        case _DACDRIVER_START: {
            uint32_t pin;
            if (parse_py_args("i", nargs, args, &pin) != 1) goto ret_err_type;
            code = vhalDacInit(pin);
            if (code < 0)
                return -code;
        }
        break;
        case _DACDRIVER_STOP: {
            uint32_t pin;
            if (parse_py_args("i", nargs, args, &pin) != 1) goto ret_err_type;
            code = vhalDacDone(pin);
            if (code < 0)
                return -code;
        }
        break;
        case _DACDRIVER_LOCK: {
            uint32_t pin;
            if (parse_py_args("i", nargs, args, &pin) != 1) goto ret_err_type;
            RELEASE_GIL();
            code = vhalDacLock(pin);
            ACQUIRE_GIL();
            if (code < 0)
                return -code;
        }
        break;
        case _DACDRIVER_UNLOCK: {
            uint32_t pin;
            if (parse_py_args("i", nargs, args, &pin) != 1) goto ret_err_type;
            code = vhalDacUnlock(pin);
            if (code < 0)
                return -code;
        }
        break;
        case _DACDRIVER_WRITE: {
            uint16_t *data=NULL;
            uint32_t datalen=0;
            uint32_t timestep = 0;
            uint32_t timestep_unit = 0;
            uint32_t circular = 0;
            uint32_t pin = 0;

            if (nargs != 5) goto ret_err_type;

            if (!IS_SHORT_PSEQUENCE_TYPE(PTYPE(args[0]))) goto ret_err_type;
            data = PSEQUENCE_SHORTS(args[0]);
            datalen = PSEQUENCE_ELEMENTS(args[0]);

            if (PTYPE(args[1]) != PSMALLINT) goto ret_err_type;
            timestep = PSMALLINT_VALUE(args[1]);

            if (PTYPE(args[2]) != PSMALLINT) goto ret_err_type;
            timestep_unit = PSMALLINT_VALUE(args[2]);

            if (PTYPE(args[3]) != PBOOL) goto ret_err_type;
            circular = PBOOL_VALUE(args[3]);

            if (PTYPE(args[4]) != PSMALLINT) goto ret_err_type;
            pin = PSMALLINT_VALUE(args[4]);

            RELEASE_GIL();
            code = vhalDacWrite(pin,data,datalen,TIME_U(timestep,timestep_unit),circular);
            ACQUIRE_GIL();
            if(code<0){
                return -code;
            }
        }
        break;
    }
    return ERR_OK;
ret_err_type:
    return ERR_TYPE_EXC;
ret_unsup:
    return ERR_UNSUPPORTED_EXC;

}


const VBLDriver dacdriver = {
    PRPH_DAC,
    _dac_ctl
};



err_t _vbl_dac_init(int nargs, PObject *self, PObject **args, PObject **res) {
    *res = MAKE_NONE();
    vbl_install_driver(&dacdriver);
    vhalInitDAC(NULL);
    return ERR_OK;
}
