#include "zerynth.h"

#define RTOS__SEM_CREATE   0
#define RTOS__SEM_SIGNAL   1
#define RTOS__SEM_WAIT     2
#define RTOS__SEM_RESET    3
#define RTOS__SEM_TRYLOCK  4
#define RTOS__SEM_SIGNAL_WAIT 5
#define RTOS__SEM_SIGNAL_IF_WAITING 6
#define RTOS__SEM_SIGNAL_ALL 7
#define RTOS__THD_CURRENT   8
#define RTOS__EVT_CREATE  9
#define RTOS__EVT_SET     10
#define RTOS__EVT_CLEAR   11
#define RTOS__EVT_WAIT    12
#define RTOS__EVT_GETFLAG 13
#define RTOS__SEM_SIGNALCAP 14


C_NATIVE(__rtos_do) {
    NATIVE_UNWARN();
    CHECK_ARG(args[0], PSMALLINT);
    CHECK_ARG(args[1], PTUPLE); //vargs
    int32_t code = PSMALLINT_VALUE(args[0]);
    PSysObject *oo;
    PObject **argv = PSEQUENCE_OBJECTS(args[1]);
    int32_t nargv = PSEQUENCE_ELEMENTS(args[1]);
    *res = MAKE_NONE();

    switch (code) {
        case RTOS__SEM_CREATE: {
            int32_t value = 1;
            if (nargv >= 1) {
                CHECK_ARG(argv[0], PSMALLINT);
                value = PSMALLINT_VALUE(argv[0]);
                if (value < 0) return ERR_VALUE_EXC;
            }
            oo = psysobj_new(PSYS_SEMAPHORE);
            oo->sys.sem = vosSemCreate(value);
            printf("+sem_create %x\r\n", oo);
            *res = (PObject *)oo;
        }
        break;
        case RTOS__SEM_SIGNAL: {
            if (nargv != 1)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+sem_signal %x\r\n", oo);
            vosSemSignal(oo->sys.sem);
            printf("-sem_signal %x\r\n", oo);
        }
        break;

        case RTOS__SEM_WAIT: {
            CHECK_ARG(argv[0], PSYSOBJ);
            int32_t timeout = -1;
            if (nargv > 1) {
                CHECK_ARG(argv[1], PSMALLINT);
                timeout = PSMALLINT_VALUE(argv[1]);
            }
            oo  = (PSysObject *)argv[0];
            //printf("RTOS LOCK\n");
            printf("+sem_wait %i %x\n",timeout,oo);
            RELEASE_GIL();
            timeout = vosSemWaitTimeout(oo->sys.sem, timeout <= 0 ? VTIME_INFINITE : (uint32_t)TIME_U(timeout, MILLIS));
            ACQUIRE_GIL();
            printf("-sem_wait %i %x\n",timeout,oo);
            *res = (PObject *)PSMALLINT_NEW(timeout);
        }
        break;

        case RTOS__SEM_RESET: {
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+sem_reset %i %x\n", code,oo);
            vosSemReset(oo->sys.sem);
            printf("-sem_reset %i %x\n", code,oo);
        }
        break;
        case RTOS__SEM_TRYLOCK: {
            if (nargv != 1)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+try_lock sem for cmd %i %x\n", code,oo);
            RELEASE_GIL();
            if (vosSemTryWait(oo->sys.sem)==VRES_OK){
                *res = PSMALLINT_NEW(0);
            } else {
                *res = PSMALLINT_NEW(-1);
            }
            // SYSLOCK();
            // int32_t cnt = vosSemGetValue(oo->sys.sem);
            // if (cnt > 0) {
            //     //semaphore not taken, take it
            //     vosSemWait(oo->sys.sem);// !!! this call will remove the lock
            //     *res = PSMALLINT_NEW(0);
            // } else {
            //     *res = PSMALLINT_NEW(-1);
            //     SYSUNLOCK();
            // } //semaphore taken
            ACQUIRE_GIL();
            printf("-try_lock sem for cmd %i %x\n", code,oo);
            
        }
        break;
        case RTOS__SEM_SIGNAL_WAIT: {
            if (nargv < 2)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0]; //sem to be signaled
            int32_t timeout = -1;
            if (nargv >= 3) {
                CHECK_ARG(argv[2], PSMALLINT);
                timeout = PSMALLINT_VALUE(argv[2]);
            }
            printf("+signalling_waiting sem for cmd %i %x\n", code,oo);
            RELEASE_GIL();
            PSysObject *qq = (PSysObject *)argv[1]; //sem to be waited
            timeout = vosSemSignalWait(oo->sys.sem,qq->sys.sem);
            // SYSLOCK();
            // vosSemSignalIsr(oo->sys.sem);
            // oo  = (PSysObject *)argv[1]; //sem to be waited
            // timeout = vosSemWaitTimeout(oo->sys.sem, timeout <= 0 ? VTIME_INFINITE : (uint32_t)TIME_U(timeout, MILLIS));
            ACQUIRE_GIL();
            printf("-signalling_waiting sem for cmd %i %x\n", code,oo);
            *res = (PObject *) PSMALLINT_NEW(timeout);
        }
        break;
        case RTOS__SEM_SIGNAL_IF_WAITING:{
            if (nargv != 1)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+signalling_if_waiting sem for cmd %i\n", code);
            SYSLOCK();
            int32_t cnt = vosSemGetValue(oo->sys.sem);
            if (cnt<0){
                vosSemSignalIsr(oo->sys.sem);
            }
            SYSUNLOCK();
            printf("-signalling_if_waiting sem for cmd %i\n", code);
        }
        break;
        case RTOS__SEM_SIGNAL_ALL:{
            if (nargv != 1)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+signalling_all sem for cmd %i %x\n", code,oo);
            SYSLOCK();
            int32_t cnt = vosSemGetValue(oo->sys.sem);
            while (cnt<0){
                vosSemSignalIsr(oo->sys.sem);
                cnt = vosSemGetValue(oo->sys.sem);
            }
            SYSUNLOCK();
            printf("-signalling_all sem for cmd %i %x\n", code,oo);

        }
        break;
        case RTOS__THD_CURRENT: {
            *res = PSMALLINT_NEW(vosThGetId(vosThCurrent()));
        }
        break;
        case RTOS__EVT_CREATE: {
            oo = psysobj_new(PSYS_EVENT);
            oo->sys.evt = vosEventCreate();
            printf("+evt_create %x\r\n", oo);
            *res = (PObject *)oo;
        }
        break;
        case RTOS__EVT_SET: {
            if (nargv != 1)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+evt_set %x\r\n", oo);
            vosEventSet(oo->sys.evt);
            printf("-evt_set %x\r\n", oo);
        }
        break;
        case RTOS__EVT_CLEAR: {
            if (nargv != 1)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            printf("+evt_clear %x\r\n", oo);
            vosEventClear(oo->sys.evt);
            printf("-evt_clear %x\r\n", oo);
        }
        break;
        case RTOS__EVT_WAIT: {
            CHECK_ARG(argv[0], PSYSOBJ);
            int32_t timeout = -1;
            if (nargv > 1) {
                CHECK_ARG(argv[1], PSMALLINT);
                timeout = PSMALLINT_VALUE(argv[1]);
            }
            oo  = (PSysObject *)argv[0];
            RELEASE_GIL();
            timeout = vosEventWait(oo->sys.evt, timeout <= 0 ? VTIME_INFINITE : (uint32_t)TIME_U(timeout, MILLIS));
            ACQUIRE_GIL();
            *res = (PObject *)PSMALLINT_NEW(timeout);
        }
        break;
        case RTOS__EVT_GETFLAG: {
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
             int32_t flag = vosEventGetFlag(oo->sys.evt);
            *res = ((flag == 1) ? PBOOL_TRUE(): PBOOL_FALSE());
        }
        break;
        case RTOS__SEM_SIGNALCAP: {
            if (nargv < 1 || nargv > 2)
                return ERR_TYPE_EXC;
            CHECK_ARG(argv[0], PSYSOBJ);
            oo  = (PSysObject *)argv[0];
            int32_t cap = 1;
            if (nargv == 2) {
                CHECK_ARG(argv[1], PSMALLINT);
                cap = PSMALLINT_VALUE(argv[1]);
            }
            printf("+sem_signal %x\r\n", oo);
            vosSemSignalCap(oo->sys.sem, cap);
            printf("-sem_signal %x\r\n", oo);
        }
        break;
        default:
            return ERR_UNSUPPORTED_EXC;
    }
    return ERR_OK;

}

