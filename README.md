# NOTES:
1) Limit client body size.
2) We will consider that NGINX is HTTP 1.1 compliant and may be used to compare headers and answer behaviors
3) Just remember that, for chunked request, your server needs to unchunk it, the CGI will expect EOF as end of the body.
4) if not a Content-Length or Transfer-Encoding: chunked the response is 400




# HTTP message body transfer mechanisms
- There a two main mechanisms
1) Fixed-length
2) Chunked


# Example for "Chunked transfer"

HTTP/1.1 200 OK
Transfer-Encoding: chunked

5
Hello
7
 World
0

# Example for "Simple transfer"

HTTP/1.1 200 OK
Content-Length: 11

Hello World