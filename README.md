SIMPLE MULTITHREADED CROSSPLATFORM LOG SERVER

This is just my simple client-server project and no more... )

Server uses multithreading and interthread sync.
Threads interacts with each other using queues and messages.

Description

1. Clients send simple periodic messages to the server.
2. Messages contains some text with timestamp at the end of each one.
3. The server listen for multiple client requests in its main thread.
4. For each request recieved from a client the server creates separate thread to proccess client's request.
5. On the server all text messages recieved from clients are written into a file.
6. On the server client's threads uses queue to write text messages into a file.

The server listens port 3000.

How to launch?

1. Launch server (on Windows or FreeBSD or Linux)
2. Launch clients. You must send to client folowing cmd line:

Client cmd line example:

Client.exe 'Server address' 'Client name' 'Server port' 'Timeout'

'Timeout' - is message sending time period in milliseconds.

Example: C:\Client.exe 192.168.100.1 Client1 3000 5000

Tested on Win 11 and FreeBSD 14.0

