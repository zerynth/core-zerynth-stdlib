/**

**********************************
Operative System Abstraction Layer
**********************************

The Zerynth VM uses a common API to create and manage threads and synchronization objects. Such API is called VOSAL
and abstracts the details of the underlying RTOS, so that it can be changed as needed for performance or licensing reasons.



*/


#ifndef __VOSAL__
#define __VOSAL__

#include "viperlib.h"
#include "vboard.h"
#if defined(VM_OTA)
#include "ota.h"
#endif


#ifndef BYTECODE_ACCESS_ALIGNED_4
#define __memcpy(d,s,sz) memcpy(d,s,sz)
#define __memcmp(d,s,sz) memcmp(d,s,sz)
#else
#define __memcpy(d,s,sz) memcpy4(d,s,sz)
#define __memcmp(d,s,sz) memcmp4(d,s,sz)
#endif

#define ALIGNED_TO(x) __attribute__((__aligned__(x)))



/**
=====
Types
=====

*/
/**
.. type:: VThread

  VThread is defined as a pointer to a region of memory containing a thread representation. In order to be as abstract as possible the actual type of VThread is void*. Values of type VThread must be managed only by calling vosal functions starting with **vosTh**.
*/
typedef void *VThread;

/**
.. type:: VSemaphore

  VSemaphore is defined as a pointer to a region of memory containing a semaphore representation. In order to be as abstract as possible the actual type of VSemaphore is void*. Values of type VSemaphore must be managed only by calling vosal functions starting with **vosSem**.
*/
typedef void *VSemaphore;

typedef void *VMutex;

/**
.. type:: VSysTimer

  VSysTimer is defined as a pointer to a region of memory containing a rtos timer representation. In order to be as abstract as possible the actual type of VSysTimer is void*. Values of type VSysTimer must be managed only by calling vosal functions starting with **vosTimer**.
*/
typedef void *VSysTimer;

/**
.. type:: VMailBox

  VMailBox is defined as a pointer to a region of memory containing a rtos mailbox representation. In order to be as abstract as possible the actual type of VMailBox is void*. Values of type VMailBox must be managed only by calling vosal function starting with **vosMBox**. Mailboxes are an optional feature and can be disabled when compiling a VM.
*/
typedef void *VMailBox;

/**
.. type:: VFifo

  VFifo is defined as a pointer to a region of memory containing a rtos fifo. In order to be as abstract as possible the actual type of VFifo is void*. Values of type VFifo must be managed only by calling vosal function starting with **vosFifo**. Fifos are an optional feature and can be disabled when compiling a VM.
*/
typedef void *VFifo;


/**
.. type:: typedef void(*vsystimer_fn)(void *)

  Type of a timer callback function
*/
typedef void (*vsystimer_fn)(void *);
typedef void (*vos_irq_handler)(void);

#if !defined(CORTEX_VECTOR_COUNT)
#define CORTEX_VECTOR_COUNT 0
#endif

typedef struct {
  uint32_t      *init_stack;
  vos_irq_handler  reset_vector;
  vos_irq_handler  nmi_vector;
  vos_irq_handler  hardfault_vector;
  vos_irq_handler  memmanage_vector;
  vos_irq_handler  busfault_vector;
  vos_irq_handler  usagefault_vector;
  vos_irq_handler  vector1c;
  vos_irq_handler  vector20;
  vos_irq_handler  vector24;
  vos_irq_handler  vector28;
  vos_irq_handler  svcall_vector;
  vos_irq_handler  debugmonitor_vector;
  vos_irq_handler  vector34;
  vos_irq_handler  pendsv_vector;
  vos_irq_handler  systick_vector;
  vos_irq_handler  vectors[CORTEX_VECTOR_COUNT];
} vos_vectors;


/**

=========
Variables
=========

*/
extern VSemaphore _memlock;
extern VSemaphore _dbglock;
extern VSemaphore _gillock;
/**

.. var:: volatile uint32_t _systime_seconds

  Holds the number of seconds passed since VM startup. It is not guaranteed to be implemented in every architecture or RTOS.

.. var:: volatile uint32_t _systime_millis

  Holds the number of milliseconds passed since VM startup. It is not guaranteed to be implemented in every architecture or RTOS.

.. var:: volatile uint32_t _systime_frequency

  Holds the current frequency of the microcontroller in Hz.

*/

extern volatile uint32_t _systime_seconds;
extern volatile uint32_t _systime_millis;
extern volatile uint32_t _system_frequency;


/**

======
Macros
======


Time Macros
-----------

.. macro:: NANOS

  Represent the time unit nanoseconds

.. macro:: MICROS

  Represent the time unit microseconds

.. macro:: MILLIS

  Represent the time unit milliseconds

.. macro:: SECONDS

  Represent the time unit seconds

.. macro:: TIME_U(value, unit)

  Time values in VOSAL are represented as value plus time unit. This macro creates a valid time representation mixing together *value* (max 30 significant bits) with *unit*. Every vosal function expecting a time must be passed the result of TIME_U

.. macro:: GET_TIME_UNIT(time)

  Extracts the time unit from *time*

.. macro:: GET_TIME_VALUE(time)

  Extracts the time value from *time*

.. macro:: GET_TIME_MICROS(time)

  Extracts the time value from *time* converting it to microseconds

.. macro:: TIME_IS_ZERO(time)

  Evaluates to true if *time* is zero in the corresponding time unit

.. macro:: VTIME_IMMEDIATE

  A unitless time value representing no time at all

.. macro:: VTIME_INFINITE

  Infinite amount of time, used when an infinite timeout is requested

*/
#define VTIME_IMMEDIATE  ((uint32_t)0)
#define VTIME_INFINITE  ((uint32_t)-1)


/**

Priority Macros
---------------

.. macro:: VOS_PRIO_IDLE

  This priority should not be used with normal threads. Only one thread exists with this priority and it is created at startup

.. macro:: VOS_PRIO_LOWEST

  Lowest possible thread priority.

.. macro:: VOS_PRIO_LOWER

  This priority is greater than all preceding thread priorities

.. macro:: VOS_PRIO_LOW

  This priority is greater than all preceding thread priorities


.. macro:: VOS_PRIO_NORMAL

  This priority is greater than all preceding thread priorities. It is the default priority.

.. macro:: VOS_PRIO_HIGH

  This priority is greater than all preceding thread priorities

.. macro:: VOS_PRIO_HIGHER

  This priority is greater than all preceding thread priorities

.. macro:: VOS_PRIO_HIGHEST

  This priority is greater than all preceding thread priorities.


Thread Status
-------------

.. macro:: VTHREAD_READY

  Status of a thread that has been created but not yet run.

.. macro:: VTHREAD_RUNNING

  Status of a thread that is currently running.

.. macro:: VTHREAD_INACTIVE

  Status of a thread that has finished its lifecycle


.. macro:: VTHREAD_WAITING

  Status of a thread that is sleeping waiting for some event to occur

=============
Return values
=============

.. macro:: VRES_OK

  If returned, the function execution encountered no problems.

.. macro:: VRES_TIMEOUT

  If returned, the function exited with a timeout condition.

.. macro:: VRES_RESET

  If returned, the function exited with a reset condition.

*/
#define VOS_PRIO_IDLE 0
#define VOS_PRIO_LOWEST 1
#define VOS_PRIO_LOWER 2
#define VOS_PRIO_LOW 3
#define VOS_PRIO_NORMAL 4
#define VOS_PRIO_HIGH 5
#define VOS_PRIO_HIGHER 6
#define VOS_PRIO_HIGHEST 7

#define VTHREAD_READY      0
#define VTHREAD_RUNNING    1
#define VTHREAD_WAITING    2
#define VTHREAD_INACTIVE   3

#define VRES_OK 0
#define VRES_TIMEOUT -1
#define VRES_RESET -2



/**

===========================
System and Thread functions
===========================

.. function:: vosSysLock(void)

  Locks the system, usually by disabling interrupts. Thread scheduling is suspended and the following code is executed atomically. It can not be used inside ISRs.

.. function:: vosSysUnlock(void)

  Unlocks the system, usually by enabling interrupts. It is used together with :func:`vosSysLock` to protect small critical sections. It can not be used inside ISRs.

.. function:: vosSysLockIsr(void)

  Same as :func:`vosSysLock`, but must be called only inside an ISR.

.. function:: vosSysUnlockIsr(void)

  Same as :func:`vosSysUnlock`, but must be called only inside an ISR.

.. function:: vosEnterIsr(void)

  Must be called when entering an ISR. Depending on the underlyng RTOS and architecture it can be an empty macro.

.. function:: vosExitIsr(void)

  Must be called when exiting an ISR. Depending on the underlyng RTOS and architecture it can be an empty macro.

.. function:: vos_irq_handler vosInstallHandler(int32_t hpos, vos_irq_handler fn)

  Install a new ISR *fn* at index *hpos* in the interrupt table. Previous ISR is returned.

  The type vos_irq_handler is: ::

    typedef void (*vos_irq_handler)(void)

.. function:: void vosSysReset(void)

  Soft reset the microcontroller.
*/

/* SYSTEM */
void vosInit(void *data);
void vosSysLock(void);
void vosSysUnlock(void);
void vosSysLockIsr(void);
void vosSysUnlockIsr(void);
void vosSysReset(void);




//isr drama
#if defined(RTOS_IRQ_PROLOGUE)

#if defined(MCU_CORTEX_M0)

     #define vosEnterIsr()                                                 \
       void* _saved_lr;                                                       \
       asm volatile ("mov     %0, lr" : "=r" (_saved_lr) : : "memory")

#else
    extern void _port_irq_prologue(void);
    #define vosEnterIsr() _port_irq_prologue()
#endif /* MCU_CORTEX_M0 */

#else
    #define vosEnterIsr()
#endif /* RTOS_IRQ_PROLOGUE */

#if defined(RTOS_IRQ_EPILOGUE)

#if defined(MCU_CORTEX_M0)
    extern void _port_irq_epilogue(void* _saved_lr);
    #define vosExitIsr() _port_irq_epilogue(_saved_lr)
#endif /* MCU_CORTEX_M0 */
#else
    extern void _port_irq_epilogue(void);
    #define vosExitIsr() _port_irq_epilogue()

#endif /* RTOS_IRQ_EPILOGUE */

    //#if defined(RTOS_IRQ_PROLOGUE)
    //    extern void _port_irq_prologue(void);
    //    #define vosEnterIsr() _port_irq_prologue()
    //#else
    //    #define vosEnterIsr()
    //#endif /*RTOS_IRQ_PROLOGUE*/

//#endif /*MCU_CORTEX_M0*/

// extern void _port_irq_epilogue(void);
// #define vosEnterIsr()
// #define vosExitIsr() _port_irq_epilogue()

uint32_t time_to_ticks(uint32_t time);
uint32_t* vosTicks(void);
uint32_t vosMillis(void);
vos_irq_handler vosInstallHandler(int32_t hpos, vos_irq_handler fn);


/**

.. function:: VThread vosThCreate(uint32_t size, int32_t prio, void *fn, void *arg, void *data)

  Create a VThread with a free workspace memory of *size* bytes. Actual memory usage is greater than *size* of an amount dependent on the underlying RTOS data structures. *prio* is the starting priority of the thread. The function run inside the thread is *fn* to which a single argument *arg* is passed. Some *data* can be associated to the created thread.

  After creation, the thread is not started. A call to :func:`vosThResume` is necessary to start the execution of *fn*.

*/
VThread vosThCreate(uint32_t size, int32_t prio, void *fn, void *arg, void *data);

/**

.. function:: void vosThDestroy(VThread th)

  Frees the memory associated to *th*. Thread *th* must be terminated in order to call vosThDestroy, otherwise memory corruption ensues.

*/
void vosThDestroy(VThread th);

/**

.. function:: void vosThSetData(VThread th, void *data)

  Attaches *data* to *th*.

*/
void vosThSetData(VThread th, void *data);
/**

.. function:: void* vosThGetData(VThread th)

  Retrieves data attached to *th*

*/
void *vosThGetData(VThread th);

/**

.. function:: void* vosThGetId(VThread th)

  Every created thread has a unique 32 bits id. Returns such id.

*/
uint32_t vosThGetId(VThread th);


/**

.. function:: uint32_t vosThGetStatus(VThread th)

  Returns the status of *th*. Possible return values are all the thread status macros.

*/
uint32_t vosThGetStatus(VThread th);


/**

.. function:: void vosThSetPriority(int32_t prio)

  Changes the priority of the current thread. Values for *prio* are all the priority macros.

*/
void vosThSetPriority(int32_t prio);

/**

.. function:: int32_T vosThGetPriority(void)

  Returns the priority of the current thread.

*/
int32_t vosThGetPriority(void);


/**

.. function:: VThread vosThCurrent(void)

  Returns the current thread.

*/
VThread vosThCurrent(void);

/**

.. function:: void vosThSleep(time)

  Suspend the current thread for an amount of time represented by *time*. *time* must be passed using :macro:`TIME_U`. The precision of vosThSleep depends on the priority of other threads with respect t the current one and to the scheduling strategy of the underlying RTOS.

*/
void vosThSleep(uint32_t time);

/**

.. function:: void vosThYield()

  The current thread is suspended and the control is given to the next thread in the scheduling order.
*/
void vosThYield(void);

/**

.. function:: void vosThSuspend()

  The current thread is suspended until the next call to :func:`vosThResume`. This function must be preceded by a call to :func:`vosSysLock`. The implementation of vosThSuspend will remove the lock if necessary

*/
void vosThSuspend(void);

/**

.. function:: VThread vosThResume(VThread th)

  Resume the thread *th*. *th* must be a thread just created (and never resumed before) or suspended by :func:`vosThSuspend`.

*/
VThread vosThResume(VThread th);

/**

.. function:: VThread vosThResumeIsr(VThread th)

  Resume the thread *th*. *th* must be a thread just created (and never resumed before) or suspended by :func:`vosThSuspend`. It must be used only inside an ISR.

*/
VThread vosThResumeIsr(VThread th);

void vosThReschedule(void);



/**

==========
Semaphores
==========


.. function:: VSemaphore vosSemCreate(uint32_t n)

  Creates a VSemaphore with initial value *n*

*/
VSemaphore vosSemCreate(uint32_t n);
/**
.. function:: void vosSemReset(VSemaphore sem)

  Reset the semaphore *sem*. Every thread waiting on *sem* is woken up and the semaphore reset signaled with :macr:`VRES_RESET`.

*/
void vosSemReset(VSemaphore sem);


/**
.. function:: void vosSemDestroy(VSemaphore sem)

  Reset *sem* and free memory used by *sem*.
*/
void vosSemDestroy(VSemaphore sem);


/**
.. function:: void vosSemSignal(VSemaphore sem)

  Signal the semaphore *sem*. If any thread is waiting on the sempahore, one thread is woken up. If no threads are waiting, the semaphore value is increased by one.

*/
void vosSemSignal(VSemaphore sem);


/**
.. function:: void vosSemSignalCap(VSemaphore sem, uint32_t cap)

  Signal the semaphore *sem*. If any thread is waiting on the sempahore, one thread is woken up. If no threads are waiting, the semaphore value is increased by one only if it is less than *cap*.

*/
void vosSemSignalCap(VSemaphore sem, uint32_t cap);

/**
.. function:: void vosSemSignalIsr(VSemaphore sem)

  Same as :func:`vosSemSignal` but must be used only inside ISRs.

*/
void vosSemSignalIsr(VSemaphore sem);


/**
.. function:: int32_t vosSemWaitTimeout(VSemaphore sem, uint32_t timeout)

  Wait on semaphore *sem*. If the value of *sem* is zero or less, the thread is suspended for a time equal to *timeout* (passed with :macro:`TIME_U`). If the value of *sem* is one or greater, it is decreased by one and the thread is not suspended. Return :macro:`VRES_OK` if the timeout is not triggered, :macro:`VRES_TIMEOUT` if it is triggered or :macro:`VRES_RESET` if :func:`vosSemReset` is called on *sem*.

*/
int32_t vosSemWaitTimeout(VSemaphore sem, uint32_t timeout);

/**
.. function:: int32_t vosSemWait(VSemaphore sem)

  It is implemented as a macro, calling :func:`voSemWaitTimeout` with *timeout* equal to :macro:`VTIME_INFINITE`.
*/
#define vosSemWait(sem) vosSemWaitTimeout(sem,VTIME_INFINITE);

/**
.. function:: int32_t vosSemGetValue(VSemaphore sem)

  Return the current value of *sem*. This function must be called inside a system lock.

*/
int32_t vosSemGetValue(VSemaphore sem);


/**
.. function:: int32_t vosSemTryWait(VSemaphore sem)

  Try to wait on semaphore *sem*. Return :macro:`VRES_OK` if the semaphore has been taken, :macro:`VRES_TIMEOUT` or :macro:`VRES_RESET` if the semaphore can't be taken without blocking.
  
*/
#define vosSemTryWait(sem) vosSemWaitTimeout(sem,TIME_U(0,MILLIS))

/**
.. function:: int32_t vosSemSignalWait(VSemaphore semS,VSemaphore semW)

  Atomically signal semaphore *semS* and wait with timeout on *semW*. Return :macro:`VRES_OK` if the *semW* is taken or :macro:`VRES_RESET` if :func:`vosSemReset` is called on *semW*.
  
*/
int32_t vosSemSignalWait(VSemaphore semS,VSemaphore semW);



/* Mutexes */
#ifndef VM_NO_MUTEXES
VMutex vosMtxCreate(void);
void vosMtxLock(VMutex mtx);
int32_t vosMtxTryLock(VMutex mtx);
VMutex vosMtxUnlock(VMutex mtx);
void vosMtxDestroy(VMutex mtx);
#endif


/**

=========
Mailboxes
=========


.. function:: VMailBox vosMBoxCreate(int32_t n)

  Creates an empty VMailBox with a size of *n* elements.

*/
VMailBox vosMBoxCreate(int n);

/**
.. function:: VMailBox vosMBoxDestroy(VMailBox mb)

  Reset *mb* and free memory used by *mb*.

*/
void vosMBoxDestroy(VMailBox mb);

/**
.. function:: int vosMBoxPostTimeout(VMailBox mb, void* msg, uint32_t timeout)

  Try to insert message *msg* in *mb*. If *mb* is full, the thread is suspended on *mb* for a time less than or equal to *timeout* (passed with :macro:`TIME_U`). If *mb* has at least one free slot, *msg* is inserted in *mb* and the thread is not suspended. Return one of the VRES macros.

*/
int vosMBoxPostTimeout(VMailBox mb, void *msg, uint32_t timeout);
/**
.. function:: int vosMBoxFetchTimeout(VMailBox mb, void** msgp, uint32_t timeout)

  Try to retrieve a message from *mb* returning a pointer to it in *msgp*. If *mb* is empty, the thread is suspended on *mb* for a time les than or equal to *timeout* (passed with :macro:`TIME_U`). If *mb* has at least one message, *msgp* is assigned such message and the thread is not suspended. Return one of the VRES macros.

*/
int vosMBoxFetchTimeout(VMailBox mb, void **msgp, uint32_t timeout);

/**
.. function:: int vosMBoxPost(VMailBox mb, void* msg)

  Same as :func:`vosMBoxPostTimeout` but with infinite timeout.

*/
#define vosMBoxPost(mb,msgp) vosMBoxPostTimeout(mb,msgp,VTIME_INFINITE)

/**
.. function:: int vosMBoxFetch(VMailBox mb, void** msgp)

  Same as :func:`vosMBoxFetchTimeout` but with infinite timeout.

*/
#define vosMBoxFetch(mb,msgp) vosMBoxFetchTimeout(mb,msgp,VTIME_INFINITE)

/**
.. function:: int vosMBoxPostIsr(VMailBox mb, void* msg)

  Same as :func:`vosMBoxPostTimeout` but with infinite timeout and can be used only in an ISR..

*/
int vosMBoxPostIsr(VMailBox mb, void *msg);
/**
.. function:: int vosMBoxFetchIsr(VMailBox mb, void** msgp)

  Same as :func:`vosMBoxFetchTimeout` but with infinite timeout and can be used only in an ISR.

*/
int vosMBoxFetchIsr(VMailBox mb, void **msgp);

/**
.. function:: int vosMBoxUsedSlots(VMailBox mb)

  Returns the number of messages in *mb*. Must be called in a system lock.

*/
int vosMBoxUsedSlots(VMailBox mb);

/**
.. function:: int vosMBoxFreeSlots(VMailBox mb)

  Returns the number of free slots in *mb*. Must be called in a system lock.

*/
int vosMBoxFreeSlots(VMailBox mb);


typedef int (*fifo_put_fn)(VFifo,uint8_t);
typedef int (*fifo_get_fn)(VFifo);

VFifo vosFifoCreate(uint16_t size,fifo_put_fn putfn,fifo_get_fn getfn, void *arg);
void vosFifoDestroy(VFifo fifo);


int vosFifoPut(VFifo fifo, uint8_t item);
int vosFifoPutTimeout(VFifo fifo, uint8_t item, uint32_t timeout);
int vosFifoPutNonBlock(VFifo fifo, uint8_t item);

int vosFifoGet(VFifo fifo);
int vosFifoGetTimeout(VFifo fifo, uint32_t timeout);
int vosFifoGetNonBlock(VFifo fifo);

int vosFifoItems(VFifo fifo);


/* Timers */
#define ticks_to_micros(tk) ((tk)/(_system_frequency/1000000))
#define ticks_to_millis(tk) ((tk)/(_system_frequency/1000))
#define ticks_to_seconds(tk) ((tk)/(_system_frequency))

/**

=============
System Timers
=============


.. function:: VSysTimer vosTimerCreate(void)

  Creates an inactive system timer.

*/
VSysTimer vosTimerCreate(void);

/**
.. function:: uint32_t vosTimerReadMillis(VSysTimer tm)

  Returns the amount of milliseconds elapsed since *tm* creation or last reset.

*/
uint32_t vosTimerReadMillis(VSysTimer tm);

/**
.. function:: uint32_t vosTimerReadMicros(VSysTimer tm)

  Returns the amount of microseconds elapsed since *tm* creation or last reset.

*/
uint32_t vosTimerReadMicros(VSysTimer tm);

/**
.. function:: uint32_t vosTimerOneShot(VSysTimer tm, uint32_t time, vsystimer_fn fn, void *arg)

  Configure the timer *tm* such that after *time* the function *fn* is execute exactly once as *fn*(*arg*) inside an ISR. Must be called inside a system lock.

*/
void vosTimerOneShot(VSysTimer tm, uint32_t time, vsystimer_fn fn, void *arg);
/**
.. function:: uint32_t vosTimerRecurrent(VSysTimer tm, uint32_t time, vsystimer_fn fn, void *arg)

  Configure the timer *tm* such that the function *fn* is execute periodically with period equal to *time*, as *fn(arg)* inside an ISR. Must be called inside a system lock.

*/
void vosTimerRecurrent(VSysTimer tm, uint32_t time, vsystimer_fn fn, void *arg);

/**
.. function:: void vosTimerReset(VSysTimer tm)

  Reset the timer. Every configured callback function is removed and any subsequent call to :func:`vosTimerReadMillis` or :func:`vosTimerReadMicros` will be relative to the time of reset. Must be called inside a system lock.

*/
void vosTimerReset(VSysTimer tm);

/**
.. function:: void vosTimerDestroy(VSysTimer tm)

  Reset the timer and free its memory.

*/
void vosTimerDestroy(VSysTimer tm);
void sleep_polling(uint32_t time);

/* Queues */
typedef void *VQueue;
VQueue vosQueueCreate(uint32_t size);
int vosQueueRead(VQueue q, uint8_t *buf, uint32_t len);
int vosQueueWrite(VQueue q, uint8_t *buf, uint32_t len);


void vosSysSetClock(uint32_t mode);

#endif
