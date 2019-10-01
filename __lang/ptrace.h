#ifndef __PTRACE__
#define __PTRACE__


#if  !defined(VM_TRACING_MASK)
#define VM_TRACING_MASK 0
#endif

void ztrace(uint8_t *sFormat, ...);
void ztraceb(uint8_t *buf, uint32_t len);
void ztraceobj(uint32_t what, uint32_t tag, uint32_t size, uint8_t* obj);
void ztracemvv(uint32_t what, uint8_t *msg, uint32_t size, uint32_t val32, uint16_t val16);
void vm_set_tracing(uint32_t mask);
void vm_tracing_init(void);
extern uint32_t vm_trace_mask;
void vm_set_tracing(uint32_t mask);

//if not tracing, avoid calling functions
#define ztracemv2(what, msg, val32, val16) ztracemvv(what,(uint8_t*)(msg),sizeof(msg),(uint32_t)(val32),val16)
#define ztracemv(what, msg, val32) ztracemvv(what,(uint8_t*)(msg),sizeof(msg),(uint32_t)(val32),0)
#define ztracem(what, msg) ztracemvv(what,(uint8_t*)(msg),sizeof(msg),0,0)
#define ztraceo(what,tag,size,obj) ztraceobj(what,tag,size,(uint8_t*)(obj))

#define VM_IS_TRACING(f) ((vm_trace_mask)&(f))

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
#define VM_TRACING_ERRORS           (1<<17)
#define VM_TRACING_DEBUG            (1<<18)


#define VM_TRACING_CUSTOM           (1<<20)

//TRACING TAGS
#define TRACING_MESSAGE 'm'
#define TRACING_ERROR 'E'
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

