// CLIENT CODE

#include <stdio.h> // Provides printf(), fprintf(), fgets(), perror(), stdin and stderr
#include <sys/types.h> // Defines system data types used in socket programming
#include <sys/socket.h> // Provides socket(), connect(), send() and socket constants
#include <netinet/in.h> // Provides sockaddr_in, AF_INET and htons()
#include <netdb.h> // Provides gethostbyname() and struct hostent

#define SERVER_PORT 5432 // Port number on which the server is running
#define MAX_LINE 256 // Maximum number of characters that can be read at a time

int main(int argc, char *argv[]){ // Main function receives command-line arguments

  struct hostent *hp; // Stores information about the server host
  struct sockaddr_in sin; // Stores the server's IP address and port number
  char *host; // Stores the server hostname
  char buf[MAX_LINE]; // Buffer used to store text entered by the user
  int s; // Socket descriptor
  int len; // Number of bytes to be sent

  /*
  * Check whether the user has supplied exactly one argument:
  * the hostname or IP address of the server.
  */
  
  if (argc == 2){
  host = argv[1]; // Store the server hostname given on the command line
  } else {
  // Display the correct format for running the client program
  fprintf(stderr, "Usage: simplex-talk host\n");
  exit(1); // Terminate the program because the argument is missing
  }

  /*
  * Convert the server hostname into its corresponding IP address.
  */

  hp = gethostbyname(host); // Obtain host information using the hostname
  if (hp == NULL){ // Check whether hostname resolution failed
  // Display an error message when the hostname cannot be found
  fprintf(stderr, "simplex-talk: unknown host: %s\n", host);
  exit(1); // Terminate the program
  }

  /*
  * Build and initialize the server-address structure.
  */

  bzero((char *)&sin, sizeof(sin)); // Set all bytes of the address structure to zero
  sin.sin_family = AF_INET; // Specify that an IPv4 address is being used

  /*
  * Copy the server IP address obtained from gethostbyname()
  * into the sockaddr_in structure.
  */

  bcopy( hp->h_addr, // Source: resolved server IP address
  (char *)&sin.sin_addr, // Destination: IP-address field of sin
  hp->h_length // Number of address bytes to copy
  );

  /*
  * Store the server port number.
  * htons() converts the port from host byte order
  * to network byte order.
  */

  sin.sin_port = htons(SERVER_PORT);
