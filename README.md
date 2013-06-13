# Meerkat

## Meerkat Protocol

Meerkat protocol is very simular with Redis protocol to focus same goal as below.

* Simple to implement
* Fast to parse by a computer
* Easy enough to parse by a human

### Networking Layer

A client connects to Meerkat server via TCP connection and PORT 7758 basically. And every commands and pareters are seperated <CR><LF>(\r\n).

### Message transmission

All most of messages send from client side to Meerkat server (Request), and then server returns to each clients (Response). When client establishs connection with server, client receives a response message from server without request message.

### Message format

Meerkat protocol dosen't have any binary data, ascii data only.

#### Request message

#### Response message

#### Error message

### Commands

#### Request 

#### Response
