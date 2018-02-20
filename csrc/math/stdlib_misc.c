#include "zerynth.h"

#include "include/math.h"

//TODO: allow integers
#define GET_ARG_x(n,x) do { \
        PObject *o = args[n];                       \
        switch(PTYPE(o)){                           \
            case PSMALLINT:                         \
                x = (double)PSMALLINT_VALUE(o);     \
                break;                              \
            case PINTEGER:                          \
                x = (double)INTEGER_VALUE(o);      \
                break;                              \
            case PFLOAT:                            \
                x = (double)FLOAT_VALUE(o);         \
                break;                              \
            default:                                \
                return ERR_TYPE_EXC;                \
        }                                           \
    } while (0)


/////////////////////EXP

C_NATIVE(__isnan) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    if (isnan(x)) *res = PBOOL_TRUE();
    else *res = PBOOL_FALSE();
    return ERR_OK;
}

C_NATIVE(__isinf) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    if (!finite(x)) *res = PBOOL_TRUE();
    else *res = PBOOL_FALSE();
    return ERR_OK;
}

C_NATIVE(__floor) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)floor(x));
    return ERR_OK;
}

C_NATIVE(__ceil) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)ceil(x));
    return ERR_OK;
}

