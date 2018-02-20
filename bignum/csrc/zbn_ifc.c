#include "zerynth.h"
#include "tommath.h"
//#define printf(...) vbl_printf_stdout(__VA_ARGS__)
#define printf(...)
char mp_s_rmap[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz+/";


int     KARATSUBA_MUL_CUTOFF = 80,      /* Min. number of digits before Karatsuba multiplication is used. */
        KARATSUBA_SQR_CUTOFF = 120,     /* Min. number of digits before Karatsuba squaring is used. */

        TOOM_MUL_CUTOFF      = 350,      /* no optimal values of these are known yet so set em high */
        TOOM_SQR_CUTOFF      = 400;


//wrapper for mp_int: enable garbage collection compatibilty
// typedef struct _zbn_wrapper {
//     mp_int a;
//     PBytes *abuf; 
// } ZBN;


void *zbn_malloc(size_t n){
    return NULL;//gc_malloc(n);
}

void *zbn_calloc(size_t n, size_t m) {
    return NULL;//gc_malloc(n*m);
}


void *zbn_realloc(void *p, size_t n){
    if(!p) return NULL;
    mp_int *zbn = (mp_int*)p;
    int bsize = PSEQUENCE_ELEMENTS(zbn->abuf);
    int tocopy = MIN(bsize,n);
    PBytes *abuf = pbytes_new(n,NULL);
    memcpy(_PIS_BYTES(abuf),_PIS_BYTES(zbn->abuf),tocopy);
    zbn->abuf = abuf;
    return _PIS_BYTES(abuf);
}   

void *zbn_zmalloc(void *p, size_t n){
    //hack to allocate a ZBN structure
    //lib passes mp_int a to p
    //if we cast it to ZBN we can allocate abuf
    mp_int *zbn = (mp_int*)p;
    zbn->abuf = pbytes_new(n,NULL);
    return _PIS_BYTES(zbn->abuf);
}


void zbn_free(void *p){
    // gc_free(p);
}

PObject *zbn_to_res(PTuple *n, mp_int *zbn){
    PTuple *tpl;
    PBytes *zbh;
    if(n) {
        //a PObject with zbn inside already exists, update!
        tpl = n;
        zbh = PTUPLE_ITEM(tpl,0);
    } else {
        //a PObject with zbn inside must be created
        tpl = ptuple_new(2,NULL);
        zbh = pbytes_new(sizeof(mp_int),zbn);
        PTUPLE_SET_ITEM(tpl,0,zbh);
    }
    //second element is abuf: set because it may have been shrinked or grown
    PTUPLE_SET_ITEM(tpl,1,zbn->abuf);
    return tpl;
}

C_NATIVE(_zbn_new){
    NATIVE_UNWARN();
    PObject *obj = args[0];
    mp_int zbn;

    int tt = PTYPE(obj);
    if (tt==PSMALLINT){
        mp_init_set_int(&zbn,PSMALLINT_VALUE(obj));
    } else if (tt==PINTEGER){
        //
    } else if (tt==PSTRING){
        uint8_t *ss = PSEQUENCE_BYTES(obj);
        uint32_t sl = PSEQUENCE_ELEMENTS(obj);
        mp_init(&zbn);
        if (sl>=3 && ss[0]=='0' && ss[1]=='x'){
            //hexadecimal
            //printf("new hexadecimal number %i %x\n",sl,mp_s_rmap);
            if (mp_read_radix_n(&zbn,ss+2,16,sl-2)!=MP_OKAY) return ERR_VALUE_EXC;
        } else {
            //integer
            if (mp_read_radix_n(&zbn,ss,10,sl)!=MP_OKAY) return ERR_VALUE_EXC;
        }

    } else if (tt==PBYTES || tt==PBYTEARRAY){
        //
    } else {
        return ERR_VALUE_EXC; 
    }
    
    *res = zbn_to_res(NULL,&zbn);        
    return ERR_OK;
}

#define GET_ZBN(o) ((mp_int*)_PIS_BYTES((PTUPLE_ITEM((o),0))))
#define DIGIT_SIZE 2
C_NATIVE(_zbn_tobase){
    NATIVE_UNWARN();
    PObject *num = args[0];
    CHECK_ARG(args[1],PSMALLINT);
    int base = PSMALLINT_VALUE(args[1]);
    int toalloc = 0;
    int i;
    if(base<2||base>64) return ERR_VALUE_EXC;
    mp_int *zbn = GET_ZBN(num);
    //determine conversion buffer size
    toalloc = 8*zbn->used*DIGIT_SIZE;
    toalloc+=4;
    RELEASE_GIL();
    uint8_t *conv = gc_malloc(toalloc); //account for sign and \0
    printf("used %i toalloc %i\n",zbn->used,toalloc);
    mp_toradix_n(zbn,conv,base,toalloc);
    for(i=0;i<toalloc;i++){
        if(!conv[i]) {
            toalloc = i;
            break;
        }
    }
    // printf("length %i\n",toalloc);
    *res = pstring_new(toalloc,conv);
    gc_free(conv);
    ACQUIRE_GIL();
    
    return ERR_OK;    
}


C_NATIVE(_zbn_add){
    NATIVE_UNWARN();
    PObject *pa = args[0];
    PObject *pb = args[1];
    mp_int *a = GET_ZBN(pa);
    mp_int *b = GET_ZBN(pb);
    mp_int c;

    RELEASE_GIL();
    mp_init(&c);
    mp_add(a,b,&c);
    ACQUIRE_GIL();
    
    *res = zbn_to_res(NULL,&c);
    return ERR_OK;
}

C_NATIVE(_zbn_sub){
    NATIVE_UNWARN();
    PObject *pa = args[0];
    PObject *pb = args[1];
    mp_int *a = GET_ZBN(pa);
    mp_int *b = GET_ZBN(pb);
    mp_int c;

    RELEASE_GIL();
    mp_init(&c);
    mp_sub(a,b,&c);
    ACQUIRE_GIL();
    
    *res = zbn_to_res(NULL,&c);
    return ERR_OK;
}

C_NATIVE(_zbn_mul){
    NATIVE_UNWARN();
    PObject *pa = args[0];
    PObject *pb = args[1];
    mp_int *a = GET_ZBN(pa);
    mp_int *b = GET_ZBN(pb);
    mp_int c;

    RELEASE_GIL();
    mp_init(&c);
    mp_mul(a,b,&c);
    ACQUIRE_GIL();
    
    *res = zbn_to_res(NULL,&c);
    return ERR_OK;
}

C_NATIVE(_zbn_div){
    NATIVE_UNWARN();
    PObject *pa = args[0];
    PObject *pb = args[1];
    mp_int *a = GET_ZBN(pa);
    mp_int *b = GET_ZBN(pb);
    mp_int c;
    mp_int d;
    int err;

    RELEASE_GIL();
    mp_init(&c);
    mp_init(&d);
    err = mp_div(a,b,&c,&d);
    if (err==MP_VAL) err = ERR_ZERODIV_EXC;
    else err=ERR_OK;
    ACQUIRE_GIL();
   
    if(err!=ERR_OK) return err; 
    PTuple *tpl = ptuple_new(2,NULL);
    PObject *q = zbn_to_res(NULL,&c); 
    PObject *r = zbn_to_res(NULL,&d);
    PTUPLE_SET_ITEM(tpl,0,q);
    PTUPLE_SET_ITEM(tpl,1,r);
    *res = tpl;
    return err;
}

C_NATIVE(_zbn_cmp){
    NATIVE_UNWARN();
    PObject *pa = args[0];
    PObject *pb = args[1];
    mp_int *a = GET_ZBN(pa);
    mp_int *b = GET_ZBN(pb);
    int r;

    RELEASE_GIL();
    r = mp_cmp(a,b);
    ACQUIRE_GIL();
    
    *res = PSMALLINT_NEW(r);
    return ERR_OK;
}


