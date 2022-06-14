# HTTP 1.0

Response Line (Status Line) :
HTTP version | Status code | English reason phrase

- status code :
1xx indicates an informational message only
2xx indicates success of some kind
3xx redirects the client to another URL
4xx indicates an error on the client's part
5xx indicates an error on the server's part

- most common ones :
200 OK : The request succeeded, and the resulting resource (e.g. file or script output) is returned in the message body.
404 Not Found : The requested resource doesn't exist.
301 Moved Permanently
302 Moved Temporarily
303 See Other (HTTP 1.1 only) : The resource has moved to another URL (given by the Location: response header), and should be automatically retrieved by the client. This is often used by a CGI script to redirect the browser to an existing file.
500 Server Error : An unexpected server error. The most common cause is a server-side script that has bad syntax, fails, or otherwise can't run correctly.

Header Lines
- Server: header is analogous to the User-Agent: header: it identifies the server software in the form "Program-name/x.xx". Example : "Server: Apache/1.2b3-dev".
- The Last-Modified: header gives the modification date of the resource that's being returned. It's used in caching and other bandwidth-saving activities. Use Greenwich Mean Time, in the format.
Example : Last-Modified: Fri, 31 Dec 1999 23:59:59 GMT

The Message Body

Header lines in the message that describe the body :

The Content-Type: header gives the MIME-type of the data in the body, such as text/html or image/gif.
The Content-Length: header gives the number of bytes in the body.

The POST Method

Contains additionnaly a body | content-type | content-length

# HTTP 1.1

- request has to contain the "host" information, otherwise return "400 Bad Request"
- accepting absolute URL
- chunked transfer-encoding
- if a client sends multiple requests through a single connection, the server should response in the same order.
The server has to support persistent connections.
If a request contains "Connection: close" -> last request, server has to close the connection after responding
- "100 Continue"
- Date Header for caching
- ...

https://www.jmarshall.com/