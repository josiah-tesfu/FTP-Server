# FTP-Server
This code is a stream socket server demo implemented in C. It creates a server that listens for incoming connections and handles basic FTP commands.

Functionality
The server provides the following functionality:

- Socket setup: Sets up a stream socket to listen for incoming connections.
- Handling multiple connections: Accepts multiple client connections using multi-threading.
- Login sequence: Initiates a login sequence by responding with a 220 code.
- Command parsing: Parses FTP commands received from the client.
- Command handling: Implements basic FTP commands such as USER, RETR, QUIT, CWD, CDUP, TYPE, MODE, STRU, PASV, and NLST.
- Directory navigation: Allows changing the current working directory (CWD) and moving up the directory hierarchy (CDUP).
- Passive mode: Implements passive mode (PASV) for data transfers.
- Error handling: Handles various error conditions and sends appropriate response codes.
