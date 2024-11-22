

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


# RFC:

## Section 4:  Request Methods
```
   +---------+-------------------------------------------------+-------+
   | Method  | Description                                     | Sec.  |
   +---------+-------------------------------------------------+-------+
   | GET     | Transfer a current representation of the target | 4.3.1 |
   |         | resource.                                       |       |
   | POST    | Perform resource-specific processing on the     | 4.3.3 |
   |         | request payload.                                |       |
   | DELETE  | Remove all current representations of the       | 4.3.5 |
   |         | target resource.                                |       |
   +---------+-------------------------------------------------+-------+
```

### GET

   The GET method requests the current representation of a specified resource. It's primarily used for data retrieval and heavily optimized for performance. Although often thought of as fetching a file from a path, GET can serve dynamic content from databases, programs, or external systems. Only the server understands the resource mapping, and it may generate content dynamically rather than directly serving files.

   - Key points:

      1) Sending a body with GET has no defined meaning and may cause errors.

### POST
   The POST method requests the server to process the enclosed data based on its specific logic. It is versatile and used for tasks like submitting forms, posting messages, creating new resources, or appending data.

   - Key points:

      1) The server processes the request and responds with an appropriate status code.
      2) If a new resource is created, the server responds with 201 Created and includes a Location header pointing to the new resource.


### DELETE
   The DELETE method requests the server to remove the association between a resource and its functionality, effectively "deleting" it. However, actual deletion of data depends on the server's implementation.

   - Key points:

      1) DELETE affects the URI mapping, not necessarily the underlying data.
      2) Commonly used in remote authoring or resource management (e.g., undoing actions from PUT or POST).
      3) Typical response: 204 No Content: Action completed without additional information.
      4) DELETE payloads are undefined and may cause issues with some implementations.



