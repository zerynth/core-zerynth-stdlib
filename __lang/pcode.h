#ifndef __PCODE__
#define __PCODE__


#include "pobj.h"
#include "builtins.h"

#define CODETYPE_VIPER      0
#define CODESUBTYPE_VIPER_FUN       0
#define CODESUBTYPE_VIPER_MODULE    1
#define CODESUBTYPE_VIPER_CLASS     2

#define CODETYPE_CNATIVE    1



#ifndef BYTECODE_ACCESS_ALIGNED_4
    typedef struct _code {
        uint8_t codetype;
        uint8_t args;
        uint8_t kwargs;
        uint8_t vararg;

        uint8_t nlocals;
        uint8_t nfree;
        uint8_t stacksize;
        uint8_t bstacksize;

        uint16_t name;
    } PCode;


    typedef struct _vipercode {
        uint8_t codetype;
        uint8_t args;
        uint8_t kwargs;
        uint8_t vararg;

        uint8_t nlocals;
        uint8_t nfree;
        uint8_t stacksize;
        uint8_t bstacksize;

        uint16_t name;
        uint16_t nconsts;

        uint16_t nmstart;
        uint16_t bcstart;
        uint8_t bytecode[];
    } PViperCode;


    typedef struct _cnativecode {
        uint8_t codetype;
        uint8_t args;
        uint8_t kwargs;
        uint8_t vararg;

        uint8_t nlocals;
        uint8_t nfree;
        uint8_t stacksize;
        uint8_t bstacksize;

        uint16_t name;

        uint16_t tableidx;
        uint8_t minitable[];
    } PCNativeCode;

    #define PCODE_CODETYPE(c) (c)->codetype
    #define PCODE_ARGS(c) (c)->args
    #define PCODE_KWARGS(c) (c)->kwargs
    #define PCODE_VARARG(c) (c)->vararg
    #define PCODE_NLOCALS(c) (c)->nlocals
    #define PCODE_NFREE(c) (c)->nfree
    #define PCODE_STACKSIZE(c) (c)->stacksize
    #define PCODE_BSTACKSIZE(c) (c)->bstacksize
    #define PCODE_NAME(c) (c)->name
    #define PCODE_TABLEIDX(c) (c)->tableidx
    #define PCODE_MINITABLE(c) (c)->minitable
    #define PCODE_NCONSTS(c) (c)->nconsts
    #define PCODE_NMSTART(c) (c)->nmstart
    #define PCODE_BCSTART(c) (c)->bcstart
    #define PCODE_BYTECODE(c) (c)->bytecode
    #define PCODE_CONST_HEADER(cheader,pos) ((cheader)[pos])
    #define PCODE_GET_LEN(cnst) (*((uint16_t *)cnst))

#else
    typedef struct _code {
        uint8_t codetype;
        uint8_t args;
        uint8_t kwargs;
        uint8_t vararg;

        uint8_t nlocals;
        uint8_t nfree;
        uint8_t stacksize;
        uint8_t bstacksize;

        uint16_t name;
    } PCode;


    typedef struct _vipercode {
        uint8_t codetype;
        uint8_t args;
        uint8_t kwargs;
        uint8_t vararg;

        uint8_t nlocals;
        uint8_t nfree;
        uint8_t stacksize;
        uint8_t bstacksize;

        uint16_t name;
        uint16_t nconsts;

        uint16_t nmstart;
        uint16_t bcstart;
        //uint8_t bytecode[];
    } PViperCode;


    typedef struct _cnativecode {
        uint8_t codetype;
        uint8_t args;
        uint8_t kwargs;
        uint8_t vararg;

        uint8_t nlocals;
        uint8_t nfree;
        uint8_t stacksize;
        uint8_t bstacksize;

        uint16_t name;

        uint16_t tableidx;
        //uint8_t minitable[];
    } PCNativeCode;


    #define PCODE_GET_WORD_AT(pnt,pos)  __access_word(pnt,pos)
    //( (__readby4(pnt,pos))|(__readby4(pnt,(pos)+1)<<8))
    //(((*(((uint32_t*)(pnt))+((pos)/2)))>>(((pos)%2)*16))&0xffff)
    #define PCODE_GET_BYTE_AT(pnt,pos)  __access_byte(pnt,pos)
    //((uint32_t)(((*(((uint32_t*)(pnt))+((pos)/4)))>>(((pos)%4)*8))&0x000000ff))
    #define PCODE_CODETYPE(c) PCODE_GET_BYTE_AT(c,0)
    #define PCODE_ARGS(c) PCODE_GET_BYTE_AT(c,1)
    #define PCODE_KWARGS(c) PCODE_GET_BYTE_AT(c,2)
    #define PCODE_VARARG(c) PCODE_GET_BYTE_AT(c,3)
    #define PCODE_NLOCALS(c)  PCODE_GET_BYTE_AT(c,4)
    //(((*( ((uint32_t*)(c))+1 )))&0xff)
    #define PCODE_NFREE(c) PCODE_GET_BYTE_AT(c,5)
    #define PCODE_STACKSIZE(c) PCODE_GET_BYTE_AT(c,6)
    #define PCODE_BSTACKSIZE(c) PCODE_GET_BYTE_AT(c,7)
    #define PCODE_NAME(c) PCODE_GET_WORD_AT(c,8)
    #define PCODE_TABLEIDX(c) PCODE_GET_WORD_AT(c,10)
    #define PCODE_MINITABLE(c) ((uint8_t*)(((uint8_t*)(c))+12))
    #define PCODE_NCONSTS(c) PCODE_GET_WORD_AT(c,10)
    #define PCODE_NMSTART(c) PCODE_GET_WORD_AT(c,12)
    #define PCODE_BCSTART(c) PCODE_GET_WORD_AT(c,14)
    #define PCODE_BYTECODE(c) ((uint8_t*)(((uint32_t*)(c))+4))
    #define PCODE_CONST_HEADER(cheader,pos) PCODE_GET_WORD_AT(cheader,(pos)*2)
    #define PCODE_GET_LEN(cnst) PCODE_GET_WORD_AT(cnst,0)
    

#endif

typedef struct _code_header {
    const uint32_t magic;
    const uint8_t flags;
    const uint8_t nmodules;
    const uint16_t nobjs;
    const uint16_t nexcp;
    const uint16_t ncnatives;
    const uint32_t ram_start;
    const uint32_t data_start;
    const uint32_t data_end;
    const uint32_t data_bss;
    const uint32_t res_table;
#ifndef BYTECODE_ACCESS_ALIGNED_4
    const uint32_t const codeobjs[];
#else
    uint32_t *codeobjs;
#endif
} CodeHeader;


#define PCODE_MAKE(n) ((PCode*)(((uint8_t*)codemem)+_vm.program->codeobjs[(n)]))
#define ViperCode(code) ((PViperCode*)code)
#define CNativeCode(code) ((PCNativeCode*)code)

#define PCODE_CELLVARS(code) (PCODE_NFREE(code)&0x0f)
#define PCODE_FREEVARS(code) (PCODE_NFREE(code)>>4)
#define PCODE_HASVARS(code) PCODE_NFREE(code)

#define PCODE_GET_BYTECODE(code) PCODE_BYTECODE(ViperCode(code))+ PCODE_BCSTART(ViperCode(code))
#endif

