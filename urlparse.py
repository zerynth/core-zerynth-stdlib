"""
.. module:: urlparse

********
Urlparse
********

This module defines a standard interface to break Uniform Resource Locator (URL)
strings up in components (addressing scheme, network location, path etc.)


    """

def parse(url):
    """
.. function:: parse(url)

   Returns a tuple (*scheme*, *netloc*, *path*, *query*, *fragment*) derived from *url*.

   This corresponds to the general structure of a URL: ``scheme://netloc/path?query#fragment``.
   Each tuple item is a string, possibly empty. The components are not broken up in
   smaller parts (for example, the network location is a single string), and %
   escapes are not expanded. The delimiters as shown above are not part of the
   result, except for a leading slash in the *path* component, which is retained if
   present.  For example:

      import urlparse
      o = urlparse('http://www.cwi.nl:80/%7Eguido/Python.html')
      
      # result is ('http', 'www.cwi.nl:80', '/%7Eguido/Python.html', '', '')
      

   Following the syntax specifications in :rfc:`1808`, urlparse recognizes
   a netloc only if it is properly introduced by '//'.  Otherwise the
   input is presumed to be a relative URL and thus to start with
   a path component.

    """
    idx_scheme = url.find("://")
    if idx_scheme >=0:
        idx_netloc = url.find(__ORD("/"),idx_scheme+3)
    else:
        idx_netloc = url.find(__ORD("/"))
    idx_query = url.find(__ORD("?"))
    idx_frag = url.find(__ORD("#"))

    if idx_scheme>=0:
        scheme = url[0:idx_scheme]
        idx_scheme+=3
    else:
        scheme = ""
        idx_scheme=0

    if idx_netloc>=0:
        netloc = url[idx_scheme:idx_netloc]        
    else:
        if idx_query<0 and idx_frag<0:
            netloc = url[idx_scheme:]
            idx_netloc = len(url)
        elif idx_query>=0:
            netloc = url[idx_scheme:idx_query]
            idx_netloc = idx_query
        elif idx_frag>=0:
            netloc = url[idx_scheme:idx_frag]
            idx_netloc = idx_frag
        else:
            netloc=""
            idx_netloc = idx_scheme

    if idx_frag>=0:
        fragment=url[idx_frag+1:]
    else:
        fragment=""
        idx_frag = len(url)

    if idx_query>=0:
        query = url[idx_query+1:idx_frag]
    else:
        query = ""        
        idx_query = idx_frag

    path = url[idx_netloc:idx_query]

    return (scheme,netloc,path,query,fragment)


def parse_netloc(netloc):
    """
.. function:: parse_netloc(netloc)

    Given *netloc* as parsed by :func:`.parse`, breaks it in its component returning a tuple (*user*, *password*, *host*, *port*).
    Each component of the returned tuple is a string.

    """
    idx_at = netloc.find(__ORD("@"))
    if idx_at>=0:
        idx2=idx_at+1
        idx_cl = netloc.find(__ORD(":"))
        if idx_cl<0:
            username=""
            password=""
        else:
            username=netloc[0:idx_cl]
            password=netloc[idx_cl+1:idx_at]
    else:
        username = ""
        password = ""
        idx2 = 0
    idx_cl = netloc.find(__ORD(":"),idx2)
    if idx_cl>=0:
        host = netloc[idx2:idx_cl]
        port = netloc[idx_cl+1:]
    else:
        host = netloc[idx2:]
        port =""

    return (username,password,host,port)

safesymbols = bytes("$-_.+!*'()")
safesymbols_noplus = bytes("$-_.!*'()")

def _quote(s, sfsymbols=safesymbols_noplus):
    res=bytearray()
    ss = bytes(s)
    for x in ss:
        if (x>=__ORD("a") and x<=__ORD("z")) or (x>=__ORD("A") and x<=__ORD("Z")) or (x>=__ORD("0") and x<=__ORD("9")) or (x in sfsymbols):
            res.append(x)
        else:
            res.append(__ORD("%"))
            h = hex(x,prefix="")
            res.append(ord(h[0]))
            res.append(ord(h[1]))
    return str(res)

def quote(s):
    """
.. function:: quote(s)

    Return the urlencoded version of *s*.

    Urlencoding transforms unsafe bytes to their %XX representation where XX is the hex value of the byte.

    Safe bytes are:

        * lowercase letters from "a" to "z" (bytes from 0x61 to 0x7a)
        * uppercase letters from "A" to "Z" (bytes from 0x41 to 0x5a)
        * numbers from "0" to "9" (bytes from 0x30 to 0x39)
        * the following symbols: $-_.+!*'()

    """
    return _quote(s, safesymbols)

def quote_plus(s):
    """
.. function:: quote_plus(s)

    Like :func:`quote`, but also escapes :code:`+` symbol.
    """
    return _quote(s)


def unquote(s):
    """
.. function:: unquote(s)

    If *s* is urlencoded, returns *s* with every ``+`` substituted with a space and every ``%xx`` substituted with the corresponding character.

    """
    res = bytearray()
    i=0
    l=len(s)
    while i<l:
        c= s[i]
        if c=="%":
            z = int(s[i+1:i+3],16)
            res.append(z)
            i+=3
            continue
        if c=="+":
            res.append(ord(" "))
        else:
            res.append(ord(c))
        i+=1
    #return res.decode("utf-8")
    return str(res)

def parse_qs(qs, unquote_key = False):
    """
.. function:: parse_qs(qs)

    Parses a query string *qs* and returns a dictionary containing the association between keys and values of *qs*.
    Values are urldecoded by :func:`unquote`.
    
    """
    res = {}
    pairs = qs.split("&")
    for pair in pairs:
        if pair:        
            vals = pair.split("=")
            if unquote_key:
                key = unquote(vals[0])
            else:
                key = vals[0]
            res[key]=unquote(vals[1])
    return res

def urlencode(data, quote_via=quote_plus):
    """
.. function:: urlencode(data)

    Tranforms data dictionary in a urlencoded query string and returns the query string.
    Each pair (key, value) is encoded by *quote_via* function.
    By default, :func:`quote_plus` is used to quote the values.
    An alternate function that can be passed as quote_via is :func:`quote`.
    
    """
    res = ""
    for k,v in data.items():
        res+=quote(str(k))+"="+quote_via(str(v))+"&"
    return res[:-1]

#######################################
# TEST


# urls =[
# "http://www.google.com",
# "http://foo:bar@w1.superman.com/very/long/path.html?p1=v1&p2=v2+%25#more-details",
# "https://secured.com:443",
# "ftp://ftp.bogus.com/~some/path/to/a/file.txt"]


# for url in urls:
#     tt = parse(url)
#     print(url)
#     print("===>",tt)
#     print("===>",parse_netloc(tt[1]))
#     print("===>",parse_qs(tt[-2]))
#     print()

