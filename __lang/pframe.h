#ifndef __PFRAME__
#define __PFRAME__

#include "pfunction.h"

/*
typedef struct _pblock {
    PObjectHeader header;
    uint16_t    sp;
    uint16_t    jump;
    struct _pblock *next;
} PBlock;
*/

typedef struct _pblock {
    uint8_t    type;
    uint8_t    sp;
    uint16_t    jump;
} PBlock;

typedef struct _pframe {
    PObjectHeader header;
    uint8_t why;
    uint8_t sp;
    uint16_t pc;
    uint16_t module;
    uint16_t code;
    uint8_t sb;
    uint8_t delegate;
    uint16_t temp;
    //PObject *caller;
    struct _pframe *parent;
    /*
        0...stacksize  
        0...blockstacksize
        0...nlocals
        freevars  -|
                  -|-> None or both
        cellvars  -|
        
    */
    PObject *storage[];
} PFrame;


#define PBLOCK_LOOP     0
#define PBLOCK_EXCEPT   1
#define PBLOCK_FINALLY  2
#define PBLOCK_HANDLER  3


PFrame *pframe_new(uint16_t ncode, PFrame *parent);

//bit 6 for activated frame
#define PFRAME_ACTIVATE(f) (((PObject*)(f))->header.flags|=0x40)
#define PFRAME_DEACTIVATE(f) (((PObject*)(f))->header.flags&=(~0x40))
#define PFRAME_IS_ACTIVE(f) (((PObject*)(f))->header.flags&(0x40))

//bit 7 for ret/no ret
#define PFRAME_NO_RET(f) (((PObject*)(f))->header.flags|=0x80)
#define PFRAME_IS_NO_RET(f) (((PObject*)(f))->header.flags&(0x80))

// #define PFRAME_HAS_DELEGATE(f) (((PObject*)(f))->header.flags&(0x7f))
// #define PFRAME_DELEGATE(f,op) (((PObject*)(f))->header.flags=(((PObject*)(f))->header.flags&0x80)|(op))
// #define PFRAME_UNDELEGATE(f) (((PObject*)(f))->header.flags&=0x80)
// #define PFRAME_DELEGATED_OP(f) (((PObject*)(f))->header.flags&(0x7f))

/*
#define PFRAME_FREEVARS(f) ((PTuple*)(*((f)->storage+PCODE_MAKE((f)->code)->nlocals+PCODE_MAKE((f)->code)->stacksize)))
#define PFRAME_CELLVARS(f) ((PTuple*)(*((f)->storage+PCODE_MAKE((f)->code)->nlocals+PCODE_MAKE((f)->code)->stacksize+1)))
#define PFRAME_SET_FREEVARS(f,fvs) (*((f)->storage+PCODE_MAKE((f)->code)->nlocals+PCODE_MAKE((f)->code)->stacksize))=(PObject*)fvs
#define PFRAME_SET_CELLVARS(f,fvs) (*((f)->storage+PCODE_MAKE((f)->code)->nlocals+PCODE_MAKE((f)->code)->stacksize+1))=(PObject*)fvs
#define PFRAME_GET_PLOCALS(frm,code) (frm)->storage+(code)->stacksize
#define PFRAME_GET_PFREEVARS(frm,code) (PCODE_FREEVARS(code)) ? PTUPLE_OBJECTS(PFRAME_FREEVARS(frm)) : NULL
#define PFRAME_GET_PCELLVARS(frm,code) (PCODE_CELLVARS(code)) ? PTUPLE_OBJECTS(PFRAME_CELLVARS(frm)) : NULL
#define PFRAME_GET_PBLOCKS(frm,code) (PBlock *)((frm)->storage+(code)->stacksize+ (code)->nlocals - 1 + ((PCODE_FREEVARS(code)) ? 1 : 0) + ((PCODE_CELLVARS(code)) ? 1 : 0));
*/

#define PFRAME_PSTACK(frm) ((frm)->storage)
#define PFRAME_PBLOCKS(frm,code) (PBlock*)((frm)->storage+PCODE_STACKSIZE(code)-1)
#define PFRAME_PLOCALS(frm,code) ((frm)->storage+PCODE_STACKSIZE(code)+PCODE_BSTACKSIZE(code))
#define PFRAME_TFREEVARS(frm,code) (PTuple*)(*((frm)->storage+PCODE_STACKSIZE(code)+PCODE_BSTACKSIZE(code)+PCODE_NLOCALS(code)))
#define PFRAME_TCELLVARS(frm,code) (PTuple*)(*((frm)->storage+PCODE_STACKSIZE(code)+PCODE_BSTACKSIZE(code)+PCODE_NLOCALS(code)+1))
#define PFRAME_TFREEVARS_FROM_LOCALS(lcls,code) (PTuple*)(*((lcls)+PCODE_NLOCALS(code)))

#define PFRAME_TFREEVARS_SAFE(frm,code) ((PCODE_FREEVARS(code)) ? PFRAME_TFREEVARS(frm,code):NULL)
#define PFRAME_TCELLVARS_SAFE(frm,code) ((PCODE_CELLVARS(code)) ? PFRAME_TCELLVARS(frm,code):NULL)

#define PFRAME_SET_FREEVARS(f,code,fvs) (f)->storage[PCODE_STACKSIZE(code)+PCODE_BSTACKSIZE(code)+PCODE_NLOCALS(code)]=(PObject*)(fvs)
#define PFRAME_SET_CELLVARS(f,code,fvs) (f)->storage[PCODE_STACKSIZE(code)+PCODE_BSTACKSIZE(code)+PCODE_NLOCALS(code)+1]=(PObject*)(fvs)

#define PFRAME_VARS(frm,code) (((frm)->storage+PCODE_STACKSIZE(code)+PCODE_BSTACKSIZE(code)+PCODE_NLOCALS(code)))
#endif
