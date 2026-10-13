#include <iostream>      // cout, cin, getline
#include <cstring>       // memset()
#include <thread>        // thread support

#include <sys/types.h>   // Basic system data types
#include <sys/socket.h>  // socket(), connect(), send(), recv()
#include <netinet/in.h>  // sockaddr_in, AF_INET
#include <unistd.h>      // close()
#include <arpa/inet.h>   // inet_pton()

using namespace std;


// ==================================================
// CONSTANTS
// ==================================================

#define PORT 5000
// Server is running on port 5000

#define BUFFER_SIZE 1024
// Buffer used to receive messages from server



// ==================================================
// FUNCTION TO RECEIVE SERVER MESSAGES
// ==================================================

// This function runs in a separate thread.
//
// Its job is to continuously wait for messages
// from the server.
//
// This is important because the server can
// broadcast an auction update at any time.

void receiveMessages(int sockfd)
{
    // Buffer for receiving server messages.

    char buffer[BUFFER_SIZE];


    // ==================================================
    // CONTINUOUSLY RECEIVE SERVER MESSAGES
    // ==================================================

    while (true)
    {
        // Clear buffer.

        memset(buffer,
               0,
               BUFFER_SIZE);


        // Receive message from server.

        int n = recv(sockfd,
                     buffer,
                     BUFFER_SIZE - 1,
                     0);


        // If server disconnected
        // or an error occurred.

        if (n <= 0)
        {
            cout << "\nServer disconnected."
                 << endl;

            break;
        }


        // Add '\0' at the end.

        buffer[n] = '\0';


        // Display server message.

        cout << "\n-----------------------------"
             << endl;

        cout << buffer;

        cout << "-----------------------------"
             << endl;


        // Show input prompt again.

        cout << "Enter command: ";
    }
}



// ==================================================
// MAIN CLIENT PROGRAM
// ==================================================

int main()
{
    // ==================================================
    // SOCKET DESCRIPTOR
    // ==================================================

    // Socket used for communicating
    // with the auction server.

    int sockfd;


    // ==================================================
    // SERVER ADDRESS
    // ==================================================

    struct sockaddr_in server_addr;


    // ==================================================
    // 1. CREATE SOCKET
    // ==================================================

    // Create IPv4 TCP socket.

    sockfd = socket(AF_INET,
                    SOCK_STREAM,
                    0);


    // Check socket creation.

    if (sockfd < 0)
    {
        cout << "Socket creation failed"
             << endl;

        return 1;
    }


    cout << "Socket created."
         << endl;



    // ==================================================
    // 2. SERVER ADDRESS
    // ==================================================

    // Clear server address structure.

    memset(&server_addr,
           0,
           sizeof(server_addr));


    // IPv4.

    server_addr.sin_family =
        AF_INET;


    // Server port.

    server_addr.sin_port =
        htons(PORT);


    // Server IP address.
    //
    // 127.0.0.1 means the server
    // is running on this same computer.

    inet_pton(AF_INET,
              "127.0.0.1",
              &server_addr.sin_addr);



    // ==================================================
    // 3. CONNECT TO SERVER
    // ==================================================

    if (connect(sockfd,
                (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        cout << "Connection failed"
             << endl;

        close(sockfd);

        return 1;
    }


    cout << "Connected to auction server."
         << endl;



    // ==================================================
    // 4. CREATE RECEIVER THREAD
    // ==================================================

    // Start a separate thread that continuously
    // receives messages from the server.
    //
    // This allows the client to receive auction
    // broadcasts even while the user is not
    // entering a command.

    thread receiver(
        receiveMessages,
        sockfd
    );


    // Detach receiver thread.

    receiver.detach();



    // ==================================================
    // 5. SEND COMMANDS TO SERVER
    // ==================================================

    while (true)
    {
        // String containing the command.

        string command;


        // Ask user for command.

        cout << "\nEnter command: ";


        // Read complete command.

        getline(cin, command);


        // ==================================================
        // SEND COMMAND
        // ==================================================

        // Send command to server.

        send(sockfd,
             command.c_str(),
             command.length(),
             0);



        // ==================================================
        // CHECK FOR ./quit
        // ==================================================

        if (command == "./quit")
        {
            // Client is leaving the auction.

            break;
        }
    }



    // ==================================================
    // CLOSE SOCKET
    // ==================================================

    close(sockfd);


    return 0;
}