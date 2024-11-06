# NOTES:
1) check the directories if exist in config file
2) create chunked with a specific size
3) if client close connection before the request complate remove the temprary file (buffer file in /tmp)
4) 
5) 




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


