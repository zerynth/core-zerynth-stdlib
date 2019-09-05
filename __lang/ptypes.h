#ifndef __PTYPES__
#define __PTYPES__

#include "opcodes.h"

#define PSMALLINT   0
#define PINTEGER    1
#define PFLOAT      2
#define PNUMBER     2
#define PBOOL       3
#define PSTRING     4
#define PBYTES      5
#define PBYTEARRAY  6
#define PSHORTS     7
#define PSHORTARRAY 8
#define PLIST       9
#define PTUPLE      10
#define PRANGE      11
#define PSEQUENCE   11
#define PFSET       12
#define PSET        13
#define PDICT       14
#define PFUNCTION   15
#define PMETHOD     16
#define PCLASS      17
#define PINSTANCE   18
#define PMODULE     19
#define PBUFFER     20
#define PSLICE      21
#define PITERATOR   22
#define PFRAME      23
#define PCELL       24
#define PNONE       25
#define PEXCEPTION  26
#define PNATIVE     27
#define PSYSOBJ     28
#define PDRIVER     29
#define PTHREAD     30

#define NTYPES  31


extern const char *const typestrings[];

#include "pexc.h"

#endif
