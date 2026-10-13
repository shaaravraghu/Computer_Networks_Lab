// SERVER

#include <iostream>
#include <cstring>
#include <thread>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

#define PORT 5000
#define BUFFER_SIZE 1024

void handleClient(int client_fd)
{
    char buffer[BUFFER_SIZE];

    while (true)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            break;
        }

        cout << "Received from client (" << client_fd << "): " << buffer << endl;
        send(client_fd, buffer, n, 0);
    }

    close(client_fd);
}

int server()
{
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        cout << "Socket creation failed" << endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (::bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "Bind failed" << endl;
        close(listen_fd);
        return 1;
    }

    if (listen(listen_fd, 5) < 0)
    {
        cout << "Listen failed" << endl;
        close(listen_fd);
        return 1;
    }

    cout << "Server listening on port " << PORT << "..." << endl;

    while (true)
    {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0)
        {
            continue;
        }

        cout << "Client (" << client_fd << ") connected." << endl;

        thread t(handleClient, client_fd);
        t.detach();
    }

    close(listen_fd);
    return 0;
}

// CLIENT

int client()
{
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        cout << "Socket creation failed" << endl;
        return 1;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        cout << "Connection failed" << endl;
        close(sockfd);
        return 1;
    }

    cout << "Connected to server." << endl;

    char buffer[BUFFER_SIZE];

    while (true)
    {
        string message;
        cout << "Enter message (or 'quit'): ";
        getline(cin, message);

        if (message == "quit")
        {
            break;
        }

        send(sockfd, message.c_str(), message.length(), 0);

        memset(buffer, 0, BUFFER_SIZE);
        int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0)
        {
            cout << "Server disconnected." << endl;
            break;
        }

        buffer[n] = '\0';
        cout << "Server echoed: " << buffer << endl;
    }

    close(sockfd);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc > 1 && strcmp(argv[1], "client") == 0)
    {
        return client();
    }
    return server();
}
