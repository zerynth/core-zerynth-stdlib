#ifndef __PTRACE__
#define __PTRACE__


#if  !defined(VM_TRACING_MASK)
#define VM_TRACING_MASK 0
#endif

#if VM_TRACING
#include "SEGGER_RTT.h"

#define ztracef(...) SEGGER_RTT_printf(0,__VA_ARGS__);
#define ztraceb(obj,sz) SEGGER_RTT_Write(0,(void*)(obj),sz);

extern uint32_t vm_trace_mask;
void vm_set_tracing(uint32_t mask);

typedef struct _pytraceobj {
    uint8_t tag;
    uint8_t size;
    uint16_t th;
    uint32_t addr;
} PyTraceObj;

#define VM_IS_TRACING(f) ((vm_trace_mask)&(f))

#define ztrace(what, ...) do { \
    if (VM_IS_TRACING(what)) {  \
        ztracef(__VA_ARGS__);      \
    }                            \
} while(0)

#define ztraceo(what, thetag, thesize, obj) do { \
    if (VM_IS_TRACING(what)) {  \
        PyTraceObj __ptf;\
        __ptf.tag=(thetag);\
        __ptf.th = vosThGetId(vosThCurrent());\
        __ptf.addr=(uint32_t)(obj);\
        __ptf.size=(thesize); \
        SEGGER_RTT_LOCK(); \
        SEGGER_RTT_WriteNoLock(0,(void*)&__ptf,sizeof(PyTraceObj)); \
        if(__ptf.size) SEGGER_RTT_WriteNoLock(0,(void*)(obj),__ptf.size); \
        SEGGER_RTT_UNLOCK(); \
    }                            \
} while(0)


#define ztracemv2(what, msg, val32, val16) do { \
    if (VM_IS_TRACING(what)) {  \
        PyTraceObj __ptf;\
        __ptf.tag=(TRACING_MESSAGE);\
        __ptf.th = (uint16_t)(val16);\
        __ptf.addr=(uint32_t)(val32);\
        __ptf.size=sizeof(msg); \
        SEGGER_RTT_LOCK(); \
        SEGGER_RTT_WriteNoLock(0,(void*)&__ptf,sizeof(PyTraceObj)); \
        if(__ptf.size) SEGGER_RTT_WriteNoLock(0,(void*)(msg),__ptf.size); \
        SEGGER_RTT_UNLOCK(); \
    }                            \
} while(0)

#define ztracemv(what, msg, val32) ztracemv2(what,msg,val32,0)
#define ztracem(what, msg) ztracemv2(what,msg,0,0)

#define vm_tracing_init(...)  do {\
    SEGGER_RTT_Init(); \
    vm_set_tracing(VM_TRACING_MASK); \
} while (0);

#else

#define ztracef(...)
#define ztrace(...)
#define ztraceb(...)
#define ztraceo(...)
#define ztracem(...)
#define ztracemv(...)
#define ztracemv2(...)
#define vm_tracing_init(...)
#define VM_IS_TRACING(f) (0)

#endif //VMTRACING


//Tracing flags
//32 bits = 32 flags

#define VM_NO_TRACING   0

//GC Tracing
//trace allocations
#define GC_TRACING_ALLOCS       (1<<0)
//trace collections
#define GC_TRACING_COLLECT      (1<<1)
//trace heap at the end of collect
#define GC_TRACING_HEAP         (1<<2)
//trace heap at all phases of collect
#define GC_TRACING_FULL         (1<<3)
//trace gc_malloc/gc_free
#define GC_TRACING_MALLOCS         (1<<4)
//trace gc stats
#define GC_TRACING_STATS         (1<<5)

//PY Tracing
//trace threads creation, switch and death
#define PY_TRACING_THREADS         (1<<6)
//trace each opcode
#define PY_TRACING_BYTECODE        (1<<7)
//trace each opcode with frame stack & locals
#define PY_TRACING_INNER          (1<<8)
//trace GIL
#define PY_TRACING_LOCKS           (1<<9)
//trace EXCEPTIONS
#define PY_TRACING_EXCEPTIONS      (1<<10)
#define PY_TRACING_FRAMES          (1<<11)


//VM Tracing
//traces uplink process
#define VM_TRACING_STARTUP         (1<<12)
//traces ota process
#define VM_TRACING_OTA             (1<<13)
//traces powersave
#define VM_TRACING_POWERSAVE       (1<<14)
//traces SFW
#define VM_TRACING_SFW             (1<<15)
//traces reset/restarts
#define VM_TRACING_RESET           (1<<16)


//TRACING TAGS
#define TRACING_MESSAGE 'm'
#define TRACING_GC_OBJECT 'O'
#define TRACING_GC_KEEP 'K'
#define TRACING_GC_ALLOC 'A'
#define TRACING_GC_MALLOC 'M'
#define TRACING_GC_FREE 'F'
#define TRACING_PY_THREAD_START 'T'
#define TRACING_PY_THREAD_YIELD 'y'
#define TRACING_PY_THREAD_RESUME 'Y'
#define TRACING_PY_THREAD_END 't'
#define TRACING_PY_OPCODE 'o'
#define TRACING_PY_FRAME 'f'
#define TRACING_PY_STACK_OBJ 'S'
#define TRACING_PY_STACK_OBJ_TAGGED 's'
#define TRACING_PY_LOCAL_OBJ 'L'
#define TRACING_PY_LOCAL_OBJ_TAGGED 'l'
#define TRACING_PY_BLOCK 'B'

#endif
