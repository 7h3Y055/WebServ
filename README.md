# NOTES:
1) check directories in Config file     DONE
2) remove temp_ in name new resource
3) check CGI                            DONE
4) Problem in chunked mechanisms        DONE
5) redirection                           ↻  
6) check config                         DONE
7) check server_names
8) time out
9) leak FD
10) memory leak
11) catch http error codes
12) create response chunked
13) https://example.com/index.php/var
14) add Date in Response
15) wen not a auto index return 403








15) redirect accept all http codes from 0 to 999
    301 Moved Permanently – The resource has been permanently moved to a new URL.
    302 Found – The resource temporarily resides under a different URL.
    303 See Other – Directs the client to retrieve the resource using a GET request, typically after a POST.
    304 Not Modified – Used for caching; indicates the resource hasn’t changed since the last request.
    307 Temporary Redirect – Similar to 302 but explicitly defines that the method should not change.
    308 Permanent Redirect – Similar to 301 but retains the original request method.


    301, 302, 303, 304, 307 , 308



# HTTP Message Body Transfer Mechanisms

There are tree main mechanisms for transferring HTTP message bodies:

## 1. Fixed-length Transfer

Uses `Content-Length` header to specify exact message size.

### Example
```http
HTTP/1.1 200 OK
Content-Length: 11
Hello World
```



## 2. Chunked Transfer
Uses "Transfer-Encoding: chunked" to send data in chunks (the length in hex).
### Example
```HTTP/1.1 200 OK
Transfer-Encoding: chunked
5
Hello
7
 World
0
```
## 3. Multiparts(boundaries)
But not is mandatory



