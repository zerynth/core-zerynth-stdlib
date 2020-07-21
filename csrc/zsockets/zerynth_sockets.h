#ifndef ZERYNTH_SOCKETS_H
#define ZERYNTH_SOCKETS_H

#include "stddef.h"
#include "stdint.h"

#if defined(ZERYNTH_SOCKETS)

#ifndef MAX_SOCKETS
#define MAX_SOCKETS 64
#endif

#if defined(ZERYNTH_SOCKETS_EXTERNAL_TCP_STACK)
//LWIP is provided by the driver itself
#include "lwip/sockets.h"
#include "lwip/netdb.h"
#include "lwip/err.h"
#else
//LWIP is not provided or must be provided by stdlib
#ifndef MAX_SOCKETS
#define MAX_SOCKETS 64
#endif
#include  "zlwip.h"
#endif

typedef struct _zsocket_info {
    int8_t idx;        //index of the socket in the list of ssl sockets. negative if not assigned
} ZSocketInfo;

extern ZSocketInfo _zsocket_info[MAX_SOCKETS];

typedef struct _socket_api_pointers
{
    int (*accept)(int s, struct sockaddr *addr, socklen_t *addrlen);
    int (*bind)(int s, const struct sockaddr *name, socklen_t namelen);
    int (*shutdown)(int s, int how);
    int (*getpeername)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*getsockname)(int s, struct sockaddr *name, socklen_t *namelen);
    int (*getsockopt)(int s, int level, int optname, void *optval, socklen_t *optlen);
    int (*setsockopt)(int s, int level, int optname, const void *optval, socklen_t optlen);
    int (*close)(int s);
    int (*connect)(int s, const struct sockaddr *name, socklen_t namelen);
    int (*listen)(int s, int backlog);
    int (*recv)(int s, void *mem, size_t len, int flags);
    int (*read)(int s, void *mem, size_t len);
    int (*recvfrom)(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
    int (*send)(int s, const void *dataptr, size_t size, int flags);
    int (*sendto)(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
    int (*socket)(int domain, int type, int protocol);
    int (*write)(int s, const void *dataptr, size_t size);
    int (*select)(int maxfdp1, void *readset, void *writeset, void *exceptset, struct timeval *timeout);
    int (*ioctl)(int s, long cmd, void *argp);
    int (*fcntl)(int s, int cmd, int val);
    int (*getaddrinfo)(const char *node, const char* service, const struct addrinfo *hints, struct addrinfo **res);
    void (*freeaddrinfo)(struct addrinfo *res);
    uint32_t (*inet_addr)(const char *cp);
    char* (*inet_ntoa)(struct in_addr *in);
} SocketAPIPointers;

//TODO: add more api objects for different interfaces (wifi, eth, gsm,...)
extern SocketAPIPointers *socket_api_pointers;
SocketAPIPointers *gzsock_init(SocketAPIPointers *pointers);

int gzsock_accept(int s, struct sockaddr *addr, socklen_t *addrlen);
int gzsock_bind(int s, const struct sockaddr *name, socklen_t namelen);
int gzsock_shutdown(int s, int how);
int gzsock_getpeername(int s, struct sockaddr *name, socklen_t *namelen);
int gzsock_getsockname(int s, struct sockaddr *name, socklen_t *namelen);
int gzsock_getsockopt(int s, int level, int optname, void *optval, socklen_t *optlen);
int gzsock_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);
int gzsock_close(int s);
int gzsock_connect(int s, const struct sockaddr *name, socklen_t namelen);
int gzsock_listen(int s, int backlog);
int gzsock_recv(int s, void *mem, size_t len, int flags);
int gzsock_read(int s, void *mem, size_t len);
int gzsock_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen);
int gzsock_send(int s, const void *dataptr, size_t size, int flags);
int gzsock_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen);
int gzsock_socket(int domain, int type, int protocol, void *ctx);
int gzsock_write(int s, const void *dataptr, size_t size);
int gzsock_select(int maxfdp1, void *readset, void *writeset, void *exceptset, struct timeval *timeout);
int gzsock_ioctl(int s, long cmd, void *argp);
int gzsock_fcntl(int s, int cmd, int val);
int gzsock_getaddrinfo(const char *node, const char* service, const struct addrinfo *hints, struct addrinfo **res);
void gzsock_freeaddrinfo(struct addrinfo *res);
uint32_t gzsock_inet_addr(const char *cp);
char* gzsock_inet_ntoa(struct in_addr *in);


int zs_addr_to_string(struct sockaddr_in *addr, uint8_t *saddr);
int zs_string_to_addr(uint8_t *saddr, int len, struct sockaddr_in *addr);


//MACROS
#define zsock_accept(s,addr,addrlen)                    (socket_api_pointers->accept)(s,addr,addrlen)
#define zsock_bind(s,name,namelen)                      (socket_api_pointers->bind)(s,name,namelen)
#define zsock_shutdown(s,how)                           (socket_api_pointers->shutdown)(s,how)
#define zsock_getpeername(s,name,namelen)               (socket_api_pointers->getpeername)(s,name,namelen)
#define zsock_getsockname(s,name,namelen)               (socket_api_pointers->getsockname)(s,name,namelen)
#define zsock_setsockopt(s,level,optname,opval,optlen)  (socket_api_pointers->setsockopt)(s,level,optname,opval,optlen)
#define zsock_getsockopt(s,level,optname,opval,optlen)  (socket_api_pointers->getsockopt)(s,level,optname,opval,optlen)
#define zsock_close(s)                                  (socket_api_pointers->close)(s)
#define zsock_connect(s,name,namelen)                   (socket_api_pointers->connect)(s,name,namelen)
#define zsock_listen(s,backlog)                         (socket_api_pointers->listen)(s,backlog)
#define zsock_recv(s,mem,len,flags)                     (socket_api_pointers->recv)(s,mem,len,flags)
#define zsock_read(s,mem,len)                           (socket_api_pointers->read)(s,mem,len)
#define zsock_recvfrom(s,mem,len,flags,from,fromlen)    (socket_api_pointers->recvfrom)(s,mem,len,flags,from,fromlen)
#define zsock_send(s,dataptr,size,flags)                (socket_api_pointers->send)(s,dataptr,size,flags)
#define zsock_write(s,dataptr,size)                     (socket_api_pointers->write)(s,dataptr,size)
#define zsock_sendto(s,dataptr,size,flags,to,tolen)     (socket_api_pointers->sendto)(s,dataptr,size,flags,to,tolen)
#define zsock_socket(domain,type,protocol)              (socket_api_pointers->socket)(domain,type,protocol)
#define zsock_select(a,b,c,d,e)                         (socket_api_pointers->select)(a,b,c,d,e)
#define zsock_ioctl(a,b,c)                              (socket_api_pointers->ioctl)(a,b,c)
#define zsock_fcntl(a,b,c)                              (socket_api_pointers->fcntl)(a,b,c)
#define zsock_getaddrinfo(a,b,c,d)                      (socket_api_pointers->getaddrinfo)(a,b,c,d)
#define zsock_freeaddrinfo(a)                           (socket_api_pointers->freeaddrinfo)(a)

#define zsock_inet_addr(cp)                             (socket_api_pointers->inet_addr)(cp)
#define zsock_inet_ntoa(in)                             (socket_api_pointers->inet_ntoa)(in)




#if defined(ZERYNTH_SSL) || defined(NATIVE_MBEDTLS)
//ADD SSL defs as needed
#include "zerynth_ssl.h"
#endif



#endif //ZERYNTH_SOCKETS


#endif
