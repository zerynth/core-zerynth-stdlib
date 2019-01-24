#ifndef __VM__
#define __VM__

#include "vmthread.h"
#include "port.h"


/* to be dynamically defined */
#define VM_VERSIONL strlen(VM_VERSION)
#define VM_BOARD_LEN strlen(VM_BOARD)
#define VM_ARCH_LEN strlen(VM_ARCH)
#ifndef VM_UPLOAD_CHUNK
#define VM_UPLOAD_CHUNK 4096
#endif
#ifndef VM_SERIAL_BAUD
#define VM_SERIAL_BAUD 38400
#endif
#ifndef VM_DEFAULT_THREAD_SIZE
#define VM_DEFAULT_THREAD_SIZE 512
#endif



extern uint8_t *codemem;
extern uint32_t _vm_thread_quantum;
extern const void * const _vsymbase[];
extern const void * const _abi_fns[];
extern const uint16_t _abi_fn_num;

#define VM_IRQS 8

typedef struct _vm {
    CodeHeader  *program;
    uint16_t nmodules;
    uint16_t irqn;
    PModule **modules;
    PException *etable;
    uint32_t *rtable;
    void **cnatives;
    IrqFn irqstack[VM_IRQS];
    IrqFn irqslots[2][EXT_SLOTS];
    PThread *thlist;
    PThread *irqthread;
    PObject *timers;
    IrqFn *irqcur;
} VM;

#define VM_ADD_MODULE(mod)  (_vm.modules[_vm.nmodules]=(mod),_vm.nmodules++)
#define VM_MODULE(n) ((PModule*)(_vm.modules[n]))
#define VM_MAIN_MODULE() VM_MODULE(0)
#define VM_BUILTINS() VM_MODULE(1)
#define VM_MODULE_GLOBALS(m) (VM_MODULE(m)->globals)


#define VM_ETABLE_ENTRY(x) (_vm.etable+(x))
#define VM_ETABLE_ENTRIES() (_vm.program->nexcp)
#define VM_ETABLE_END() ((uint8_t*)(VM_ETABLE_ENTRY(VM_ETABLE_ENTRIES())))

#ifndef BYTECODE_ACCESS_ALIGNED_4
#define VM_EXCEPTION_MSG_LEN(e)   *((uint16_t*)(VM_ETABLE_END()+VM_ETABLE_ENTRY(PEXCEPTION_ERR(e))->msg))
#define VM_EXCEPTION_MSG(e) ((VM_ETABLE_END()+VM_ETABLE_ENTRY(PEXCEPTION_ERR(e))->msg)+2)
#else
#define VM_EXCEPTION_MSG_LEN(e)   __access_word( (uint32_t*)(VM_ETABLE_END()+  PEXCEPTION_MSG(VM_ETABLE_ENTRY(PEXCEPTION_ERR(e)))) ,0)
#define VM_EXCEPTION_MSG(e) ((VM_ETABLE_END()+PEXCEPTION_MSG(VM_ETABLE_ENTRY(PEXCEPTION_ERR(e))))+2)
#endif

#ifndef BYTECODE_ACCESS_ALIGNED_4
#define VM_EXCEPTION_MSG_LEN_FROM_IDX(e)   *((uint16_t*)(VM_ETABLE_END()+VM_ETABLE_ENTRY(e)->msg))
#define VM_EXCEPTION_MSG_FROM_IDX(e) ((VM_ETABLE_END()+VM_ETABLE_ENTRY(e)->msg)+2)
#else
#define VM_EXCEPTION_MSG_LEN_FROM_IDX(e)   __access_word( (uint32_t*)(VM_ETABLE_END()+  PEXCEPTION_MSG(VM_ETABLE_ENTRY(e))) ,0)
#define VM_EXCEPTION_MSG_FROM_IDX(e) ((VM_ETABLE_END()+PEXCEPTION_MSG(VM_ETABLE_ENTRY(e)))+2)
#endif

#define ACQUIRE_GIL() do {\
        vosSemWait(_gillock); \
    }while(0)

#define RELEASE_GIL() do {\
        vosSemSignalCap(_gillock,1); \
    }while(0)

PThread *vm_init(VM *vm);
int vm_upload(VM *vm);
void vm_load_header(VM *vm);
void vm_add_irq_slot_isr(int slot, int dir);
err_t vm_fill_irq_slot(int slot, int dir, PObject *fn, PObject *args);
void vm_add_irq(PObject *fn, PObject *args);
int vm_run(PThread *thread);
int vm_irqthread(void *prm);
err_t vm_add_timer(PObject *tm, uint32_t msec);
err_t vm_del_timer(PObject *tm);


extern volatile uint8_t gc_checktime;
extern const char *const opstrings[];


extern PObject *P_TRUE;
extern PObject *P_FALSE;
extern PObject *P_NONE;
extern PObject *P_ONE;
extern PObject *P_TWO;
extern PObject *P_ZERO;
extern PObject *P_M_ONE;
extern PObject *P_ONE_F;
extern PObject *P_ZERO_F;
extern PObject *P_M_ONE_F;
extern PObject *P_EMPTY_S;
extern PObject *P_SPACE_S;
extern PObject *P_NEWLINE_S;

extern VM *vmpnt;

#define _vm (*vmpnt)
#define _vmpnt vmpnt


#endif
