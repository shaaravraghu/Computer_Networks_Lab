#include <stdio.h> // Provides printf(), fprintf(), fgets(), perror(), stdin and stderr
#include <sys/types.h> // Defines system data types used in socket programming
#include <sys/socket.h> // Provides socket(), connect(), send() and socket constants
#include <netinet/in.h> // Provides sockaddr_in, AF_INET and htons()
#include <netdb.h> // Provides gethostbyname() and struct hostent

#define SERVER_PORT 5432 // Port number on which the server is running
#define MAX_LINE 256 // Maximum number of characters that can be read at a time
