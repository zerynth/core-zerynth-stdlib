#include "zerynth.h"

#define GC_CMD_INFO     0
#define GC_CMD_COLLECT  1
#define GC_CMD_DISABLE  2
#define GC_CMD_ENABLE   3


C_NATIVE(__gc) {
    NATIVE_UNWARN();
    CHECK_ARG(args[0], PSMALLINT);
    int32_t code = PSMALLINT_VALUE(args[0]);

    *res = MAKE_NONE();

    switch (code) {
        case GC_CMD_INFO: {
            //total mem, free mem, fragmentation, blocks, fblocks, gctime interval, ms since last collection 
            PTuple *pt = (PTuple*)psequence_new(PTUPLE,7);
            PTUPLE_SET_ITEM(pt, 0, PSMALLINT_NEW(gc_info(0)));
            PTUPLE_SET_ITEM(pt, 1, PSMALLINT_NEW(gc_info(1)));
            PTUPLE_SET_ITEM(pt, 2, PSMALLINT_NEW(100 - gc_info(2)));
            PTUPLE_SET_ITEM(pt, 3, PSMALLINT_NEW(gc_info(3)));
            PTUPLE_SET_ITEM(pt, 4, PSMALLINT_NEW(gc_info(4)));
            PTUPLE_SET_ITEM(pt, 5, PSMALLINT_NEW(gc_info(5)));
            PTUPLE_SET_ITEM(pt, 6, PSMALLINT_NEW(gc_info(6)));
            *res = (PObject *)pt;
        }
        break;
        case GC_CMD_COLLECT: {
            //gc_collect();
            return ERR_NOT_IMPLEMENTED_EXC;
        }
        break;
        case GC_CMD_DISABLE: {
            gc_pause();
            return ERR_OK;
        }
        break;
        case GC_CMD_ENABLE: {
            
            if (nargs == 2) {
                CHECK_ARG(args[1], PSMALLINT);
                code = PSMALLINT_VALUE(args[1]);
                if (code <= 0)
                    return ERR_VALUE_EXC;
            } else code = 0;
            gc_pause();
            gc_resume(TIME_U(code, MILLIS));
        }
        break;
    }
    return ERR_OK;
}
