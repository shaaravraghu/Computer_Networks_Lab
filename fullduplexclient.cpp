#include <cstring>  // memset(), strlen()
#include <iostream> // cout, cin, getline

#include <arpa/inet.h>  // inet_pton()
#include <netinet/in.h> // sockaddr_in, AF_INET
#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <sys/types.h>  // Basic system data types
#include <unistd.h>     // close()

using namespace std;

#define PORT 5000
#define BUFFER_SIZE 1024

int main() {

  int sockfd;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];

  sockfd = socket(AF_INET, SOCK_STREAM, 0);

  if (sockfd < 0) {
    cout << "Socket creation failed" << endl;
    return 1;
  }
  cout << "Socket created." << endl;

  memset(&server_addr, 0, sizeof(server_addr));

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    cout << "Connection failed" << endl;
    close(sockfd);
    return 1;
  }
  cout << "Connected to server." << endl;

  memset(buffer, 0, BUFFER_SIZE);

  int n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

  if (n > 0) {
    buffer[n] = '\0';
    cout << buffer;
  }

  while (true) {
    string message;
    cout << "\nEnter message: ";
    getline(cin, message);

    send(sockfd, message.c_str(), message.length(), 0);

    if (message == "./quit") {

      memset(buffer, 0, BUFFER_SIZE);

      n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

      if (n > 0) {
        buffer[n] = '\0';
        cout << buffer;
      }

      break;
    }

    memset(buffer, 0, BUFFER_SIZE);

    n = recv(sockfd, buffer, BUFFER_SIZE - 1, 0);

    if (n <= 0) {
      cout << "Server disconnected." << endl;

      break;
    }

    buffer[n] = '\0';
    cout << "Server: " << buffer << endl;
  }

  close(sockfd);
  return 0;
}