# Meerkat

## Meerkat Protocol

Meerkat protocol is very simular with Redis protocol to focus same goal as below.

* Simple to implement
* Fast to parse by a computer
* Easy enough to parse by a human

### Networking Layer

A client connects to Meerkat server via TCP connection and PORT 7758 basically. And every commands and pareters are seperated by <CR><LF>(\r\n).

### Message transmission

All most of messages send from client side to Meerkat server (Request), and then server returns to each clients (Response). When client establishs connection with server, client receives a response message from server without request message once.

    +WELCOME<CR><LF>
    
Most of the time, a client receives WELCOME response. Only when server reaches number of connections to MAX_CONNECTIONS, then client will be received as below ERROR message as 

    +ERROR
    989
    Server's having too many connections. (MAX_CONNECTIONS:XXXXX)

Please see [Error Messsages](https://github.com/jinoos/meerkat/)

### Message format

Meerkat protocol dosen't have any binary data, ascii data only. A message has a command and parameter(s). 

Mentioned in above, a request message come from a client base on below request message format,

    *COMMAND<CR><LF>
    PARAMETER1<CR><LF>
    PARAMETER2<CR><LF>
    PARAMETER3<CR><LF>

First of all, every request commands have to start Request Command Prefix, "*". If not, will be ignored.

A response message also has that kind of prefix "+", so response message format as below,

    +COMMAND<CR><LF>
    PARAMETER1<CR><LF>
    PARAMETER2<CR><LF>

#### Array format

Some meesages require to contain array type data, that will be included a number what number of array data. See below example

    +NODES<CR><LF>
    3<CR><LF>                   # <---  
    192.168.0.1:1234<CR><LF>
    192.168.0.1:1234<CR><LF>
    192.168.0.1:1234<CR><LF>

It is a response message, but when request message need to have array data, then will be also same format.

You can see detail format for each commands 

#### Error message

