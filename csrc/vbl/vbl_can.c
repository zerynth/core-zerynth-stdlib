#include "vosal.h"
#include "vhal.h"
#include "vbl.h"
#include "lang.h"


//Enable/Disable debug printf
#define CANBUS_DEBUG 0
#define DEBUG_CHANNEL 1

#if CANBUS_DEBUG
static void debug_printf(const char* fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    vbl_printf((void*)((uint32_t)&vhalSerialWrite | 1), DEBUG_CHANNEL, (uint8_t*)fmt, &vl);
    va_end(vl);
}
#define printf(...) debug_printf(__VA_ARGS__)
#define print_buffer(buf, len) vhalSerialWrite(DEBUG_CHANNEL, buf, len)
#else
#define printf(...)
#define print_buffer(buf, len)
#endif // CANBUS_DEBUG


#define _CANDRIVER_INIT         0
#define _CANDRIVER_DONE         1
#define _CANDRIVER_ERRORS       2
#define _CANDRIVER_TIMING       3
#define _CANDRIVER_RX           4
#define _CANDRIVER_TX           5
#define _CANDRIVER_ADDFILTER    6
#define _CANDRIVER_DELFILTER    7
#define _CANDRIVER_STOPRX       8
#define _CANDRIVER_STOPTX       9
#define _CANDRIVER_RESUMERX     10
#define _CANDRIVER_RESUMETX     11

err_t _can_ctl(int nargs, PObject *self, PObject **args, PObject **res) {
    (void)self;
    int32_t code;
    int32_t drvid;
    *res = MAKE_NONE();
    printf("VBL_CAN nargs=%i args=%x map=%x map[0]=%d\n", nargs, args, _vhal_can_map, _vhal_can_map[0]);
    //printf("VBL_CAN nargs=%i args=%x\n", nargs, args);
    if (nargs < 2)
        goto ret_err_type;
    PARSE_PY_ARGCODE(code);
    PARSE_PY_INT(drvid); //number of vm can prph: CAN0, CAN1, etcc...


    drvid=drvid&0xff;
    printf("VBL_CAN %i %i %i %x\n",drvid,code,nargs, args);

    // drvid = 0;//TEST-ONLY

    switch (code) {
        case _CANDRIVER_INIT: {
            vhalCanConf conf;
            int32_t baud,prop,phase1,phase2,sjw,options;
            if (parse_py_args("iiiiii", nargs, args, &baud, &options, &prop, &phase1, &phase2, &sjw) != 6)
                goto ret_err_type;
            conf.speed = baud;
            conf.prop_seg = prop;
            conf.phase1_seg = phase1;
            conf.phase2_seg = phase2;
            conf.sjw = sjw;
            conf.options = options;
            printf("VBL_CAN_INIT asked: %i %i %x %i %i %i %i\n",drvid,baud,options,prop,phase1,phase2,sjw);
            CanPins *canpins = ((CanPins*)_vm_pin_map(PRPH_CAN));
            conf.rx = canpins[drvid].rxpin;
            conf.tx = canpins[drvid].txpin;
            printf("VBL_CAN_INIT pins: rx %x tx %x\n",conf.rx,conf.tx);
            RELEASE_GIL();
            code = vhalCanInit(drvid, &conf);
            ACQUIRE_GIL();
            printf("VBL_CAN_INIT: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_DONE: {
            if (nargs != 0)
                goto ret_err_type;
            RELEASE_GIL();
            code = vhalCanDone(drvid);
            ACQUIRE_GIL();
            printf("VBL_CAN_DONE: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_ERRORS: {
            if (nargs != 0)
                goto ret_err_type;
            vhalCanErrors err;
            RELEASE_GIL();
            code = vhalCanGetErrors(drvid, &err);
            ACQUIRE_GIL();
            printf("VBL_CAN_ERRORS: %i\n",code);
            if (code < 0)
                return -code;
            // format result
            *res = ptuple_new(3, NULL);
            PTUPLE_SET_ITEM(*res, 0, pinteger_new_u(err.flags));
            PTUPLE_SET_ITEM(*res, 1, PSMALLINT_NEW(err.rx_count));
            PTUPLE_SET_ITEM(*res, 2, PSMALLINT_NEW(err.tx_count));
        }
        break;
        case _CANDRIVER_TIMING: {
            vhalCanConf conf;
            int32_t baud,sample_point;
            if (parse_py_args("ii", nargs, args, &baud, &sample_point) != 2)
                goto ret_err_type;
            printf("VBL_CAN_TIMING asked: %i %i %i\n",drvid,baud,sample_point);
            RELEASE_GIL();
            code = vhalCanFindTiming(drvid, &conf, baud, sample_point, 0);
            ACQUIRE_GIL();
            printf("VBL_CAN_TIMING: %i\n",code);
            if (code < 0)
                return -code;
            // format result
            *res = ptuple_new(3, NULL);
            PTUPLE_SET_ITEM(*res, 0, PSMALLINT_NEW(conf.prop_seg));
            PTUPLE_SET_ITEM(*res, 1, PSMALLINT_NEW(conf.phase1_seg));
            PTUPLE_SET_ITEM(*res, 2, PSMALLINT_NEW(conf.phase2_seg));
        }
        break;
        case _CANDRIVER_RX: {
            vhalCanFrame frame;
            int32_t data_len=0;
            uint8_t* data;
            uint32_t timeout = VTIME_INFINITE;
            if (parse_py_args("bi", nargs, args, &data, &data_len, &timeout) != 2)
                goto ret_err_type;
            printf("VBL_CAN_RX asked: %i (%d %d)\n",drvid, data_len, timeout);
            frame.dlc = (data_len > 64) ? 64 : data_len;
            RELEASE_GIL();
            code = vhalCanRx(drvid, &frame, data, timeout);
            ACQUIRE_GIL();
            printf("VBL_CAN_RX: %i\n",code);
            if (code < 0)
                return -code;
            // format result
            data_len = (frame.id & CAN_RTR_FLAG) ? 0 : frame.dlc;
            printf("VBL_CAN_RX len: %i\n",data_len);
            PSEQUENCE_ELEMENTS_SET(args[0], data_len);
            *res = ptuple_new(3, NULL);
            PTUPLE_SET_ITEM(*res, 0, pinteger_new_u(frame.id));
            PTUPLE_SET_ITEM(*res, 1, PSMALLINT_NEW(frame.dlc));
            PTUPLE_SET_ITEM(*res, 2, args[0]);
        }
        break;
        case _CANDRIVER_TX: {
            vhalCanFrame frame;
            int32_t id,dlc,data_len=0;
            uint8_t* data = NULL;
            uint32_t timeout = VTIME_INFINITE;
            if (nargs != 4)
                goto ret_err_type;
            if (parse_py_args("ii", 2, args, &id, &dlc) != 2)
                goto ret_err_type;
            if (PTYPE(args[2]) == PNONE) {
                // no data allowed only for Remote frames
                if (!(id & CAN_RTR_FLAG))
                    goto ret_err_value;
                args += 3;
                if (parse_py_args("i", 1, args, &timeout) != 1)
                    goto ret_err_type;
            }
            else {
                args += 2;
                if (parse_py_args("si", 2, args, &data, &data_len, &timeout) != 2)
                    goto ret_err_type;
            }
            printf("VBL_CAN_TX asked: %i (%x %d %d %d)\n",drvid,id,dlc,data_len,timeout);
            if (!(id & CAN_RTR_FLAG) && dlc > data_len)
                goto ret_err_value;
            frame.id = id;
            frame.dlc = dlc;
            RELEASE_GIL();
            code = vhalCanTx(drvid, &frame, data, timeout);
            ACQUIRE_GIL();
            printf("VBL_CAN_TX: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_ADDFILTER: {
            vhalCanFilter filter;
            int32_t id,mask;
            if (parse_py_args("ii", nargs, args, &id, &mask) != 2)
                goto ret_err_type;
            filter.id = id;
            filter.mask = mask;
            printf("VBL_CAN_ADD asked: %i (%x %x)\n",drvid,filter.id,filter.mask);
            RELEASE_GIL();
            code = vhalCanAddFilter(drvid, &filter);
            ACQUIRE_GIL();
            printf("VBL_CAN_ADD: %i\n",code);
            if (code < 0)
                return -code;
            // format result (filter index)
            *res = PSMALLINT_NEW(code);
        }
        break;
        case _CANDRIVER_DELFILTER: {
            int32_t filter;
            if (parse_py_args("i", nargs, args, &filter) != 1)
                goto ret_err_type;
            printf("VBL_CAN_DEL asked: %i (%d)\n",drvid,filter);
            RELEASE_GIL();
            code = vhalCanRemoveFilter(drvid, filter);
            ACQUIRE_GIL();
            printf("VBL_CAN_DEL: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_STOPRX: {
            if (nargs != 0)
                goto ret_err_type;
            RELEASE_GIL();
            code = vhalCanAbortRx(drvid);
            ACQUIRE_GIL();
            printf("VBL_CAN_STOPRX: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_STOPTX: {
            if (nargs != 0)
                goto ret_err_type;
            RELEASE_GIL();
            code = vhalCanAbortTx(drvid);
            ACQUIRE_GIL();
            printf("VBL_CAN_STOPTX: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_RESUMERX: {
            if (nargs != 0)
                goto ret_err_type;
            RELEASE_GIL();
            code = vhalCanResumeRx(drvid);
            ACQUIRE_GIL();
            printf("VBL_CAN_RESUMERX: %i\n",code);
            if (code < 0)
                return -code;
        }
        break;
        case _CANDRIVER_RESUMETX: {
            if (nargs != 0)
                goto ret_err_type;
            RELEASE_GIL();
            code = vhalCanResumeTx(drvid);
            ACQUIRE_GIL();
            printf("VBL_CAN_RESUMETX: %i\n",code);
            if (code < 0)
                return -code;
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

err_t _vbl_can_init(int nargs, PObject *self, PObject **args, PObject **res) {
    *res = MAKE_NONE();
    VBLDriver candriver;
    candriver.id=PRPH_CAN;
    candriver.ctl=_can_ctl;
    void* ret = vbl_install_driver(&candriver);
    printf("_vbl_can_init %x (%i %x)\n",ret,candriver.id,candriver.ctl);
    vhalInitCAN(NULL);
    return ERR_OK;
}
