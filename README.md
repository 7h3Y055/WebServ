# NOTES: ↻ 
1) check directories in Config file     DONE
2) remove temp_ in name new resource    DONE
3) check CGI                            DONE
4) Problem in chunked mechanisms        DONE
5) redirection                          DONE
6) check config                         DONE
7) check server_names                   DONE
8) time out                             NTME
9) leak FD                              LAST
10) memory leak                         LAST
11) catch http error codes              DONE
13) add Date: Header                    DONE
14) Redirection                         DONE
15) remove temprary file wen time out



12) http://x.com/index.php/var?q=1      mustapha






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



