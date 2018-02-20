#ifndef __OPCODES__
#define __OPCODES__


extern const char * const opstrings[];



//OPCODES
#define NOP				0x00
#define STOP			0x01

//constants
#define CONSTI 			0x02
#define CONSTF 			0x03
#define CONSTS 			0x04

#define CONST_NONE 		0x05

#define CONSTI_0 		0x06
#define CONSTI_1 		0x07
#define CONSTI_2 		0x08
#define CONSTI_M1 		0x09

#define CONSTF_0 		0x0A
#define CONSTF_1 		0x0B
#define CONSTF_M1 		0x0C

#define CONSTS_0 		0x0D
#define CONSTS_S 		0x0E
#define CONSTS_N 		0x0F


//VARS & NAMES
#define LOAD_FAST  		0x10
#define LOAD_GLOBAL 	0x11
#define LOAD_DEREF  	0x12
#define LOAD_SUBSCR		0x13
#define LOAD_ATTR		0x14
#define LOOKUP_BUILTIN	0x15


#define STORE_FAST  	0x16
#define STORE_GLOBAL  	0x17
#define STORE_DEREF  	0x18
#define STORE_SUBSCR	0x19
#define STORE_ATTR		0x1A

#define DELETE_FAST  	0x1B
#define DELETE_GLOBAL  	0x1C
#define DELETE_DEREF  	0x1D
#define DELETE_SUBSCR	0x1E
#define DELETE_ATTR		0x1F


#define UPOS			0x20
#define UNEG			0x21
#define NOT				0x22
#define INVERT			0x23

//binary ops
#define ADD             0x24
#define IADD            0x25
#define SUB             0x26
#define ISUB            0x27
#define MUL             0x28
#define IMUL            0x29
#define DIV             0x2A
#define IDIV            0x2B
#define FDIV            0x2C
#define IFDIV           0x2D
#define MOD             0x2E
#define IMOD            0x2F
#define POW             0x30
#define IPOW            0x31
#define LSHIFT          0x32
#define ILSHIFT         0x33
#define RSHIFT          0x34
#define IRSHIFT         0x35
#define BIT_OR          0x36
#define IBIT_OR         0x37
#define BIT_XOR         0x38
#define IBIT_XOR        0x39
#define BIT_AND         0x3A
#define IBIT_AND        0x3B
#define EQ              0x3C
#define NOT_EQ          0x3D
#define LT              0x3E
#define LTE             0x3F
#define GT              0x40
#define GTE             0x41
#define L_AND           0x42
#define L_OR            0x43
#define IS              0x44
#define IS_NOT          0x45
#define IN              0x46
#define IN_NOT          0x47


#define BUILD_LIST		0x48
#define BUILD_TUPLE		0x49
#define BUILD_SLICE		0x4A
#define BUILD_DICT		0x4B
#define BUILD_SET		0x4C
#define BUILD_CLASS		0x4D
#define END_CLASS		0x4E
#define UNPACK			0x4F



//branch && LOOPS
#define JUMP_IF_TRUE	0x50
#define JUMP_IF_FALSE	0x51
#define IF_TRUE			0x52
#define IF_FALSE		0x53
#define JUMP 			0x54
#define SETUP_LOOP		0x55
#define POP_BLOCK 		0x56
#define BREAK 			0x57
#define CONTINUE 		0x58
#define GET_ITER 		0x59
#define FOR_ITER		0x5A

//MISC
#define MAKE_FUNCTION	0x5B
#define MAKE_CLOSURE	0x5C
#define CALL 			0x5D
#define CALL_VAR 		0x5E
#define RET 			0x5F

#define POP				0x60
#define DUP				0x61
#define DUP_TWO			0x62
#define ROT_TWO			0x63
#define ROT_THREE		0x64

#define LOOKUP_CODE		0x65
#define LOOKUP_NAME		0x66
#define LOOKUP_NATIVE    0x67


//EXCEPTIONS
#define RAISE	 		0x68
#define SETUP_FINALLY 	0x69
#define SETUP_EXCEPT	0x6A
#define POP_EXCEPT 		0x6B
#define END_FINALLY 	0x6C
#define BUILD_EXCEPTION 0x6D
#define CHECK_EXCEPTION 0x6E


#define IMPORT_NAME		0x6F
#define IMPORT_BUILTINS	0x70

#define MAP_STORE       0x71
#define LIST_STORE      0x72
#define CONST_TRUE      0x73
#define CONST_FALSE     0x74
#define MAKE_CELL       0x75
#define YIELD           0x76
#define CONSTB          0x77



#define _BIN_OP(a) (a-ADD)
#define _OPSTR(a) opstrings[a]
#include "pnames.h"

#endif