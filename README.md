# WebServ Project

This project is part of the 42 (1337) school curriculum. The goal is to create a simple HTTP web server in C++ that is compliant with a subset of the HTTP/1.1 protocol.

## Table of Contents
- [Introduction](#introduction)
- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Testing](#testing)
- [Contributing](#contributing)

## Introduction
WebServ is a lightweight HTTP server built from scratch in C++. It is designed to handle basic HTTP requests and serve static files. This project aims to deepen understanding of network programming, HTTP protocol, and server-client architecture.

## Features
- HTTP/1.1 compliance
- GET, POST, and DELETE request handling
- Static file serving
- Basic CGI support
- Configurable through a configuration file

## Installation
To install and build the WebServ project, follow these steps:

1. Clone the repository:
   ```sh
   git clone https://github.com/7h3Y055/WebServ.git
   ```
2. Navigate to the project directory:
   ```sh
   cd WebServ
   ```
3. Build the project:
   ```sh
   make
   ```

## Usage
To start the server, run the following command:
```sh
./webserv [configuration_file]
```
If no configuration file is provided, the server will use the default settings.

## Configuration
The server can be configured using a configuration file. Below is an example configuration:
```conf
SERVER{
	
    port = 8080
    host=127.0.0.1
    server_names=webserv
    root=./www
    error_page=404 error_pages/404.html
    client_max_body_size=   10G


    location / {
        root=./www/cgi-bin/
        index=README.html
    }

    location /cgi-bin {
        methods=GET, POST, DELETE
        directory_listing=ON
        cgi=.py: /usr/bin/python3
    }

}SERVER
```

## Testing
To test the server, you can use tools like `curl` or `Postman` to send HTTP requests and verify the responses. Additionally, you can open a web browser and navigate to `http://localhost:8080` to see the server in action.

## Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your changes. Make sure to follow the coding standards and include tests for any new features or bug fixes.

