//This is for drivers that have standard sockets based on lwip
#if defined(ZERYNTH_SOCKETS_PYNATIVE)

//If lwip headers are external, include files
#if defined(ZERYNTH_SOCKETS_EXTERNAL_TCP_STACK)
#include "lwip/netif.h"
#include "lwip/dns.h"
#include "lwip/sockets.h"
#include "lwip/api.h"
#include "lwip/ip_addr.h"
#endif

//zerynth stuff
#include "zerynth.h"
#include "zerynth_sockets.h"
#include "zerynth_sockets_debug.h"


#if !defined(ZERYNTH_SOCKETS_PYNATIVE_CUSTOM_RESOLVE)
C_NATIVE(py_net_resolve)
{
    C_NATIVE_UNWARN();
    uint8_t* url;
    uint32_t len;
    int32_t code;
    NetAddress addr;
    if (parse_py_args("s", nargs, args, &url, &len) != 1)
        return ERR_TYPE_EXC;
    addr.ip = 0;
    uint8_t* name = (uint8_t*)gc_malloc(len + 1);
    __memcpy(name, url, len);
    name[len] = 0;
    DEBUG(LVL0,"Resolving %s",name);
    RELEASE_GIL();
    struct ip4_addr ares;
    code = netconn_gethostbyname(name, &ares);
    ACQUIRE_GIL();
    gc_free(name);
    DEBUG(LVL0,"Resolved return code %i",code);
    if (code != ERR_OK)
        return ERR_IOERROR_EXC;
    addr.port = 0;
    addr.ip = ares.addr;
    *res = netaddress_to_object(&addr);
    return ERR_OK;
}
#endif

#define DRV_SOCK_DGRAM 1
#define DRV_SOCK_STREAM 0
#define DRV_AF_INET 0

typedef struct sockaddr_in sockaddr_t;

void py_prepare_addr(sockaddr_t* vmSocketAddr, NetAddress* addr)
{
    vmSocketAddr->sin_family = AF_INET;
    vmSocketAddr->sin_port = addr->port;
    vmSocketAddr->sin_addr.s_addr = addr->ip;
}

C_NATIVE(py_net_socket)
{
    C_NATIVE_UNWARN();
    int32_t family = DRV_AF_INET;
    int32_t type = DRV_SOCK_STREAM;
    int32_t proto = IPPROTO_TCP;
    int32_t sock;
    if (parse_py_args("III", nargs, args, DRV_AF_INET, &family, DRV_SOCK_STREAM, &type, IPPROTO_TCP, &proto) != 3)
        return ERR_TYPE_EXC;
    if (type != DRV_SOCK_DGRAM && type != DRV_SOCK_STREAM)
        return ERR_TYPE_EXC;
    if (family != DRV_AF_INET)
        return ERR_UNSUPPORTED_EXC;

 
    DEBUG(LVL0,"Creating socket -> family %i type %i proto %i",family,type,proto);
    RELEASE_GIL();
    sock = gzsock_socket(AF_INET, (type == DRV_SOCK_DGRAM) ? SOCK_DGRAM : SOCK_STREAM,
        (type == DRV_SOCK_DGRAM) ? IPPROTO_UDP : IPPROTO_TCP, NULL);
    ACQUIRE_GIL();

    if (sock < 0)
        return ERR_IOERROR_EXC;
    DEBUG(LVL0,"Created socket %i",sock);
    *res = PSMALLINT_NEW(sock);
    return ERR_OK;
}

C_NATIVE(py_net_connect)
{
    C_NATIVE_UNWARN();
    int32_t sock,err;
    NetAddress addr;

    if (parse_py_args("in", nargs, args, &sock, &addr) != 2)
        return ERR_TYPE_EXC;
    sockaddr_t vmSocketAddr;
    py_prepare_addr(&vmSocketAddr, &addr);
    DEBUG(LVL0,"Connecting with socket %i",sock);
    RELEASE_GIL();
    err = gzsock_connect(sock, &vmSocketAddr, sizeof(vmSocketAddr));
    ACQUIRE_GIL();
    DEBUG(LVL0,"Connected with socket %i return code %i",sock,err);
    if (err < 0) {
        return ERR_IOERROR_EXC;
    }
    *res = PSMALLINT_NEW(sock);
    return ERR_OK;
}

C_NATIVE(py_net_close)
{
    C_NATIVE_UNWARN();
    int32_t sock;
    int rr;
    if (parse_py_args("i", nargs, args, &sock) != 1)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    rr = gzsock_close(sock);
    DEBUG(LVL0,"Closed socket %i return code %i",sock,rr);
    ACQUIRE_GIL();
    *res = PSMALLINT_NEW(sock);
    return ERR_OK;
}

C_NATIVE(py_net_send)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    int32_t len;
    int32_t flags;
    int32_t sock;
    int32_t snt;
    if (parse_py_args("isi", nargs, args,
            &sock,
            &buf, &len,
            &flags)
        != 3)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    DEBUG(LVL0,"Sending with socket %i %i bytes",sock,len);
    snt = gzsock_send(sock, buf, len, flags);
    ACQUIRE_GIL();
    if (snt < 0) {
        return ERR_IOERROR_EXC;
    }
    DEBUG(LVL0,"Sent with socket %i %i bytes",sock,snt);
    *res = PSMALLINT_NEW(snt);
    return ERR_OK;
}

C_NATIVE(py_net_send_all)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    int32_t len;
    int32_t flags;
    int32_t sock;
    int32_t wrt;
    int32_t w;
    if (parse_py_args("isi", nargs, args,
            &sock,
            &buf, &len,
            &flags)
        != 3)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    DEBUG(LVL0,"Sending with socket %i %i bytes",sock,len);
    wrt = 0;
    while (wrt < len) {
        w = gzsock_send(sock, buf + wrt, len - wrt, flags);
        if (w < 0)
            break;
        wrt += w;
    }
    ACQUIRE_GIL();
    if (w < 0) {
        return ERR_IOERROR_EXC;
    }
    DEBUG(LVL0,"Sent with socket %i %i bytes",sock,wrt);
    *res = MAKE_NONE();
    return ERR_OK;
}

C_NATIVE(py_net_sendto)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    int32_t len;
    int32_t flags;
    int32_t sock;
    NetAddress addr;
    if (parse_py_args("isni", nargs, args,
            &sock,
            &buf, &len,
            &addr,
            &flags)
        != 4)
        return ERR_TYPE_EXC;

    RELEASE_GIL();
    sockaddr_t vmSocketAddr;
    py_prepare_addr(&vmSocketAddr, &addr);
    sock = gzsock_sendto(sock, buf, len, flags, &vmSocketAddr, sizeof(sockaddr_t));
    ACQUIRE_GIL();

    if (sock < 0) {
        return ERR_IOERROR_EXC;
    }
    *res = PSMALLINT_NEW(sock);
    return ERR_OK;
}

C_NATIVE(py_net_recv_into)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    int32_t len;
    int32_t sz;
    int32_t flags;
    int32_t ofs;
    int32_t sock;
    if (parse_py_args("isiiI", nargs, args,
            &sock,
            &buf, &len,
            &sz,
            &flags,
            0,
            &ofs)
        != 5)
        return ERR_TYPE_EXC;
    buf += ofs;
    len -= ofs;
    len = (sz < len) ? sz : len;
    RELEASE_GIL();
    int rb = 0;
    int r;
    while (rb < len) {
        r = gzsock_recv(sock, buf + rb, len - rb, flags);
        if (r <= 0)
            break;
        rb += r;
    }
    ACQUIRE_GIL();
    if (r <= 0) {
       if (r != 0){
            if (r==-ETIMEDOUT)
            // if (r == MBEDTLS_ERR_SSL_TIMEOUT /*|| *__errno() == EAGAIN || *__errno() == ETIMEDOUT*/)
                return ERR_TIMEOUT_EXC;
            return ERR_IOERROR_EXC;
        }
    }
    *res = PSMALLINT_NEW(rb);

    return ERR_OK;
}

C_NATIVE(py_net_recvfrom_into)
{
    C_NATIVE_UNWARN();
    uint8_t* buf;
    int32_t len;
    int32_t sz;
    int32_t flags;
    int32_t ofs;
    int32_t sock;
    NetAddress addr;
    if (parse_py_args("isiiI", nargs, args,
            &sock,
            &buf, &len,
            &sz,
            &flags,
            0,
            &ofs)
        != 5)
        return ERR_TYPE_EXC;
    buf += ofs;
    len -= ofs;
    len = (sz < len) ? sz : len;

    RELEASE_GIL();
    addr.ip = 0;
    int r;
    sockaddr_t vmSocketAddr;
    socklen_t tlen = sizeof(vmSocketAddr);
    r = gzsock_recvfrom(sock, buf, len, flags, &vmSocketAddr, &tlen);
    ACQUIRE_GIL();
    addr.ip = vmSocketAddr.sin_addr.s_addr;
    addr.port = vmSocketAddr.sin_port;
    if (r < 0) {
        if (r == ETIMEDOUT)
            return ERR_TIMEOUT_EXC;
        return ERR_IOERROR_EXC;
    }
    PTuple* tpl = (PTuple*)psequence_new(PTUPLE, 2);
    PTUPLE_SET_ITEM(tpl, 0, PSMALLINT_NEW(r));
    PObject* ipo = netaddress_to_object(&addr);
    PTUPLE_SET_ITEM(tpl, 1, ipo);
    *res = tpl;
    return ERR_OK;
}

C_NATIVE(py_net_setsockopt)
{
    C_NATIVE_UNWARN();
    int32_t sock;
    int32_t level;
    int32_t optname;
    int32_t optvalue;

    if (parse_py_args("iiii", nargs, args, &sock, &level, &optname, &optvalue) != 4)
        return ERR_TYPE_EXC;

    if (level == 0xffff)
        level = SOL_SOCKET;

    // SO_RCVTIMEO zerynth value
    if (optname == 1) {
        optname = SO_RCVTIMEO;
    }

    RELEASE_GIL();
    if (optname == SO_RCVTIMEO) {
        struct timeval tms;
        tms.tv_sec = optvalue / 1000;
        tms.tv_usec = (optvalue % 1000) * 1000;
        sock = gzsock_setsockopt(sock, level, optname, &tms, sizeof(struct timeval));
    }
    else {
        sock = gzsock_setsockopt(sock, level, optname, &optvalue, sizeof(optvalue));
    }
    ACQUIRE_GIL();
    if (sock < 0)
        return ERR_IOERROR_EXC;

    *res = MAKE_NONE();
    return ERR_OK;
}

C_NATIVE(py_net_bind)
{
    C_NATIVE_UNWARN();
    int32_t sock;
    NetAddress addr;
    if (parse_py_args("in", nargs, args, &sock, &addr) != 2)
        return ERR_TYPE_EXC;
    sockaddr_t serverSocketAddr;
    //addr.ip = bcm_net_ip.addr;
    py_prepare_addr(&serverSocketAddr, &addr);
    RELEASE_GIL();
    sock = gzsock_bind(sock, &serverSocketAddr, sizeof(sockaddr_t));
    ACQUIRE_GIL();
    if (sock < 0)
        return ERR_IOERROR_EXC;
    *res = MAKE_NONE();
    return ERR_OK;
}

C_NATIVE(py_net_listen)
{
    C_NATIVE_UNWARN();
    int32_t maxlog;
    int32_t sock;
    if (parse_py_args("ii", nargs, args, &sock, &maxlog) != 2)
        return ERR_TYPE_EXC;
    RELEASE_GIL();
    maxlog = gzsock_listen(sock, maxlog);
    ACQUIRE_GIL();
    if (maxlog)
        return ERR_IOERROR_EXC;
    *res = MAKE_NONE();
    return ERR_OK;
}

C_NATIVE(py_net_accept)
{
    C_NATIVE_UNWARN();
    int32_t sock;
    NetAddress addr;
    if (parse_py_args("i", nargs, args, &sock) != 1)
        return ERR_TYPE_EXC;
    sockaddr_t clientaddr;
    socklen_t addrlen;
    memset(&clientaddr, 0, sizeof(sockaddr_t));
    addrlen = sizeof(sockaddr_t);
    RELEASE_GIL();
    sock = gzsock_accept(sock, &clientaddr, &addrlen);
    ACQUIRE_GIL();
    if (sock < 0)
        return ERR_IOERROR_EXC;
    addr.port = clientaddr.sin_port;
    addr.ip = clientaddr.sin_addr.s_addr;

    PTuple* tpl = (PTuple*)psequence_new(PTUPLE, 2);
    PTUPLE_SET_ITEM(tpl, 0, PSMALLINT_NEW(sock));
    PObject* ipo = netaddress_to_object(&addr);
    PTUPLE_SET_ITEM(tpl, 1, ipo);
    *res = tpl;
    return ERR_OK;
}

C_NATIVE(py_net_select)
{
    C_NATIVE_UNWARN();
    int32_t timeout;
    int32_t tmp, i, j, sock = -1;

    if (nargs < 4)
        return ERR_TYPE_EXC;

    fd_set rfd;
    fd_set wfd;
    fd_set xfd;
    struct timeval tms;
    struct timeval* ptm;
    PObject* rlist = args[0];
    PObject* wlist = args[1];
    PObject* xlist = args[2];
    fd_set* fdsets[3] = { &rfd, &wfd, &xfd };
    PObject* slist[3] = { rlist, wlist, xlist };
    PObject* tm = args[3];

    if (tm == MAKE_NONE()) {
        ptm = NULL;
    }
    else if (IS_PSMALLINT(tm)) {
        timeout = PSMALLINT_VALUE(tm);
        if (timeout < 0)
            return ERR_TYPE_EXC;
        tms.tv_sec = timeout / 1000;
        tms.tv_usec = (timeout % 1000) * 1000;
        ptm = &tms;
    }
    else
        return ERR_TYPE_EXC;

    for (j = 0; j < 3; j++) {
        tmp = PTYPE(slist[j]);
        if (!IS_OBJ_PSEQUENCE_TYPE(tmp))
            return ERR_TYPE_EXC;
        FD_ZERO(fdsets[j]);
        for (i = 0; i < PSEQUENCE_ELEMENTS(slist[j]); i++) {
            PObject* fd = PSEQUENCE_OBJECTS(slist[j])[i];
            if (IS_PSMALLINT(fd)) {
                FD_SET(PSMALLINT_VALUE(fd), fdsets[j]);
                if (PSMALLINT_VALUE(fd) > sock)
                    sock = PSMALLINT_VALUE(fd);
            }
            else
                return ERR_TYPE_EXC;
        }
    }

    RELEASE_GIL();
    tmp = gzsock_select((sock + 1), fdsets[0], fdsets[1], fdsets[2], ptm);
    ACQUIRE_GIL();


    if (tmp < 0) {
        return ERR_IOERROR_EXC;
    }

    PTuple* tpl = (PTuple*)psequence_new(PTUPLE, 3);
    for (j = 0; j < 3; j++) {
        tmp = 0;
        for (i = 0; i <= sock; i++) {
            if (FD_ISSET(i, fdsets[j]))
                tmp++;
        }
        PTuple* rtpl = psequence_new(PTUPLE, tmp);
        tmp = 0;
        for (i = 0; i <= sock; i++) {
            if (FD_ISSET(i, fdsets[j])) {
                PTUPLE_SET_ITEM(rtpl, tmp, PSMALLINT_NEW(i));
                tmp++;
            }
        }
        PTUPLE_SET_ITEM(tpl, j, rtpl);
    }
    *res = tpl;
    return ERR_OK;
}


#define _CERT_NONE 1
#define _CERT_OPTIONAL 2
#define _CERT_REQUIRED 4
#define _CLIENT_AUTH 8
#define _SERVER_AUTH 16


// ZHWCryptoAPIPointers *zhwcrypto_api_pointers_backup = NULL;
// extern ZHWCryptoAPIPointers null_api_pointers;

C_NATIVE(py_secure_socket)
{
    C_NATIVE_UNWARN();
#if !defined(ZERYNTH_SSL)
    return ERR_UNSUPPORTED_EXC;
#else
    int32_t err = ERR_OK;
    int32_t family = DRV_AF_INET;
    int32_t type = DRV_SOCK_STREAM;
    int32_t proto = IPPROTO_TCP;
    int32_t sock;
    int32_t i;
    SSLInfo nfo;

    int32_t ssocknum = 0;
    int32_t ctxlen;
    uint8_t* certbuf = NULL;
    uint16_t certlen = 0;
    uint8_t* clibuf = NULL;
    uint16_t clilen = 0;
    uint8_t* pkeybuf = NULL;
    uint16_t pkeylen = 0;
    uint32_t options = _CLIENT_AUTH | _CERT_NONE;
    uint8_t* hostbuf = NULL;
    uint16_t hostlen = 0;

    PTuple* ctx;
    memset(&nfo,0,sizeof(nfo));
    ctx = (PTuple*)args[nargs - 1];
    nargs--;
    if (parse_py_args("III", nargs, args, DRV_AF_INET, &family, DRV_SOCK_STREAM, &type, IPPROTO_TCP, &proto) != 3){
        return ERR_TYPE_EXC;
    }
    if (type != DRV_SOCK_DGRAM && type != DRV_SOCK_STREAM){
        return ERR_TYPE_EXC;
    }
    if (family != DRV_AF_INET)
        return ERR_UNSUPPORTED_EXC;

    ctxlen = PSEQUENCE_ELEMENTS(ctx);
    if (ctxlen && ctxlen != 5)
        return ERR_TYPE_EXC;

    if (ctxlen) {
        //ssl context passed
        PObject* cacert = PTUPLE_ITEM(ctx, 0);
        PObject* clicert = PTUPLE_ITEM(ctx, 1);
        PObject* ppkey = PTUPLE_ITEM(ctx, 2);
        PObject* host = PTUPLE_ITEM(ctx, 3);
        PObject* iopts = PTUPLE_ITEM(ctx, 4);

        nfo.cacert = PSEQUENCE_BYTES(cacert);
        nfo.cacert_len = PSEQUENCE_ELEMENTS(cacert);
        nfo.clicert = PSEQUENCE_BYTES(clicert);
        nfo.clicert_len = PSEQUENCE_ELEMENTS(clicert);
        nfo.hostname = PSEQUENCE_BYTES(host);
        nfo.hostname_len = PSEQUENCE_ELEMENTS(host);
        nfo.pvkey = PSEQUENCE_BYTES(ppkey);
        nfo.pvkey_len = PSEQUENCE_ELEMENTS(ppkey);
        nfo.options = PSMALLINT_VALUE(iopts);
    }
    RELEASE_GIL();
    DEBUG(LVL0,"Creating secure socket","");
    sock = gzsock_socket(
          AF_INET,
          (type == DRV_SOCK_DGRAM) ? SOCK_DGRAM : SOCK_STREAM,
          (type == DRV_SOCK_DGRAM) ? IPPROTO_UDP : IPPROTO_TCP,
          (ctxlen) ? &nfo:NULL);
    ACQUIRE_GIL();
    if (sock < 0)
        return ERR_IOERROR_EXC;
    DEBUG(LVL0,"Created secure socket %i",sock);
    *res = PSMALLINT_NEW(sock);
    return ERR_OK;
#endif
}




#endif
