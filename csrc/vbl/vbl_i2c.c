#include "vosal.h"
#include "vhal.h"
#include "vbl.h"
#include "lang.h"


//#define printf(...) vbl_printf_stdout(__VA_ARGS__)
#define printf(...)


#define _I2CDRIVER_START    0
#define _I2CDRIVER_STOP     1
#define _I2CDRIVER_READ     2
#define _I2CDRIVER_WRITE    3
#define _I2CDRIVER_WRITE_READ 4
#define _I2CDRIVER_LOCK     5
#define _I2CDRIVER_UNLOCK   6
#define _I2CDRIVER_SET_ADDR   7

err_t _i2c_ctl(int nargs, PObject *self, PObject **args, PObject **res) {
    (void)self;
    int32_t code;
    int32_t drvid;
    *res = MAKE_NONE();
    PARSE_PY_ARGCODE(code);
    PARSE_PY_INT(drvid); //number of vm spi prph: SPI0, SPI1, etcc...

    switch (code) {
        case _I2CDRIVER_START: {
            vhalI2CConf conf;
            uint32_t addr;
            I2CPins *i2cpins = ((I2CPins*)_vm_pin_map(PRPH_I2C));
            if (parse_py_args("ii", nargs, args, &addr, &conf.clock) != 2) goto ret_err_type;
            conf.addr = (uint16_t)addr;
            conf.sda = i2cpins[drvid].sda;
            conf.scl = i2cpins[drvid].scl;
            code = vhalI2CInit(drvid, &conf);
            if (code < 0)
                return -code;
        }
        break;
        case _I2CDRIVER_STOP: {
            code = vhalI2CDone(drvid);
            if (code < 0)
                return -code;
        }
        break;
        case _I2CDRIVER_LOCK: {
            RELEASE_GIL();
            code = vhalI2CLock(drvid);
            ACQUIRE_GIL();
            if (code < 0)
                return -code;
        }
        break;
        case _I2CDRIVER_UNLOCK: {
            code = vhalI2CUnlock(drvid);
            if (code < 0)
                return -code;
        }
        break;
        case _I2CDRIVER_WRITE_READ: {
            uint8_t *tx=NULL;
            uint32_t rxlen=0;
            uint32_t txlen=0;
            int32_t timeout;
            uint32_t addr;
            if (parse_py_args("siii",nargs,args,&tx,&txlen,&rxlen,&timeout,&addr)!=4) goto ret_err_type;
            PBytes *bb = psequence_new(PBYTES,rxlen);
            RELEASE_GIL();
            vhalI2CSetAddr(drvid,(uint16_t)addr);
            code = vhalI2CTransmit(drvid,tx,txlen,PSEQUENCE_BYTES(bb),rxlen,(timeout<0) ? (VTIME_INFINITE):TIME_U(timeout,MILLIS));
            ACQUIRE_GIL();
            if(code<0){
                GC_UNSTAGE(bb);
                return -code;
            }
            *res=(PObject*)bb;
        }
        break;
        case _I2CDRIVER_WRITE: {
            uint8_t *tx=NULL;
            uint32_t txlen=0;
            int32_t timeout;
            uint32_t addr;
            if (parse_py_args("sii",nargs,args,&tx,&txlen,&timeout,&addr)!=3) goto ret_err_type;
            RELEASE_GIL();
            vhalI2CSetAddr(drvid,(uint16_t)addr);
            code = vhalI2CTransmit(drvid,tx,txlen,NULL,0,(timeout<0) ? (VTIME_INFINITE):TIME_U(timeout,MILLIS));
            ACQUIRE_GIL();
            if(code<0){
                return -code;
            }
        }
        break;
        case _I2CDRIVER_READ: {
            uint32_t rxlen=0;
            int32_t timeout;
            uint32_t addr;
            if (parse_py_args("iii",nargs,args,&rxlen,&timeout,&addr)!=3) goto ret_err_type;
            PBytes *bb = psequence_new(PBYTES,rxlen);
            RELEASE_GIL();
            vhalI2CSetAddr(drvid,(uint16_t)addr);
            code = vhalI2CRead(drvid,PSEQUENCE_BYTES(bb),rxlen,(timeout<0) ? (VTIME_INFINITE):TIME_U(timeout,MILLIS));
            ACQUIRE_GIL();
            if(code<0){
                GC_UNSTAGE(bb);
                return -code;
            }
            *res=(PObject*)bb;
        }
        break;
        case _I2CDRIVER_SET_ADDR: {
            if (parse_py_args("i",nargs,args,&code)!=1) goto ret_err_type;
            code = vhalI2CSetAddr(drvid,code);
            if (code<0)
                return -code;
        }
        break;
    }
    return ERR_OK;
ret_err_type:
    return ERR_TYPE_EXC;
ret_unsup:
    return ERR_UNSUPPORTED_EXC;

}




err_t _vbl_i2c_init(int nargs, PObject *self, PObject **args, PObject **res) {
    *res = MAKE_NONE();
    VBLDriver i2cdriver;
    i2cdriver.id=PRPH_I2C;
    i2cdriver.ctl=_i2c_ctl;
    vbl_install_driver(&i2cdriver);
    vhalInitI2C(NULL);
    return ERR_OK;
}


