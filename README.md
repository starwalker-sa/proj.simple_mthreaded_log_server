# The simplest multithreaded crossplatform log server

This is just my simple client-server project and no more... )

Server uses multithreading and interthread sync.
Threads interacts with each other using queues and messages.

# Description

1. Client hosts send simple periodic text messages to the server.
2. The messages contains client name text string with timestamp at the end of each one.
3. The server listens for multiple client requests in its main thread. This thread is only for listening.
4. Also the server has single logger object running in separate thread and pulling queue messages coming from client's threads on the server. 
5. For each request recieved from a client host the server creates separate thread to proccess client's message.
6. After processing the request client's thread pushes it into the logger thread queue.
7. Logger thread owning opened file handle grabes and writes client message to this file.

The server listens port 3000.

# How to launch?

1. Launch server (on Windows or FreeBSD or Linux)
2. Launch clients. You must send to client folowing cmd line:

Client cmd line example:

Client.exe 'Server address' 'Client name' 'Server port' 'Timeout'

'Timeout' - is message sending time period in milliseconds.

Example: C:\Client.exe 192.168.100.1 Client1 3000 5000

Tested on Win 11 and FreeBSD 14.0

