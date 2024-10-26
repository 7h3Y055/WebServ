# NOTES:
1) Limit client body size.
2) We will consider that NGINX is HTTP 1.1 compliant and may be used to compare headers and answer behaviors
3) Just remember that, for chunked request, your server needs to unchunk it, the CGI will expect EOF as end of the body.
