#ifndef ZERYNTH_SSL_H
#define ZERYNTH_SSL_H

#if defined(ZERYNTH_SOCKETS)

#define _CERT_NONE 1
#define _CERT_OPTIONAL 2
#define _CERT_REQUIRED 4
#define _CLIENT_AUTH 8
#define _SERVER_AUTH 16


typedef struct _sslinfo {
    uint8_t* cacert;
    uint8_t* clicert;
    uint8_t* pvkey;
    uint8_t* hostname;
    uint16_t cacert_len;
    uint16_t clicert_len;
    uint16_t pvkey_len;
    uint16_t hostname_len;
    uint32_t options;
} SSLInfo;


#endif //ZERYNTH_SOCKETS


#endif
