#ifndef ZERYNTH_SOCKETS_H
#define ZERYNTH_SOCKETS_H

#if defined(ZERYNTH_SOCKETS)

#define SOMAXCONN 2 // max number of waiting connections for listen

#if defined(ZERYNTH_EXTERNAL_LWIP)

//LWIP is provided by the driver itself
#include "lwip/sockets.h"
#include "lwip/netdb.h"

#else

//LWIP is not provided or must be provided by stdlib


/* If your port already typedef's in_addr_t, define IN_ADDR_T_DEFINED
   to prevent this code from redefining it. */
// #if !defined(in_addr_t) && !defined(IN_ADDR_T_DEFINED)
typedef uint32_t in_addr_t;
// #endif

#if !defined(sa_family_t) && !defined(SA_FAMILY_T_DEFINED)
typedef uint8_t sa_family_t;
#endif
/* If your port already typedef's in_port_t, define IN_PORT_T_DEFINED
   to prevent this code from redefining it. */
#if !defined(in_port_t) && !defined(IN_PORT_T_DEFINED)
typedef uint16_t in_port_t;
#endif

typedef uint32_t socklen_t;


struct in_addr {
  in_addr_t s_addr;
};

/* members are in network byte order */
struct sockaddr_in {
  uint8_t            sin_len;
  sa_family_t     sin_family;
  in_port_t       sin_port;
  struct in_addr  sin_addr;
#define SIN_ZERO_LEN 8
  char            sin_zero[SIN_ZERO_LEN];
};

struct sockaddr {
  uint8_t        sa_len;
  sa_family_t sa_family;
#if defined(ZERYNTH_SOCKETS_IPV6)
  char        sa_data[22];
#else /* LWIP_IPV6 */
  char        sa_data[14];
#endif /* LWIP_IPV6 */
};

struct addrinfo {
    int               ai_flags;      /* Input flags. */
    int               ai_family;     /* Address family of socket. */
    int               ai_socktype;   /* Socket type. */
    int               ai_protocol;   /* Protocol of socket. */
    socklen_t         ai_addrlen;    /* Length of socket address. */
    struct sockaddr  *ai_addr;       /* Socket address of socket. */
    char             *ai_canonname;  /* Canonical name of service location. */
    struct addrinfo  *ai_next;       /* Pointer to next in list. */
};

struct timeval {
  long    tv_sec;         /* seconds */
  long    tv_usec;        /* and microseconds */
};

// TODO: LWIP has 0xfff instead of BSD 0xffff!!!
#define SOL_SOCKET  0xfff      /* options for socket level */

#endif //ZERYNTH_EXTERNAL_LWIP


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

extern SocketAPIPointers *socket_api_pointers;

SocketAPIPointers *gzsock_init(SocketAPIPointers *pointers);

int gzsock_socket(int domain, int type, int protocol, void *info); //info points to SSLInfo
int gzsock_close(int s);
int gzsock_connect(int s, const struct sockaddr *name, socklen_t namelen);
int gzsock_recv(int s, void *mem, size_t len, int flags);
int gzsock_read(int s, void *mem, size_t len);
int gzsock_send(int s, const void *dataptr, size_t size, int flags);
int gzsock_write(int s, const void *dataptr, size_t size);
int gzsock_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen);


#define zsock_accept(s,addr,addrlen)                    (socket_api_pointers->accept)(s,addr,addrlen)
#define gzsock_accept(s,addr,addrlen)                    (socket_api_pointers->accept)(s,addr,addrlen)
#define zsock_bind(s,name,namelen)                      (socket_api_pointers->bind)(s,name,namelen)
#define gzsock_bind(s,name,namelen)                      (socket_api_pointers->bind)(s,name,namelen)
#define zsock_shutdown(s,how)                           (socket_api_pointers->shutdown)(s,how)
#define gzsock_shutdown(s,how)                           (socket_api_pointers->shutdown)(s,how)
#define zsock_getpeername(s,name,namelen)               (socket_api_pointers->getpeername)(s,name,namelen)
#define zsock_getsockname(s,name,namelen)               (socket_api_pointers->getsockname)(s,name,namelen)
#define zsock_setsockopt(s,level,optname,opval,optlen)  (socket_api_pointers->setsockopt)(s,level,optname,opval,optlen)
#define zsock_getsockopt(s,level,optname,opval,optlen)  (socket_api_pointers->getsockopt)(s,level,optname,opval,optlen)
#define gzsock_getsockopt(s,level,optname,opval,optlen)  (socket_api_pointers->getsockopt)(s,level,optname,opval,optlen)
#define zsock_close(s)                                  (socket_api_pointers->close)(s)
#define zsock_connect(s,name,namelen)                   (socket_api_pointers->connect)(s,name,namelen)
#define zsock_listen(s,backlog)                         (socket_api_pointers->listen)(s,backlog)
#define gzsock_listen(s,backlog)                         (socket_api_pointers->listen)(s,backlog)
#define zsock_recv(s,mem,len,flags)                     (socket_api_pointers->recv)(s,mem,len,flags)
#define zsock_read(s,mem,len)                           (socket_api_pointers->read)(s,mem,len)
#define zsock_recvfrom(s,mem,len,flags,from,fromlen)    (socket_api_pointers->recvfrom)(s,mem,len,flags,from,fromlen)
#define gzsock_recvfrom(s,mem,len,flags,from,fromlen)    (socket_api_pointers->recvfrom)(s,mem,len,flags,from,fromlen)
#define zsock_send(s,dataptr,size,flags)                (socket_api_pointers->send)(s,dataptr,size,flags)
#define zsock_write(s,dataptr,size)                     (socket_api_pointers->write)(s,dataptr,size)
#define zsock_sendto(s,dataptr,size,flags,to,tolen)     (socket_api_pointers->sendto)(s,dataptr,size,flags,to,tolen)
#define gzsock_sendto(s,dataptr,size,flags,to,tolen)     (socket_api_pointers->sendto)(s,dataptr,size,flags,to,tolen)
#define zsock_socket(domain,type,protocol)              (socket_api_pointers->socket)(domain,type,protocol)
#define zsock_select(a,b,c,d,e)                         (socket_api_pointers->select)(a,b,c,d,e)
#define gzsock_select(a,b,c,d,e)                         (socket_api_pointers->select)(a,b,c,d,e)
#define zsock_ioctl(a,b,c)                              (socket_api_pointers->ioctl)(a,b,c)
#define gzsock_ioctl(a,b,c)                              (socket_api_pointers->ioctl)(a,b,c)
#define zsock_fcntl(a,b,c)                              (socket_api_pointers->fcntl)(a,b,c)
#define gzsock_fcntl(a,b,c)                              (socket_api_pointers->fcntl)(a,b,c)
#define zsock_getaddrinfo(a,b,c,d)                      (socket_api_pointers->getaddrinfo)(a,b,c,d)
#define zsock_freeaddrinfo(a)                           (socket_api_pointers->freeaddrinfo)(a)

#define zsock_inet_addr(cp)                             (*socket_api_pointers.inet_addr)(cp)
#define zsock_inet_ntoa(in)                             (*socket_api_pointers.inet_ntoa)(in)




#if defined(ZERYNTH_SSL)
//ADD SSL defs as needed
#include "zerynth_ssl.h"
#endif


#endif //ZERYNTH_SOCKETS


#endif
