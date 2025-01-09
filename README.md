# DS-Assignment-2
Direct Message Chat System

A C++ implementation of a multi-threaded chat server and client that supports direct messaging between users. The system uses Windows Sockets (Winsock) for network communication and implements thread-safe message handling.

1. Features
      Real-time Direct Messaging: Users can send private messages to specific users
      Multi-threaded Architecture: Handles multiple client connections simultaneously
      Persistent Chat History: Logs all chat messages to a file
      User Management: Tracks active users and their connections
      Clean Disconnect Handling: Properly manages user disconnections
      Simple Client Interface: Easy-to-use command-line client

2. System Requirements
      Windows operating system
      C++11 or later
      Winsock2 library
      Compiler with threading support

3. Project Structure
   1) Server Component
      Handles multiple client connections
      Manages user sessions
      Routes direct messages
      Logs chat history
      Broadcasts user connection/disconnection events

  2) Client Component
      Connects to the server
      Sends/receives messages
      Supports direct messaging format
      Handles disconnection gracefully

4. Setup and Compilation
  1) Install Required Tools
     Ensure you have a C++ compiler with C++11 support
     Visual Studio or MinGW-w64 recommended

  2) Compile the Server
     g++ server.cpp -o server.exe -lws2_32 -std=c++11

  3) Compile the Client
     g++ client.cpp -o client.exe -lws2_32 -std=c++11
