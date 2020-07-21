# Debug Log 

During debugging it may be necessary to enable some additional logging to understand what is going on.
C code can be debugged by using the debug module.
Each library that has some C code can enable debugging by:

- defining a tag
- checking if the debug enabling macro is set
- including the ```zerynth_debug_macros.h``` header (ONLY IN C FILES THAT NEED DEBUGGING, NEVER IN OTHER HEADERS)

For example, Zerynth Sockets provide the macro ZERYNTH_SOCKETS_DEBUG that must be set to an integer from 0 to 4 included.
If it is set together with ZERYNTH_DEBUG, the logging is enabled for zerynth sockets.

To do so for a library include the following code in the library C files:

```
#define DEBUG_TAG "MY_LIBRARY_TAG"
#if defined(ZERYNTH_MYLIBRARY_DEBUG)
#define DEBUG_MAX_LEVEL ZERYNTH_MYLIBRARY_DEBUG
#else
#define DEBUG_MAX_LEVEL -1
#endif
#include "zerynth_debug_macros.h"

```

The available macros for debuggin will be:

- DEBUG(lvl,fmt,...): debug message from format ```fmt``` with level ```lvl```
- DEBUG0(fmt,...): as DEBUG with level 0
- DEBUG1(fmt,...): as DEBUG with level 1
- DEBUG2(fmt,...): as DEBUG with level 2
- DEBUG3(fmt,...): as DEBUG with level 3
- ERROR(fmt,...): as DEBUG0, without level, message marked as error

All the macros automatically add a newline character at the end of the message.
