#include "zerynth_sockets.h"
#include "zerynth_ssl.h"
#include "zerynth.h" // last position since some defines might cause conflicts (ERR_OK with lwIP, opcodes)
#include "zerynth_sockets_debug.h"

ZSocketInfo _zsocket_info[MAX_SOCKETS];

#if !defined(ZERYNTH_SOCKETS_EXTERNAL_API)
SocketAPIPointers *socket_api_pointers = NULL;
#endif


SocketAPIPointers *gzsock_init(SocketAPIPointers *pointers) {
    SocketAPIPointers *old = socket_api_pointers;
    if (pointers)
        socket_api_pointers = pointers;
    memset(_zsocket_info,0xff,sizeof(_zsocket_info));
#if defined(ZERYNTH_SSL)
    zssl_init();
#endif
    return old;
}

/*
 * Create a new socket
 *
 * For stacks where ssl is handled by the connectivity driver it calls zsock api
 * else it uses zerynth ssl sockets with ssl context in ctx
 *
 */
int gzsock_socket(int domain, int type, int protocol, void *ctx){
    int32_t i;
    int32_t sslsock_fd=0;
    int32_t err=0;

    DEBUG(LVL0,"args %i %i %i %x",domain,type,protocol,ctx);
#if defined(ZERYNTH_SSL)
    if(ctx) {
        return zssl_socket(domain,type,protocol,ctx);
    } else
#endif
    {
        //call zsock api
        return zsock_socket(domain,type,protocol);
    }
}


int gzsock_connect(int s, const struct sockaddr *name, socklen_t namelen){
    DEBUG(LVL0,"args %i %x %i",s,name,namelen);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_connect(s,name,namelen);
    } else
#endif
    {
        return zsock_connect(s,name,namelen);
    }
}


int gzsock_close(int s){
    DEBUG(LVL0,"args %i",s);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_close(s);
    } else
#endif
    {
        return zsock_close(s);
    }
}
int gzsock_recv(int s, void *mem, size_t len, int flags){
    DEBUG(LVL0,"args %i %x %i %i",s,mem,len,flags);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_recv(s,mem,len,flags);
    } else
#endif
    {
        return zsock_recv(s,mem,len,flags);
    }
}
int gzsock_read(int s, void *mem, size_t len){
    DEBUG(LVL0,"args %i %x %i %i",s,mem,len);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_read(s,mem,len);
    } else
#endif
    {
        return zsock_read(s,mem,len);
    }
}
int gzsock_send(int s, const void *dataptr, size_t size, int flags){
    DEBUG(LVL0,"args %i %x %i %i",s,dataptr,size,flags);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_send(s,dataptr,size,flags);
    } else
#endif
    {
        return zsock_send(s,dataptr,size,flags);
    }

}
int gzsock_write(int s, const void *dataptr, size_t size){
    DEBUG(LVL0,"args %i %x %i",s,dataptr,size);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_write(s,dataptr,size);
        SSLSock *sslsock = GET_SECURE_SOCKET(s);
        return mbedtls_ssl_write(&sslsock->ssl,dataptr,size);
    } else
#endif
    {
        return zsock_write(s,dataptr,size);
    }

}

int gzsock_setsockopt(int s, int level, int optname, const void *optval, socklen_t optlen){
    DEBUG(LVL0,"args %i %i %i %x %i",s,level,optname,optval,optlen);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(s)) {
        return zssl_setsockopt(s,level,optname,optval,optlen);
    } else
#endif
    {
        return zsock_setsockopt(s, level, optname, optval, optlen);
    }
}

int gzsock_sendto(int s, const void *dataptr, size_t size, int flags, const struct sockaddr *to, socklen_t tolen){
    DEBUG(LVL0,"args %i %x %i %i %x %i",s,dataptr,size,flags,to,tolen);
    return zsock_sendto(s,dataptr,size,flags,to,tolen);
}
int gzsock_recvfrom(int s, void *mem, size_t len, int flags, struct sockaddr *from, socklen_t *fromlen){
    DEBUG(LVL0,"args %i %x %i %i %x %x",s,mem,len,flags,from,fromlen);
    return zsock_recvfrom(s,mem,len,flags,from,fromlen);
}
int gzsock_bind(int s, const struct sockaddr *name, socklen_t namelen){
    DEBUG(LVL0,"args %i %x %i",s,name,namelen);
    return zsock_bind(s,name,namelen);
}
int gzsock_listen(int s, int backlog){
    DEBUG(LVL0,"args %i %i",s,backlog);
    return zsock_listen(s,backlog);
}

int gzsock_accept(int s, struct sockaddr *addr, socklen_t *addrlen){
    DEBUG(LVL0,"args %i %x %x",s,addr,addrlen);
    return zsock_accept(s,addr,addrlen);
}

int gzsock_select(int maxfdp1, void *readset, void *writeset, void *exceptset, struct timeval *timeout){
    DEBUG(LVL0,"args %i %x %x %x %x",maxfdp1,readset,writeset,exceptset,timeout);
#if defined(ZERYNTH_SSL)
    if (IS_SECURE_SOCKET(maxfdp1-1)) {
        return zssl_select(maxfdp1,readset,writeset,exceptset,timeout);
    } else 
#endif
    {
        return zsock_select(maxfdp1,readset,writeset,exceptset,timeout);
    }
}

int zs_addr_to_string(struct sockaddr_in *addr, uint8_t *saddr) {
    uint8_t *buf = saddr;
    buf+=modp_itoa10(OAL_IP_AT(addr->sin_addr.s_addr, 0),buf);
    *buf++='.';
    buf+=modp_itoa10(OAL_IP_AT(addr->sin_addr.s_addr, 1),buf);
    *buf++='.';
    buf+=modp_itoa10(OAL_IP_AT(addr->sin_addr.s_addr, 2),buf);
    *buf++='.';
    buf+=modp_itoa10(OAL_IP_AT(addr->sin_addr.s_addr, 3),buf);
    return buf-saddr;
}

int zs_string_to_addr(uint8_t *saddr, int len, struct sockaddr_in *addr) {
    
    int32_t cnt = 0, ph = 3, cnz = 0, oct = 0;
    while (cnt < len) {
        if (saddr[cnt] >= '0' && saddr[cnt] <= '9') {
            oct *= 10;
            oct += saddr[cnt] - '0';
            cnz++;
        } else if (saddr[cnt] == '.') {
            OAL_IP_SET(addr->sin_addr.s_addr, 3 - ph, oct);
            ph--;
            cnz = oct = 0;
        } else return -1;
        cnt++;
        if (cnz > 3 || ph < 0)
            return -1;
    }
    if (ph != 0) return -1;
    OAL_IP_SET(addr->sin_addr.s_addr, 3, oct);
    return ERR_OK;
}

#if !defined(ZERYNTH_SOCKETS_EXTERNAL_TCP_STACK)
int errno;
#endif

