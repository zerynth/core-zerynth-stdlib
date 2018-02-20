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
                x = (double)INTEGER_VALUE(o);       \
                break;                              \
            case PFLOAT:                            \
                x = (double)FLOAT_VALUE(o);         \
                break;                              \
            default:                                \
                return ERR_TYPE_EXC;                \
        }                                           \
    } while (0)


/////////////////////TRIGONOMETRY
#define printf(...) vbl_printf_stdout(__VA_ARGS__)

C_NATIVE(__cos) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)cos(x));
    return ERR_OK;
}

C_NATIVE(__sin) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)sin(x));
    return ERR_OK;
}

C_NATIVE(__tan) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)tan(x));
    return ERR_OK;
}

C_NATIVE(__acos) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)acos(x));
    return ERR_OK;
}

C_NATIVE(__asin) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)asin(x));
    return ERR_OK;
}

C_NATIVE(__atan) {
    NATIVE_UNWARN();
    double x;
    GET_ARG_x(0,x);
    *res = (PObject*)pfloat_new((float)atan(x));
    return ERR_OK;
}

C_NATIVE(__atan2) {
    NATIVE_UNWARN();
    double x,y;
    GET_ARG_x(1,x);
    GET_ARG_x(0,y);
    *res = (PObject*)pfloat_new((float)atan2(y,x));
    return ERR_OK;
}



