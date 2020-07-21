# Zerynth Sockets

Zerynth sockets are an abstraction layer for BSD compliant sockets that allows connectivity drivers to be developed
uniformly. The goal of a connectivity driver is to provide two types of functions:

- Configuration functions for the specific driver (i.e. wifi station link/unlink, retrieve rssi, etc...)
- Socket API functions

## Socket API

The Socket API functions must implement the following BSD compliant functions:

## Socket API pointers

The core of Zerynth Sockets is a structure containing the pointers to the above API. Having these pointers, the Zerynth Socket layer can call into the connectivity driver without knowing anything about it. The connectibity driver must always initialize the Zerynth Socket layer by calling ```gzsock_init()``` and passing the pointers.


## zsock functions

A set of macro exists, defined in ```zerynth_sockets.h```, that allows to call the Socket API pointers by prefixing the original functions with ```zsock_```. For example, to call the connect function of the connectivity driver, ```zsock_connect``` must be called.

## zssl functions

For secure sockets, there exists a specific layer, where each function starts with ```zssl_```. To connect a secure socket, one can call ```zssl_connect``` that abstracts away all the details of the secure connection.

## gzsock functions

The main functions in the Zerynth Socket layer starts with ```gzsock_```. They are implemented in the stdlib and have the same semantic of the socket API. However they also check if the socket is a secure socket or not. In case of a secure socket, the ```zssl_``` functions are called instead of the ```zsock_``` ones.

## Python socket interface

A layer of C functions callable from Python is implemented in the ```zsocket_pynative.c``` file. Such functions call the ```gzsock_``` ones but are also callable from Python, giving and entry point into the Zerynth Sockets. Connectivity drivers, once the function pointers are defined, can simply include the Python socket interface and avoid reimplementing it.

## Zerynth Socket Scenarios

Zerynth sockets are quite flexible. The main components of the architecture are:

- a low level connectivity driver giving access to the physical medium
- a tcp/ip stack (usually lwIP)
- a secure socket stack (usually mbedtls)
- a crypto element (usually ateccx08)

Each one of the above components can reside either in the bytecode or in the VM giving rise to different scenarios.
The simpler one is when everything is in the bytecode. In this case, the low level driver, the tcp stack, mbedtls and the crypto library are compiled on the development machine and linked together. The opposite case is the esp32 where the low level driver, the tcp stack and mbedtls is in the VM while the crypto library is compiled in the bytecode.

In order to manage different scenarios, Zerynth Sockets can be configured by a set of macros.

### Main Macros

- **ZERYNTH_SOCKETS**: if set, enables the compilation of the Zerynth Socket layer
- **ZERYNTH_SOCKETS_EXTERNAL_TCP_STACK**: if set, the layer expects the TCP stack to be already compiled in the VM (i.e. esp32) or to be provided by the connectivity driver (i.e. stm.f4eth). In both cases, the low level socket functions are defined by include files that must be present (usually in the VHAL or in the connectivity driver). If the macro is not set, it means that the connectivity driver does not provide a standard TCP stack and low level socket functions must be implemented by the driver. In this case, function definitions and constants for the TCP stack are provided by the Zerynth Sockets by including the ```zlwip.h``` header
- **ZERYNTH_SOCKETS_EXTERNAL_API**: the structure holding the socket API pointers can be in the VM or in the firmware. By default is in the firmware, however by setting this macro, it is assumed to be in the VM
- **ZERYNTH_SSL**: this macro enables SSL support. The Zerynth Sockets will provide zssl_ functions and the gzsock_ functions will be able to use secure sockets. If not specified, when ZERYNTH_SSL is enabled, the mbedtls will be used.
- **ZERYNTH_SSL_EXTERNAL_STACK**: if SSL is enabled, the ssl stack (mbedtls) can reside in the firmware or in the VM. By default it resides in the firmware, if this macro is enabled, it is expected to be in the VM
- **ZERYNTH_SSL_MAX_SOCKS**: if defined, sets the maximum number of SSL sockets that can be opened. By default is 2.
- **ZERYNTH_SOCKETS_PYNATIVE**: if defined, the Python native functions for Zerynth Sockets are enabled and compiled.
- **ZERYNTH_SOCKETS_PYNATIVE_CUSTOM_RESOLVE**: if defined, the native function ```py_net_resolve``` is not compiled and must be provided by the driver.

### MbedTLS Macros

- **ZERYNTH_SSL_CUSTOM_CONFIG_INCLUDE**: if set, the programmer can fully customized MbedTLS by providing a ```zerynth_mbedtls_custom_config.h``` header files that is included in the config header of MbedTLS. This macro works if ZERYNTH_SSL_EXTERNAL_STACK is not defined.
- **ZERYNTH_SSL_PROFILE_RSA_MIN_BITS**: by default is 2048 and represents the minimum number of bits for RSA based algorithm. Connections with credentials with a lower number of bits can't be established.
- **ZERYNTH_SSL_MAX_CONTENT_LEN**: by default 8192. It represents the buffers allocated by MbedTLS for storing a communication fragment in each direction (rx/tx). The maximum defined by the protocol is 16384.
- **ZERYNTH_SSL_ALLOW_SHA1_IN_CERTIFICATES**: if enabled allows the usage of sha1 certificates. Disabled by default.
- **ZERYNTH_SSL_DEBUG**: by default is unset. It must be set to an integer from 0 to 4 included. It will enable the MbedTLS debug log with that level of detail.


## Secure Crypto Element

The support for secure crypto element is similar in design to the Zerynth Sockets and they work together. To enable the support, the files under csrc/hwcrypto must be included.

### HWCrypto Macros

- **ZERYNTH_HWCRYPTO_EXTERNAL**: like ZERYNTH_SOCKETS_EXTERNAL_API, it requires the HWCrypto API holding structure to be defined in the VM. By default it is unset


