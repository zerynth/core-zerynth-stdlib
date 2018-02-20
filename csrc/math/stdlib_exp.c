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

C_NATIVE(__expn) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)exp(x));
    return ERR_OK;
}

C_NATIVE(__logn) {
    NATIVE_UNWARN();
    double x,y;
    GET_ARG_x(0,x);
    GET_ARG_x(1,y);

    if (y<=0) {
        *res = (PObject*)pfloat_new((float)log(x)); 
    } else {
        *res = (PObject*)pfloat_new((float)(log(x)/log(y)));
    }
    return ERR_OK;
}

