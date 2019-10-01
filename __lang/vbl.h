#ifndef __VBL__
#define __VBL__

/* Zerynth Base Layer */

#include "vosal.h"
#include "vhal.h"

extern uint8_t *__flash_end__;


#define MAX_INT	INT32_MAX
#define MIN_INT INT32_MIN

#define MIN(a,b) ((a)<(b) ? (a):(b))
#define MAX(a,b) ((a)>(b) ? (a):(b))

#define RETVAL_OK 0



/* Endianess */
extern uint8_t __codemem__[];

#define Z_BIG_ENDIAN  0
#define Z_LITTLE_ENDIAN  1


#define BLTSWAP16(a) __builtin_bswap16(a)
#define BLTSWAP32(a) __builtin_bswap32(a)
#define BLTSWAP64(a) __builtin_bswap64(a)


#if (__BYTE_ORDER__) == (__ORDER_LITTLE_ENDIAN__)

#define PY_ENDIANNESS   Z_LITTLE_ENDIAN
#define OAL_MAKE_IP(ip,a,b,c,d) ip = (uint32_t)(((uint32_t)(a))|(((uint32_t)(b))<<8)|((uint32_t)(c)<<16)|((uint32_t)(d)<<24))
#define OAL_IP_AT(ip,n) (((ip)&(0x000000ff<<(8*(n))))>>(8*(n)))
#define OAL_IP_SET(ip,n,x) ip= ((x)<<(8*(n)))| ( (ip)&~(0xff<<(8*(n))))
#define OAL_SET_NETPORT(port,x) port = BLTSWAP16((uint16_t)x)
#define OAL_GET_NETPORT(port) BLTSWAP16((uint16_t)port)


#else

#define PY_ENDIANNESS   Z_BIG_ENDIAN
#define OAL_MAKE_IP(ip,a,b,c,d) ip = (uint32_t)(((uint32_t)(d))|(((uint32_t)(c))<<8)|((uint32_t)(b)<<16)|((uint32_t)(a)<<24))
#define OAL_IP_AT(ip,n) (((ip)&(0x000000ff<<(24-8*(n))))>>(24-8*(n)))
#define OAL_IP_SET(ip,n,x) ip= ((x)<<(24-8*(n)))| ( (ip)&~(0xff<<(24-8*(n))))
#define OAL_SET_NETPORT(port,x) port = (uint16_t)(x)
#define OAL_GET_NETPORT(port) (port)


#endif



/* Thread Macros */


#define SYSLOCK() vosSysLock()
#define SYSUNLOCK() vosSysUnlock()
#define SYSLOCK_I() vosSysLockIsr()
#define SYSUNLOCK_I() vosSysUnlockIsr()

#define THD_CURRENT() vosThCurrent()



/* types */
typedef int32_t err_t;


typedef struct _netaddress {
    uint32_t ip;
    uint16_t port;
} NetAddress;


/*
    ======================================
    DRIVERS
    ======================================
*/


/*types*/

#define DRIVER_SERIAL       1
#define DRIVER_ADC          2
#define DRIVER_SPI          3
#define DRIVER_I2C          4
#define DRIVER_NET          5
#define DRIVER_PWM          6
#define DRIVER_OTHER        7

/*subtypes*/

#define DRIVER_WIFI        50
#define DRIVER_ETH         51
#define DRIVER_SERIAL_USB  10
#define DRIVER_SERIAL_UART  11



/*driver commands */

#define DRV_CMD_INIT            0
#define DRV_CMD_DONE            1
#define DRV_CMD_WRITE           2
#define DRV_CMD_READ            3
#define DRV_CMD_AVAILABLE       4
#define DRV_CMD_OPEN            7
#define DRV_CMD_CLOSE           8
#define DRV_CMD_STARTUP         9
//sockets
#define DRV_CMD_SOCKET          10
#define DRV_CMD_BIND            11
#define DRV_CMD_LISTEN          12
#define DRV_CMD_ACCEPT          13
#define DRV_CMD_RESOLVE         14
#define DRV_CMD_INFO            15
#define DRV_CMD_SETOPTS         16
#define DRV_CMD_GETOPTS         17
#define DRV_CMD_START_PING      18
#define DRV_CMD_CHECK_PING      19
#define DRV_CMD_STOP_PING       20
#define DRV_CMD_WRITE_TO        21
#define DRV_CMD_READ_FROM       22
//wifi
#define DRV_CMD_SCAN            23
#define DRV_CMD_GET_LOWLEVEL    24


struct _driver {
    uint32_t id;
    void *ctl;
};

typedef struct _driver VBLDriver;

extern VBLDriver _drivers[];


int get_driver(uint32_t id);
void init_drivers(void);


void vbl_printf(void *fn, uint32_t drvid, uint8_t *fmt, va_list *vlp);

void *vbl_get_adjusted_codemem(void* codemem);

void *vbl_install_driver(const VBLDriver* drv);


typedef struct _vm_ser_pin {
    uint16_t rxpin;
    uint16_t txpin;
    uint16_t ctspin;
    uint16_t rtspin;
} SerialPins;


typedef struct _vm_spi_pin {
    uint16_t mosi;
    uint16_t miso;
    uint16_t sclk;
    uint8_t nss;
    uint8_t spiid;
} SpiPins;

typedef struct _vm_i2c_pin {
    uint16_t sda;
    uint16_t scl;
} I2CPins;

typedef struct _vm_sdio_pins {
    uint16_t sdio_pins[10];
} SdioPins;

#if defined(VM_IS_CUSTOM)
extern SerialPins *_vm_serial_pins;
extern SpiPins *_vm_spi_pins;
extern I2CPins *_vm_i2c_pins;
extern SdioPins *_vm_sdio_pins;

#define _vm_pin_map(prph) ( ((prph)==0x02) ? (((void*)_vm_spi_pins)):( ((prph)==0x03) ? (((void*)_vm_i2c_pins)):( NULL ))  )

#else

extern const SerialPins const _vm_serial_pins[];
extern const SpiPins const _vm_spi_pins[];
extern const I2CPins const _vm_i2c_pins[];
extern const SdioPins const _vm_sdio_pins[];

#define _vm_pin_map(prph) ( ((prph)==0x02) ? ((void*)_vm_spi_pins):( ((prph)==0x03) ? ((void*)_vm_i2c_pins):( NULL ))  )

#endif


#define VM_RXPIN(ch) _vm_serial_pins[ch].rxpin
#define VM_TXPIN(ch) _vm_serial_pins[ch].txpin
#define VM_RTSPIN(ch)  _vm_serial_pins[ch].rtspin
#define VM_CTSPIN(ch)  _vm_serial_pins[ch].ctspin
#define VM_HWFC(ch) (((VM_RTSPIN(ch)!=0xff)&&(VM_CTSPIN(ch)!=0xff)) ? SERIAL_HWFC:SERIAL_NO_HWFC)
#define VM_HWFCPINS(ch) (SERIAL_CTS(VM_CTSPIN((ch)))|SERIAL_RTS(VM_RTSPIN(ch)))

/*====== BYTECODE ================================================== */


uint8_t *vbl_init(void *);
void *begin_bytecode_storage(int);
void *bytecode_store(void *, uint8_t *, uint16_t);
void *end_bytecode_storage(void);
uint32_t adler32(uint8_t *buf, uint32_t len);


/*====== DEBUG ================================================== */


void vbl_printf_stdout(uint8_t *fmt,...);
void vbl_printf_noos(uint8_t *fmt,...);
void zerynth_hardfault_handler(void*);
void zerynth_outofmemory_handler(void*);
#define printf(...) vbl_printf_stdout(__VA_ARGS__)

#define _DEBUG 0
#define _INFO 1
#define _WARNING 2
#define _ERROR 3

#ifdef VM_DEBUG
extern void viper_debug(int lvl, const char *fmt, ...);

#if VM_DEBUG_LEVEL==0
#define debug(...) viper_debug(_DEBUG,__VA_ARGS__)
#define info(...) viper_debug(_INFO,__VA_ARGS__)
#define warning(...) viper_debug(_WARNING,__VA_ARGS__)
#define error(...) viper_debug(_ERROR,__VA_ARGS__)
#endif

#if VM_DEBUG_LEVEL==1
#define debug(...) 
#define info(...) viper_debug(_INFO,__VA_ARGS__)
#define warning(...) viper_debug(_WARNING,__VA_ARGS__)
#define error(...) viper_debug(_ERROR,__VA_ARGS__)
#endif


#if VM_DEBUG_LEVEL==2
#define debug(...) 
#define info(...) 
#define warning(...) viper_debug(_WARNING,__VA_ARGS__)
#define error(...) viper_debug(_ERROR,__VA_ARGS__)
#endif

#if VM_DEBUG_LEVEL==3
#define debug(...) 
#define info(...) 
#define warning(...)
#define error(...) viper_debug(_ERROR,__VA_ARGS__)
#endif


#else
#define debug(...)
#define info(...)
#define warning(...)
#define error(...)
#endif



#ifdef BYTECODE_ACCESS_ALIGNED_4
void *memcpy4(void *dst, void *src, int size);
void* memset4(void *dst, int c, int size);
int memcmp4(void *dst, void *src, int size);


uint32_t __access_byte(uint32_t *c, int32_t pos);
uint32_t __access_word(uint32_t *c, int32_t pos);
uint32_t __access_dword(uint32_t *c, int32_t pos);
uint64_t __access_qword(uint32_t *c, int32_t pos);
#endif

#endif
