#include "vosal.h"
#include "vhal.h"
#include "vbl.h"
#include "lang.h"


#define printf(...) vbl_printf_stdout(__VA_ARGS__)

err_t _rtc_ctl(int nargs, PObject *self, PObject **args, PObject **res) {
    (void)self;
    int32_t code;
    int32_t drvid;
    *res = MAKE_NONE();
    PARSE_PY_ARGCODE(code);
    PARSE_PY_INT(drvid); //number of vm rtc prph: RTC0, RTC1, etc...

    switch (code) {
        case DRV_CMD_INIT: {

            RELEASE_GIL();
            code = vhalRTCInit(drvid);
            ACQUIRE_GIL();

            if (code < 0) {
                goto ret_err_value;
            }
        }
        break;

        case DRV_CMD_WRITE: {

            if (nargs != 2) goto ret_err_type;

            if ((PTYPE(args[0]) != PSMALLINT && PTYPE(args[0]) != PINTEGER) ||
                (PTYPE(args[1]) != PSMALLINT && PTYPE(args[1]) != PINTEGER)) {
                goto ret_err_type;
            }

            uint32_t seconds = INTEGER_VALUE(args[0]);
            uint32_t useconds = INTEGER_VALUE(args[1]);

            RELEASE_GIL();
            code = vhalRTCSetUTC(drvid, seconds, useconds);
            ACQUIRE_GIL();

            if (code < 0) {
                goto ret_err_value;
            }
        }
        break;
        case DRV_CMD_READ: {
            vhalRTCTimeInfo time_info;

            uint8_t verbosity;
            if (parse_py_args("i", nargs, args, &verbosity) != 1) goto ret_err_type;

            RELEASE_GIL();
            code = vhalRTCGetUTC(drvid, &time_info);
            ACQUIRE_GIL();

            if (code < 0) {
                goto ret_err_value;
            }

            if (verbosity == 0) {
                *res = pinteger_new(time_info.tv_seconds);
            }
            else if (verbosity == 1) {
                PTuple *pt = (PTuple*)psequence_new(PTUPLE,2);

                PTUPLE_SET_ITEM(pt, 0, pinteger_new(time_info.tv_seconds));
                PTUPLE_SET_ITEM(pt, 1, PSMALLINT_NEW(time_info.tv_microseconds));

                *res = (PObject *)pt;
            }
            else {
                //timestamp in seconds, microseconds, sec, min, hour, mday, mon, year, wday, yday, isdst
                PTuple *pt = (PTuple*)psequence_new(PTUPLE,11);

                PTUPLE_SET_ITEM(pt, 0, pinteger_new(time_info.tv_seconds));
                PTUPLE_SET_ITEM(pt, 1, PSMALLINT_NEW(time_info.tv_microseconds));
                PTUPLE_SET_ITEM(pt, 2, PSMALLINT_NEW(time_info.tm_sec));
                PTUPLE_SET_ITEM(pt, 3, PSMALLINT_NEW(time_info.tm_min));
                PTUPLE_SET_ITEM(pt, 4, PSMALLINT_NEW(time_info.tm_hour));
                PTUPLE_SET_ITEM(pt, 5, PSMALLINT_NEW(time_info.tm_mday));
                PTUPLE_SET_ITEM(pt, 6, PSMALLINT_NEW(time_info.tm_mon));
                PTUPLE_SET_ITEM(pt, 7, PSMALLINT_NEW(time_info.tm_year));
                PTUPLE_SET_ITEM(pt, 8, PSMALLINT_NEW(time_info.tm_wday));
                PTUPLE_SET_ITEM(pt, 9, PSMALLINT_NEW(time_info.tm_yday));
                PTUPLE_SET_ITEM(pt,10, PSMALLINT_NEW(time_info.tm_isdst));

                *res = (PObject *)pt;
            }
        }
        break;
        default:
            goto ret_unsup;
    }


    return ERR_OK;
ret_err_value:
    return ERR_VALUE_EXC;
ret_err_type:
    return ERR_TYPE_EXC;
ret_unsup:
    return ERR_UNSUPPORTED_EXC;

}

const VBLDriver rtcdriver = {
    PRPH_RTC,
    _rtc_ctl
};

err_t _vbl_rtc_init(int nargs, PObject *self, PObject **args, PObject **res){
    *res = MAKE_NONE();
    vbl_install_driver(&rtcdriver);
    return ERR_OK;
}