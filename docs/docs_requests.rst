.. module:: requests

********
Requests
********

This module implements functions to easily handle the intricacies of the HTTP protocol. The name and the API are inspired by the wonderful Python module `Requests <http://docs.python-requests.org/>`_.
To use *requests* a net driver must have been properly configured and started.

    
.. function:: get(url,params=None,headers=None,connection=None)    

    Implements the GET method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *params* is given as a dictionary, each pair (key, value) is appended to the requested url, properly encoded and sent.

    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *connection* is given, the initial connection step is skipped and *connection* is used for communication. This feature allows the reuse of a 
    connection to a HTTP server opened with a "Keep-Alive" header.

    *get* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    
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


    
.. function:: put(url,data=None,headers=None,ctx=None)    

    Implements the PUT method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *data* is provided (always as dictionary), each pair (key, value) will be form-encoded and send in the body of the request with {"content-type":"application/x-www-form-urlencoded"} appended in the headers.

    *put* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    
.. function:: patch(url,data=None,headers=None,ctx=None)    

    Implements the PATCH method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    If *data* is provided (always as dictionary), each pair (key, value) will be form-encoded and send in the body of the request with {"content-type":"application/x-www-form-urlencoded"} appended in the headers.

    *patch* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    
.. function:: delete(url,headers=None,ctx=None)    

    Implements the DELETE method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    *delete* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    
.. function:: head(url,headers=None,ctx=None)    

    Implements the HEAD method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    *head* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    
.. function:: options(url,headers=None,ctx=None)    

    Implements the OPTIONS method of the HTTP protocol. A tcp connection is made to the host:port given in the url using the default net driver.
    
    If *headers* is given as a dictionary, each pair (key, value) is appropriately sent as a HTTP request header. Mandatory headers are transparently handled: "Host:" is always derived by parsing *url*;
    other headers are set to defaults if not given: for example "Connection: close" is sent if no value for "Connection" is specified in *headers*. To request a permanent connection,
    *headers* must contain the pair {"Connection":"Keep-Alive"}.

    *options* returns a :class:`Response` instance.

    Exceptions can be raised: :exc:`HTTPConnectionError` when the HTTP server can't be contacted; :exc:`IOError` when the source of error lies at the socket level (i.e. closed sockets, invalid sockets, etc..)


    
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

    
.. method:: text()

    Returns a string representing the content section of the HTTP response
        
