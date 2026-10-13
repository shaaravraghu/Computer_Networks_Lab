#include <cstdlib>
#include <cstring>
#include <iostream>
#include <thread>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define PORT 5000
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024

void handleClient(int client_fd, int clientNumber) {
  char buffer[BUFFER_SIZE];
  char serverMessage[BUFFER_SIZE];

  cout << "Client " << clientNumber << " connected." << endl;

  string welcome = "Connected to server.\n";

  send(client_fd, welcome.c_str(), welcome.length(), 0);

  while (true) {
    memset(buffer, 0, BUFFER_SIZE);

    int n = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    if (n <= 0) {
      cout << "Client " << clientNumber << " disconnected." << endl;

      break;
    }

    buffer[n] = '\0';

    cout << "Client " << clientNumber << ": " << buffer << endl;

    if (strcmp(buffer, "./quit") == 0 || strcmp(buffer, "./quit\n") == 0) {
      string message = "Goodbye!\n";

      send(client_fd, message.c_str(), message.length(), 0);

      break;
    }

    cout << "Server -> Client " << clientNumber << ": ";

    cin.getline(serverMessage, BUFFER_SIZE);

    send(client_fd, serverMessage, strlen(serverMessage), 0);
  }

  close(client_fd);
}

int main() {
  int listen_fd;

  int client_fd[MAX_CLIENTS];

  struct sockaddr_in server_addr;

  struct sockaddr_in client_addr;

  socklen_t client_len;

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (listen_fd < 0) {
    cout << "Socket creation failed" << endl;

    return 1;
  }

  cout << "Socket created." << endl;

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (::bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    cout << "Bind failed" << endl;

    close(listen_fd);

    return 1;
  }

  cout << "Bind successful." << endl;

  if (listen(listen_fd, MAX_CLIENTS) < 0) {
    cout << "Listen failed" << endl;

    close(listen_fd);

    return 1;
  }

  cout << "Server waiting for clients..." << endl;

  int clientCount = 0;

  while (clientCount < MAX_CLIENTS) {

    client_len = sizeof(client_addr);

    int new_client =
        accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

    if (new_client < 0) {
      cout << "Accept failed" << endl;

      continue;
    }

    clientCount++;

    cout << "Client " << clientCount << " accepted." << endl;

    thread clientThread(handleClient, new_client, clientCount);

    clientThread.detach();
  }

  cout << "Maximum clients reached." << endl;

  close(listen_fd);
  return 0;
}