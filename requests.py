"""
.. module:: requests

********
Requests
********

This module implements functions to easily handle the intricacies of the HTTP protocol. The name and the API are inspired by the wonderful Python module `Requests <http://docs.python-requests.org/>`_.
To use *requests* a net driver must have been properly configured and started.

    """

import urlparse
import socket
import streams
import json as json_encoder
import ssl

new_exception(HTTPError,Exception)
new_exception(HTTPConnectionError,HTTPError)
new_exception(HTTPResponseError,HTTPError)

zverbs = ("GET", "POST", "PUT", "DELETE", "OPTIONS", "PATCH", "HEAD")


def get_pdata(data,json):
    pdata = None
    if data is not None:
        if type(data)==PDICT:
            pdata = [urlparse.urlencode(data),"application/x-www-form-urlencoded"]
        else:
            pdata = [data,""]
    elif json is not None:
        pdata = [json_encoder.dumps(json),"application/json"]

    return pdata



def get(url,params=None,headers=None, connection=None,ctx=None,stream_callback=None,stream_chunk=512):
    """
.. function:: get(url,params=None,headers=None,connection=None,stream_callback=None,stream_chunk=512)    

    Implements the GET method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *params* is given as a dictionary, each pair (key, value) is appended to the requested url, properly encoded and sent.

    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *connection* is given, the initial connection step is skipped and *connection* is used for communication. This feature allows the reuse of a 
    connection to a HTTP server opened with a "Keep-Alive" header.

    *get* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)

    If the parameter *stream_callback* is given, the HTTP body data will be retrieved in chunk s of *stream_chunk* size and passed as arguments to *stream_callback* one by one. If *stream_callback* is used, the content of :class:`Response` instance is the last chunk.


    """
    return _verb(url,None,params,headers,connection,"GET",ctx, stream_callback,stream_chunk)


def post(url,data=None,json=None,headers=None,ctx=None):
    """
.. function:: post(url,data=None,json=None,headers=None,ctx=None)    

    Implements the POST method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *data* is provided (always as dictionary), each pair (key, value) will be form-encoded and send in the body of the request with {"content-type":"application/x-www-form-urlencoded"} appended in the headers.
    If *json* is provided (always as dictionary), json data will send in the body of the request with {"content-type":"application/json"} appended in the headers.

    .. note:: if both (*data* and *json*) dict are provided, json data are ignored and post request is performed with urlencoded data.

    *post* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    """
    pdata = get_pdata(data,json)
    return _verb(url,pdata,None,headers,None,"POST",ctx)


def put(url,data=None,json=None,headers=None,ctx=None):
    """
.. function:: put(url,data=None,json=None,headers=None,ctx=None)    

    Implements the PUT method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *data* is provided (always as dictionary), each pair (key, value) will be form-encoded and send in the body of the request with {"content-type":"application/x-www-form-urlencoded"} appended in the headers.
    If *json* is provided (always as dictionary), json data will send in the body of the request with {"content-type":"application/json"} appended in the headers.

    .. note:: if both (*data* and *json*) dict are provided, json data are ignored and post request is performed with urlencoded data.


    *put* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    """
    pdata = get_pdata(data,json)
    return _verb(url,pdata,None,headers,None,"PUT",ctx)


def patch(url,data=None,json=None,headers=None,ctx=None):
    """
.. function:: patch(url,data=None,headers=None,ctx=None)    

    Implements the PATCH method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *data* is provided (always as dictionary), each pair (key, value) will be form-encoded and send in the body of the request with {"content-type":"application/x-www-form-urlencoded"} appended in the headers.
    If *json* is provided (always as dictionary), json data will send in the body of the request with {"content-type":"application/json"} appended in the headers.

    .. note:: if both (*data* and *json*) dict are provided, json data are ignored and post request is performed with urlencoded data.


    *patch* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    """
    pdata = get_pdata(data,json)
    return _verb(url,pdata,None,headers,None,"PATCH",ctx)


def delete(url,headers=None,ctx=None):
    """
.. function:: delete(url,headers=None,ctx=None)    

    Implements the DELETE method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    *delete* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    """
    return _verb(url,None,None,headers,None,"DELETE",ctx)


def head(url,headers=None,ctx=None):
    """
.. function:: head(url,headers=None,ctx=None)    

    Implements the HEAD method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    *head* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    """
    return _verb(url,None,None,headers,None,"HEAD",ctx)


def options(url,headers=None,ctx=None):
    """
.. function:: options(url,headers=None,ctx=None)    

    Implements the OPTIONS method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    *options* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    """
    return _verb(url,None,None,headers,None,"OPTIONS",ctx)


def upload(url,fd,ctx=None,mime_type="application/octet-stream",method="POST"):
    """
.. function:: upload(url,fd,ctx=None,mime_type="application/octet-stream",method="POST")

    Upload a file identified by *fd* to *url*. *fd* must provide methods read and size.

    A tcp connection is made to the host:port given in the url using the default net driver.

    The type of the file contents and the HTTP method (POST pr PUT) can be customized.

    *upload* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, file, etc..)


    """
    return _verb(url,None,None,{"content-type":mime_type},None,method,ctx,None,0,fd)



def _connect(host,port,scheme,ctx):
    try:
        #print("_connect",host,port,scheme)
        ip = __default_net["sock"][0].gethostbyname(host)
        #print(ip)
        if port: # port
            port = int(port)
        elif scheme=="http":
            port = 80
        else:
            port = 443

        ip = (ip,port) 
        if scheme=="http":
            sock=socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        else:
            if ctx is None:
                ctx = ()
            sock=ssl.sslsocket(ctx=ctx)
        # print(ip)
        sock.connect(ip)
        return sock
    except ConnectionError as e:
        sock.close()
        raise e
    except IOError:
        #print("IOError")
        raise HTTPConnectionError
    except Exception as e:
        raise e


def _readline(ssock,buffer,ofs,size):
    try:
        msg = ssock.readline(buffer=buffer,ofs=ofs,size=size)
        if len(msg)==0:
            raise ConnectionError
        return msg
    except Exception as e:
        ssock.close()
        raise e

BUFFER_LEN = 2048

def add_to_buffer(buffer,what,pos):
    l =len(what)
    npos = pos+l
    if l>0 and npos<BUFFER_LEN:
        npos = pos+l
        l=0
        #TODO: use buffer[pos:npos]=what
        while pos<npos:
            buffer[pos]=__byte_get(what,l)
            l+=1
            pos+=1
        return npos
    return pos 


def _verb(url,data=None,params=None,headers=None,connection=None,verb=None,ctx=None,stream_callback=None,stream_chunk=512,fd=None):
    urlp = urlparse.parse(url)
    netl = urlparse.parse_netloc(urlp[1])
    host = netl[2]
    # print(verb,urlp,netl)
    if connection:
        sock = connection
    else:
        sock = _connect(host,netl[3],urlp[0],ctx)

    # print("CREATED SOCKET",sock.channel)
    #Generate and send Request Line
    msg = bytearray(BUFFER_LEN)
    p = 0
    endline = "\r\n"
    p = add_to_buffer(msg,verb,p)
    p = add_to_buffer(msg," ",p)

    if not urlp[2]:
        p = add_to_buffer(msg,"/",p)
    else:
        p = add_to_buffer(msg,urlp[2],p)
    if (verb in zverbs) and (urlp[-2] or params):
        p = add_to_buffer(msg,"?",p)
        if urlp[-2]:
            p = add_to_buffer(msg,urlp[-2],p)
            if params:
                p = add_to_buffer(msg,"&",p)
        if params:
            p = add_to_buffer(msg,urlparse.urlencode(params),p)

    if msg[p-1]!=__ORD(" "):
        p = add_to_buffer(msg," ",p)
    p = add_to_buffer(msg,"HTTP/1.1",p)
    p = add_to_buffer(msg,endline,p)
    __elements_set(msg,p) #clamp buffer
    #send cmd
    try:
        # print(">>",msg)
        sock.sendall(msg)
    except Exception as e:
        sock.close()
        raise HTTPConnectionError

    __elements_set(msg,BUFFER_LEN)
    p = 0
    #Generated and send Request headers
    p = add_to_buffer(msg,"Host: ",p)
    p = add_to_buffer(msg,host,p)
    if netl[3]:
        p = add_to_buffer(msg,":",p)
        p = add_to_buffer(msg,netl[3],p)
            
    p = add_to_buffer(msg,endline,p)
    __elements_set(msg,p)
    # print(">>",msg)
    sock.sendall(msg)
    __elements_set(msg,BUFFER_LEN)
    p = 0

    rh = {}
    if headers:
       for k in headers:
            rh[k.lower()]=headers[k]
    
    if "connection" not in rh:
        rh["connection"]="close"    

    if data is not None:
        rh["content-length"] = str(len(data[0])) #data[0] is actual data
        if data[1]:
            rh["content-type"] = data[1]             #data[1] is data type header
    if fd is not None:
        rh["content-length"] = str(fd.size())



    for k,v in rh.items():
        p = add_to_buffer(msg,k,p)
        p = add_to_buffer(msg,": ",p)
        p = add_to_buffer(msg,v,p)
        p = add_to_buffer(msg,endline,p)
        __elements_set(msg,p)
        # print(">>",msg)
        sock.sendall(msg)
        __elements_set(msg,BUFFER_LEN)
        p = 0

    # Generate and send Body
    p = add_to_buffer(msg,endline,p)

    if data is not None:
        p = add_to_buffer(msg,data[0],p)
    
    __elements_set(msg,p)
    # print(">>",msg)
    sock.sendall(msg)
    __elements_set(msg,BUFFER_LEN)
    # stream body
    if fd is not None:
        while True:
            rd = fd.read(512)
            if len(rd)>0:
                sock.sendall(rd)
            else:
                break
    p = 0


    #Parse Response
    rr = Response()

    ssock = streams.SocketStream(sock)
    buffer = msg
    msg = _readline(ssock,buffer,0,BUFFER_LEN)
    # print("<<",msg)
    
    if msg.startswith("HTTP/1."):
        rr.status = int(msg[9:12])

    __elements_set(msg,BUFFER_LEN)
    msg = _readline(ssock,buffer,0,BUFFER_LEN)
    # print("<<",msg)
    #print(msg)

    #print(">[",msg,"]",msg=="\n",msg==endline)
    while not (msg==endline or msg=="\n"):
        idx_cl = msg.find(__ORD(":"))
        if idx_cl<0:
            sock.close()
            raise HTTPResponseError
        rr.headers[str(msg[0:idx_cl].lower())]=str(msg[idx_cl+1:-2].strip(endline))
        __elements_set(msg,BUFFER_LEN)
        msg = _readline(ssock,buffer,0,BUFFER_LEN)
        # print("<<",msg)
        # print(msg)
        #print(">[",msg,"]",msg=="\n",msg==endline)
   
    #print(rr.headers)
    rr.connection=sock

    if verb != "HEAD":
        # read response body
        if "content-length" in rr.headers:
            bodysize=int(rr.headers["content-length"])
            #print("bodysize",bodysize)
            contentsize = bodysize
            reset_content = False
            if stream_callback is not None:
                reset_content = True
                contentsize = stream_chunk

            rr.content = bytearray(contentsize)
            tmp = 0
            rdr = 0
            while bodysize>0:
                if reset_content:
                    ofs=0
                else:
                    ofs=tmp
                rdr = sock.recv_into(rr.content,min(bodysize,contentsize),ofs=ofs)
                if rdr and stream_callback:
                    __elements_set(rr.content,rdr)
                    stream_callback(rr.content)
                #print(rdr,rr.content[tmp:tmp+rdr])
                tmp+=rdr
                bodysize-=rdr
                if not rdr:
                    break
            #print("CLOSED SOCKET A",sock.channel,rdr,tmp,bodysize)
        elif "transfer-encoding" in rr.headers:
            if stream_callback:
                chbuf = bytearray(stream_chunk)
            last_term = False
            while True:
                __elements_set(msg,BUFFER_LEN)
                msg = _readline(ssock,buffer,0,BUFFER_LEN)
                # print(">>",msg,len(msg),[hex(x) for x in msg])
                if len(msg)==0:
                    # print("breaking out of loop")
                    break
                if msg=="\r\n":
                    # print("received terminator",last_term)
                    if last_term:
                        break
                    else:
                        continue
                chsize = int(msg,16)
                # print("chsize",chsize)
                if chsize:
                    if stream_callback:
                        while chsize>0:
                            rdr = sock.recv_into(chbuf,min(chsize,stream_chunk),ofs=0)
                            chsize-=rdr
                            # print("rdr",rdr,chsize)
                            __elements_set(chbuf,rdr)
                            stream_callback(chbuf)
                            __elements_set(chbuf,stream_chunk)
                    else:
                        chbuf = sock.recv(chsize)
                        # print(msg)
                        # if msg:
                        rr.content.extend(chbuf)
                        # else:
                            # break
                        # __elements_set(buffer,BUFFER_LEN)
                        # msg = _readline(ssock,buffer,0,BUFFER_LEN)
                else:
                    last_term=True
                # else:
                #     print("Read terminator")
                #     __elements_set(buffer,BUFFER_LEN)
                #     msg = _readline(ssock,buffer,0,2) #terminator
                    # break
            #print("CLOSED SOCKET B",sock.channel)
        else:
            while True:
                tmp = sock.recv(32)
                if tmp:
                    rr.content.extend(tmp)
                    #print(tmp)
                else:
                    break
            #print("CLOSED SOCKET C",sock.channel)

    # handle connection close or keep-alive
    if "connection" in rr.headers and rr.headers["connection"] == "keep-alive":
        rr.connection = sock
    else:
        sock.close()
        rr.connection = None

    return rr


class Response():
    """
.. class:: Response

    This class represent the result of a HTTP request.

    It contains the following members:

    .. attribute:: status

        Contains the HTTP response code

    .. attribute:: content

        It is the bytearray containing the byte version of the content section of a HTTP response

    .. attribute:: headers

        A dictionary with all the response headers

    .. attribute:: connection

        the connection used to communicate with the server, or None if it has been closed.

    """
    def __init__(self):
        self.status = 0
        self.content = bytearray()
        self.headers = {}
        self.connection = None
    def text(self):
        """
.. method:: text()

    Returns a string representing the content section of the HTTP response
        """
        return str(self.content)
    
    def json(self):
        return json_encoder.loads(self.content)
