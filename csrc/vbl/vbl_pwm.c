#include "vosal.h"
#include "vhal.h"
#include "vbl.h"
#include "lang.h"


#define printf(...) vbl_printf_stdout(__VA_ARGS__)

err_t _pwm_ctl(int nargs, PObject *self, PObject **args, PObject **res) {
    (void)self;
    int32_t code;
    *res = MAKE_NONE();
    PARSE_PY_ARGCODE(code);


    switch (code) {
        case DRV_CMD_WRITE: {
            uint32_t pin = 0;
            uint32_t period = 0;
            uint32_t pulse = 0;
            uint32_t time_unit = 0;
            int32_t npulses = 0;
            if (parse_py_args("iiiii", nargs, args, &pin, &period, &pulse, &time_unit, &npulses) != 5) goto ret_err_type;
            period = TIME_U(period, time_unit);
            pulse = TIME_U(pulse, time_unit);
            if(npulses>0) RELEASE_GIL();
            code = vhalPwmStart(pin, period, pulse,npulses);
            if(npulses>0) ACQUIRE_GIL();
            if (code < 0)
                return -code;
        }
        break;
        default:
            goto ret_unsup;
    }


    return ERR_OK;
ret_err_type:
    return ERR_TYPE_EXC;
ret_unsup:
    return ERR_UNSUPPORTED_EXC;

}


const VBLDriver pwmdriver = {
    PRPH_PWM,
    _pwm_ctl
};



err_t _vbl_pwm_init(int nargs, PObject *self, PObject **args, PObject **res) {
    *res = MAKE_NONE();
    vbl_install_driver(&pwmdriver);
    vhalInitPWM(NULL);
    return ERR_OK;
}


