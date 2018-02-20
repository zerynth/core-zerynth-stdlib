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


/////////////////////POW

C_NATIVE(__pow) {
    NATIVE_UNWARN();
    double x,y;
    GET_ARG_x(0,x);
    GET_ARG_x(1,y);
    *res = (PObject*)pfloat_new((float)pow(x,y));
    return ERR_OK;
}

C_NATIVE(__sqrt) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)sqrt(x)); 
    return ERR_OK;
}


