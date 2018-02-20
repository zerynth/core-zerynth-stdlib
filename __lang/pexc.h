#ifndef __P_EXC__
#define __P_EXC__

#include "pnames.h"

#define ERR_OK                  0
#define ERR_TYPE_EXC            NAME_TypeError
#define ERR_ZERODIV_EXC         NAME_ZeroDivisionError
#define ERR_ATTRIBUTE_EXC       NAME_AttributeError
#define ERR_RUNTIME_EXC         NAME_RuntimeError
#define ERR_VALUE_EXC           NAME_ValueError
#define ERR_INDEX_EXC           NAME_IndexError
#define ERR_KEY_EXC             NAME_KeyError
#define ERR_NOT_IMPLEMENTED_EXC NAME_NotImplementedError
#define ERR_UNSUPPORTED_EXC     NAME_UnsupportedError
#define ERR_OVERFLOW_EXC        NAME_OverflowError
#define ERR_STOP_ITERATION      NAME_StopIteration
#define ERR_NAME_EXC            NAME_NameError 
#define ERR_IOERROR_EXC         NAME_IOError 
#define ERR_CONNECTION_REF_EXC  NAME_ConnectionRefusedError
#define ERR_CONNECTION_RES_EXC  NAME_ConnectionResetError
#define ERR_CONNECTION_ABR_EXC  NAME_ConnectionAbortedError
#define ERR_TIMEOUT_EXC         NAME_TimeoutError

#define ERR_PERIPHERAL_ERROR_EXC NAME_PeripheralError
#define ERR_PERIPHERAL_INVALID_PIN_EXC NAME_InvalidPinError
#define ERR_PERIPHERAL_INVALID_HARDWARE_STATUS_EXC NAME_InvalidHardwareStatusError
#define ERR_HARDWARE_INITIALIZATION_ERROR NAME_HardwareInitializationError

#define MAKE_ERR_FROM_EXC_NAME(n) ((n))

#endif