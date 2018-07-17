.. default-domain:: c


**********************************
Operative System Abstraction Layer
**********************************

The Zerynth VM uses a common API to create and manage threads and synchronization objects. Such API is called VOSAL
and abstracts the details of the underlying RTOS, so that it can be changed as needed for performance or licensing reasons.



=====
Types
=====

.. type:: VThread

  VThread is defined as a pointer to a region of memory containing a thread representation. In order to be as abstract as possible the actual type of VThread is void*. Values of type VThread must be managed only by calling vosal functions starting with **vosTh**.
.. type:: VSemaphore

  VSemaphore is defined as a pointer to a region of memory containing a semaphore representation. In order to be as abstract as possible the actual type of VSemaphore is void*. Values of type VSemaphore must be managed only by calling vosal functions starting with **vosSem**.
.. type:: VSysTimer

  VSysTimer is defined as a pointer to a region of memory containing a rtos timer representation. In order to be as abstract as possible the actual type of VSysTimer is void*. Values of type VSysTimer must be managed only by calling vosal functions starting with **vosTimer**.
.. type:: VMailBox

  VMailBox is defined as a pointer to a region of memory containing a rtos mailbox representation. In order to be as abstract as possible the actual type of VMailBox is void*. Values of type VMailBox must be managed only by calling vosal function starting with **vosMBox**. Mailboxes are an optional feature and can be disabled when compiling a VM.
.. type:: VFifo

  VFifo is defined as a pointer to a region of memory containing a rtos fifo. In order to be as abstract as possible the actual type of VFifo is void*. Values of type VFifo must be managed only by calling vosal function starting with **vosFifo**. Fifos are an optional feature and can be disabled when compiling a VM.
.. type:: typedef void(*vsystimer_fn)(void *)

  Type of a timer callback function

=========
Variables
=========


.. var:: volatile uint32_t _systime_seconds

  Holds the number of seconds passed since VM startup. It is not guaranteed to be implemented in every architecture or RTOS.

.. var:: volatile uint32_t _systime_millis

  Holds the number of milliseconds passed since VM startup. It is not guaranteed to be implemented in every architecture or RTOS.

.. var:: volatile uint32_t _systime_frequency

  Holds the current frequency of the microcontroller in Hz.


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

.. function:: VThread vosThCreate(uint32_t size, int32_t prio, void *fn, void *arg, void *data)

  Create a VThread with a free workspace memory of *size* bytes. Actual memory usage is greater than *size* of an amount dependent on the underlying RTOS data structures. *prio* is the starting priority of the thread. The function run inside the thread is *fn* to which a single argument *arg* is passed. Some *data* can be associated to the created thread.

  After creation, the thread is not started. A call to :func:`vosThResume` is necessary to start the execution of *fn*.


.. function:: void vosThDestroy(VThread th)

  Frees the memory associated to *th*. Thread *th* must be terminated in order to call vosThDestroy, otherwise memory corruption ensues.


.. function:: void vosThSetData(VThread th, void *data)

  Attaches *data* to *th*.


.. function:: void* vosThGetData(VThread th)

  Retrieves data attached to *th*


.. function:: void* vosThGetId(VThread th)

  Every created thread has a unique 32 bits id. Returns such id.


.. function:: uint32_t vosThGetStatus(VThread th)

  Returns the status of *th*. Possible return values are all the thread status macros.


.. function:: void vosThSetPriority(int32_t prio)

  Changes the priority of the current thread. Values for *prio* are all the priority macros.


.. function:: int32_T vosThGetPriority(void)

  Returns the priority of the current thread.


.. function:: VThread vosThCurrent(void)

  Returns the current thread.


.. function:: void vosThSleep(time)

  Suspend the current thread for an amount of time represented by *time*. *time* must be passed using :macro:`TIME_U`. The precision of vosThSleep depends on the priority of other threads with respect t the current one and to the scheduling strategy of the underlying RTOS.


.. function:: void vosThYield()

  The current thread is suspended and the control is given to the next thread in the scheduling order.

.. function:: void vosThSuspend()

  The current thread is suspended until the next call to :func:`vosThResume`. This function must be preceded by a call to :func:`vosSysLock`. The implementation of vosThSuspend will remove the lock if necessary


.. function:: VThread vosThResume(VThread th)

  Resume the thread *th*. *th* must be a thread just created (and never resumed before) or suspended by :func:`vosThSuspend`.


.. function:: VThread vosThResumeIsr(VThread th)

  Resume the thread *th*. *th* must be a thread just created (and never resumed before) or suspended by :func:`vosThSuspend`. It must be used only inside an ISR.


==========
Semaphores
==========


.. function:: VSemaphore vosSemCreate(uint32_t n)

  Creates a VSemaphore with initial value *n*

.. function:: void vosSemReset(VSemaphore sem)

  Reset the semaphore *sem*. Every thread waiting on *sem* is woken up and the semaphore reset signaled with :macr:`VRES_RESET`.

.. function:: void vosSemDestroy(VSemaphore sem)

  Reset *sem* and free memory used by *sem*.
.. function:: void vosSemSignal(VSemaphore sem)

  Signal the semaphore *sem*. If any thread is waiting on the sempahore, one thread is woken up. If no threads are waiting, the semaphore value is increased by one.

.. function:: void vosSemSignalCap(VSemaphore sem, uint32_t cap)

  Signal the semaphore *sem*. If any thread is waiting on the sempahore, one thread is woken up. If no threads are waiting, the semaphore value is increased by one only if it is less than *cap*.

.. function:: void vosSemSignalIsr(VSemaphore sem)

  Same as :func:`vosSemSignal` but must be used only inside ISRs.

.. function:: int32_t vosSemWaitTimeout(VSemaphore sem, uint32_t timeout)

  Wait on semaphore *sem*. If the value of *sem* is zero or less, the thread is suspended for a time equal to *timeout* (passed with :macro:`TIME_U`). If the value of *sem* is one or greater, it is decreased by one and the thread is not suspended. Return :macro:`VRES_OK` if the timeout is not triggered, :macro:`VRES_TIMEOUT` if it is triggered or :macro:`VRES_RESET` if :func:`vosSemReset` is called on *sem*.

.. function:: int32_t vosSemWait(VSemaphore sem)

  It is implemented as a macro, calling :func:`voSemWaitTimeout` with *timeout* equal to :macro:`VTIME_INFINITE`.
.. function:: int32_t vosSemGetValue(VSemaphore sem)

  Return the current value of *sem*. This function must be called inside a system lock.

.. function:: int32_t vosSemTryWait(VSemaphore sem)

  Try to wait on semaphore *sem*. Return :macro:`VRES_OK` if the semaphore has been taken, :macro:`VRES_TIMEOUT` or :macro:`VRES_RESET` if the semaphore can't be taken without blocking.
  
.. function:: int32_t vosSemSignalWait(VSemaphore semS,VSemaphore semW)

  Atomically signal semaphore *semS* and wait with timeout on *semW*. Return :macro:`VRES_OK` if the *semW* is taken or :macro:`VRES_RESET` if :func:`vosSemReset` is called on *semW*.
  

=========
Mailboxes
=========


.. function:: VMailBox vosMBoxCreate(int32_t n)

  Creates an empty VMailBox with a size of *n* elements.

.. function:: VMailBox vosMBoxDestroy(VMailBox mb)

  Reset *mb* and free memory used by *mb*.

.. function:: int vosMBoxPostTimeout(VMailBox mb, void* msg, uint32_t timeout)

  Try to insert message *msg* in *mb*. If *mb* is full, the thread is suspended on *mb* for a time less than or equal to *timeout* (passed with :macro:`TIME_U`). If *mb* has at least one free slot, *msg* is inserted in *mb* and the thread is not suspended. Return one of the VRES macros.

.. function:: int vosMBoxFetchTimeout(VMailBox mb, void** msgp, uint32_t timeout)

  Try to retrieve a message from *mb* returning a pointer to it in *msgp*. If *mb* is empty, the thread is suspended on *mb* for a time les than or equal to *timeout* (passed with :macro:`TIME_U`). If *mb* has at least one message, *msgp* is assigned such message and the thread is not suspended. Return one of the VRES macros.

.. function:: int vosMBoxPost(VMailBox mb, void* msg)

  Same as :func:`vosMBoxPostTimeout` but with infinite timeout.

.. function:: int vosMBoxFetch(VMailBox mb, void** msgp)

  Same as :func:`vosMBoxFetchTimeout` but with infinite timeout.

.. function:: int vosMBoxPostIsr(VMailBox mb, void* msg)

  Same as :func:`vosMBoxPostTimeout` but with infinite timeout and can be used only in an ISR..

.. function:: int vosMBoxFetchIsr(VMailBox mb, void** msgp)

  Same as :func:`vosMBoxFetchTimeout` but with infinite timeout and can be used only in an ISR.

.. function:: int vosMBoxUsedSlots(VMailBox mb)

  Returns the number of messages in *mb*. Must be called in a system lock.

.. function:: int vosMBoxFreeSlots(VMailBox mb)

  Returns the number of free slots in *mb*. Must be called in a system lock.


=============
System Timers
=============


.. function:: VSysTimer vosTimerCreate(void)

  Creates an inactive system timer.

.. function:: uint32_t vosTimerReadMillis(VSysTimer tm)

  Returns the amount of milliseconds elapsed since *tm* creation or last reset.

.. function:: uint32_t vosTimerReadMicros(VSysTimer tm)

  Returns the amount of microseconds elapsed since *tm* creation or last reset.

.. function:: uint32_t vosTimerOneShot(VSysTimer tm, uint32_t time, vsystimer_fn fn, void *arg)

  Configure the timer *tm* such that after *time* the function *fn* is execute exactly once as *fn*(*arg*) inside an ISR. Must be called inside a system lock.

.. function:: uint32_t vosTimerRecurrent(VSysTimer tm, uint32_t time, vsystimer_fn fn, void *arg)

  Configure the timer *tm* such that the function *fn* is execute periodically with period equal to *time*, as *fn(arg)* inside an ISR. Must be called inside a system lock.

.. function:: void vosTimerReset(VSysTimer tm)

  Reset the timer. Every configured callback function is removed and any subsequent call to :func:`vosTimerReadMillis` or :func:`vosTimerReadMicros` will be relative to the time of reset. Must be called inside a system lock.

.. function:: void vosTimerDestroy(VSysTimer tm)

  Reset the timer and free its memory.


======
Events
======


.. function:: VEvent vosEventCreate(void)

  Creates a VEvent. A VEvent manages a flag that can be set or cleared and on whose value a thread can be blocked.
  The flag is initially false.

.. function:: void vosEventSet(VEvent event)

  Set the event flag.

.. function:: void vosEventClear(VEvent event)

  Clear the event flag.

.. function:: int32_t vosEventWait(VEvent event, uint32_t timeout)

  Wait for the event flag to be set. Return :macro:`VRES_OK` if the flag has been set, :macro:`VRES_TIMEOUT` if it has not been set in given timeout time,

.. function:: int32_t vosEventGetFlag(VEvent event)

  Get current flag value for selected event,

.. function:: void vosEventDestroy(VEvent event)

  Destroy the event and frees its memory.

