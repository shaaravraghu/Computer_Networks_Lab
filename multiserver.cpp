#include <cstdlib>  // General C functions
#include <cstring>  // memset(), strcmp()
#include <iostream> // cout, endl
#include <map>      // map for storing auction information
#include <mutex>    // mutex for synchronization
#include <sstream>  // stringstream for command parsing
#include <thread>   // thread support
#include <vector>   // vector for storing client sockets

#include <netinet/in.h> // sockaddr_in, AF_INET
#include <sys/socket.h> // socket(), bind(), listen(), accept(), send(), recv()
#include <sys/types.h>  // Basic system data types
#include <unistd.h>     // close()

using namespace std;

// ==================================================
// CONSTANTS
// ==================================================

#define PORT 5000
// Server will run on port 5000

#define MAX_CLIENTS 5
// Server will support maximum 5 clients

#define BUFFER_SIZE 1024
// Buffer can store up to 1024 bytes

// ==================================================
// AUCTION STRUCTURE
// ==================================================

// This structure stores information about
// the current highest bid for one item.

struct Auction {
  int highestBid;
  int highestBidder;
};

// Example:
//
// laptop -> highestBid = 150
//           highestBidder = 2
//
// means Client 2 currently has the highest
// bid of 150 for laptop.

// ==================================================
// GLOBAL VARIABLES
// ==================================================

// Store socket descriptors of all connected clients.
//
// Example:
//
// clients = {4, 5, 6}
//
// 4 -> Client 1
// 5 -> Client 2
// 6 -> Client 3

vector<int> clients;

// Store auction information for every item.
//
// Example:
//
// auctions["laptop"]
// auctions["phone"]
// auctions["car"]

map<string, Auction> auctions;

// Mutex used to protect shared data.
//
// Multiple client threads can access:
//
// clients
// auctions
//
// at the same time.
//
// mutex prevents two threads from modifying
// the same data simultaneously.

mutex mtx;

// ==================================================
// FUNCTION: BROADCAST MESSAGE
// ==================================================

// This function sends a message to ALL
// currently connected clients.
//
// sender_fd is optional.
// If sender_fd is given, we can exclude
// that client from the broadcast.
//
// Here we will send to everyone.

void broadcastMessage(string message) {
  // Lock shared client list.

  lock_guard<mutex> lock(mtx);

  // Go through every connected client.

  for (int client_fd : clients) {
    // Send the message to this client.

    send(client_fd, message.c_str(), message.length(), 0);
  }
}

// ==================================================
// FUNCTION: REMOVE CLIENT
// ==================================================

void removeClient(int client_fd) {
  // Lock the clients vector.

  lock_guard<mutex> lock(mtx);

  // Search for this client's socket.

  for (auto it = clients.begin(); it != clients.end(); ++it) {
    if (*it == client_fd) {
      // Remove the client from vector.

      clients.erase(it);

      break;
    }
  }
}

// ==================================================
// FUNCTION TO HANDLE ONE CLIENT
// ==================================================

void handleClient(int client_fd, int clientNumber) {
  // Buffer used to receive commands
  // from this client.

  char buffer[BUFFER_SIZE];

  cout << "Client " << clientNumber << " connected." << endl;

  // ==================================================
  // ADD CLIENT TO CLIENT LIST
  // ==================================================

  {
    // Lock the shared vector.

    lock_guard<mutex> lock(mtx);

    // Store this client's socket.

    clients.push_back(client_fd);
  }

  // ==================================================
  // SEND WELCOME MESSAGE
  // ==================================================

  string welcome = "Welcome to the Auction Server!\n"
                   "Client number: " +
                   to_string(clientNumber) +
                   "\n"
                   "Use: ./item <item_name> <bid_amount>\n"
                   "Use: ./quit to disconnect.\n";

  send(client_fd, welcome.c_str(), welcome.length(), 0);

  while (true) {
    // Clear receive buffer.

    memset(buffer, 0, BUFFER_SIZE);

    // ==================================================
    // RECEIVE COMMAND FROM CLIENT
    // ==================================================

    int n = recv(client_fd, buffer, BUFFER_SIZE - 1, 0);

    // If client disconnected or error occurred.

    if (n <= 0) {
      cout << "Client " << clientNumber << " disconnected." << endl;

      break;
    }

    // Add string termination character.

    buffer[n] = '\0';

    // Convert received data into a string.

    string command(buffer);

    // Remove trailing newline if present.

    if (!command.empty() && command.back() == '\n') {
      command.pop_back();
    }

    cout << "Client " << clientNumber << ": " << command << endl;

    // ==================================================
    // CHECK FOR ./quit
    // ==================================================

    if (command == "./quit") {
      string goodbye = "Goodbye!\n";

      // Send goodbye only to this client.

      send(client_fd, goodbye.c_str(), goodbye.length(), 0);

      // Stop handling this client.

      break;
    }

    // ==================================================
    // PARSE COMMAND
    // ==================================================

    // We expect:

    // ./item laptop 100
    //
    // command  = ./item
    // itemName = laptop
    // bid      = 100

    stringstream ss(command);

    string operation;
    string itemName;
    int bid;

    // Extract the three parts.

    ss >> operation >> itemName >> bid;

    // ==================================================
    // CHECK WHETHER COMMAND IS ./item
    // ==================================================

    if (operation != "./item") {
      string error = "Invalid command.\n"
                     "Use: ./item <item_name> <bid_amount>\n";

      send(client_fd, error.c_str(), error.length(), 0);

      continue;
    }

    // ==================================================
    // VALIDATE BID
    // ==================================================

    // Check whether bid is positive.

    if (bid <= 0) {
      string error = "Bid must be greater than 0.\n";

      send(client_fd, error.c_str(), error.length(), 0);

      continue;
    }

    // ==================================================
    // UPDATE AUCTION
    // ==================================================

    // Lock the auction data.
    //
    // This is important because multiple clients
    // may place bids at the same time.

    lock_guard<mutex> lock(mtx);

    // --------------------------------------------------
    // CHECK WHETHER ITEM ALREADY EXISTS
    // --------------------------------------------------

    if (auctions.find(itemName) == auctions.end()) {
      // This is the first bid for this item.

      auctions[itemName].highestBid = bid;

      auctions[itemName].highestBidder = clientNumber;

      // Create broadcast message.

      string message = "Client " + to_string(clientNumber) +
                       " placed a bid of " + to_string(bid) + " for " +
                       itemName + ".\n" +
                       "Current highest bid: " + to_string(bid) +
                       " by Client " + to_string(clientNumber) + ".\n";

      // Send to every client.

      for (int fd : clients) {
        send(fd, message.c_str(), message.length(), 0);
      }
    }

    // --------------------------------------------------
    // ITEM ALREADY EXISTS
    // --------------------------------------------------

    else {
      // Get current highest bid.

      int currentBid = auctions[itemName].highestBid;

      int currentBidder = auctions[itemName].highestBidder;

      // ==================================================
      // NEW BID IS HIGHER
      // ==================================================

      if (bid > currentBid) {
        // Update highest bid.

        auctions[itemName].highestBid = bid;

        // Update highest bidder.

        auctions[itemName].highestBidder = clientNumber;

        // Create message.

        string message = "Client " + to_string(clientNumber) +
                         " placed a bid of " + to_string(bid) + " for " +
                         itemName + ".\n" +
                         "New highest bid: " + to_string(bid) + " by Client " +
                         to_string(clientNumber) + ".\n";

        // Broadcast to all clients.

        for (int fd : clients) {
          send(fd, message.c_str(), message.length(), 0);
        }
      }

      // ==================================================
      // NEW BID IS NOT HIGHER
      // ==================================================

      else {
        // Bid is rejected.

        string message = "Bid rejected for " + itemName + ".\n" +
                         "Current highest bid: " + to_string(currentBid) +
                         " by Client " + to_string(currentBidder) + ".\n";

        // Send rejection only to the
        // client who placed the invalid bid.

        send(client_fd, message.c_str(), message.length(), 0);
      }
    }
  }

  // ==================================================
  // REMOVE CLIENT
  // ==================================================

  removeClient(client_fd);

  // ==================================================
  // CLOSE CLIENT SOCKET
  // ==================================================

  close(client_fd);

  cout << "Connection closed for Client " << clientNumber << "." << endl;
}

// ==================================================
// MAIN SERVER PROGRAM
// ==================================================

int main() {
  // ==================================================
  // SERVER SOCKET
  // ==================================================

  int listen_fd;

  // Server address structure.

  struct sockaddr_in server_addr;

  // Client address structure.

  struct sockaddr_in client_addr;

  // Size of client address.

  socklen_t client_len;

  // ==================================================
  // 1. CREATE SOCKET
  // ==================================================

  listen_fd = socket(AF_INET, SOCK_STREAM, 0);

  if (listen_fd < 0) {
    cout << "Socket creation failed" << endl;

    return 1;
  }

  cout << "Socket created." << endl;

  // ==================================================
  // 2. SERVER ADDRESS
  // ==================================================

  memset(&server_addr, 0, sizeof(server_addr));

  // IPv4.

  server_addr.sin_family = AF_INET;

  // Accept connections on any local IP.

  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Server port.

  server_addr.sin_port = htons(PORT);

  // ==================================================
  // 3. BIND
  // ==================================================

  if (::bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    cout << "Bind failed" << endl;

    close(listen_fd);

    return 1;
  }

  cout << "Bind successful." << endl;

  // ==================================================
  // 4. LISTEN
  // ==================================================

  if (listen(listen_fd, MAX_CLIENTS) < 0) {
    cout << "Listen failed" << endl;

    close(listen_fd);

    return 1;
  }

  cout << "Auction server waiting for clients..." << endl;

  // ==================================================
  // 5. ACCEPT CLIENTS
  // ==================================================

  int clientCount = 0;

  while (clientCount < MAX_CLIENTS) {
    // Set client address size.

    client_len = sizeof(client_addr);

    // Accept a client.

    int new_client =
        accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

    if (new_client < 0) {
      cout << "Accept failed" << endl;

      continue;
    }

    // Increase client count.

    clientCount++;

    cout << "Client " << clientCount << " accepted." << endl;

    // ==================================================
    // 6. CREATE THREAD
    // ==================================================

    // Each client gets its own thread.

    thread clientThread(handleClient, new_client, clientCount);

    // Allow the thread to run independently.

    clientThread.detach();
  }

  // ==================================================
  // MAXIMUM CLIENTS REACHED
  // ==================================================

  cout << "Maximum clients reached." << endl;

  // ==================================================
  // CLOSE SERVER SOCKET
  // ==================================================

  close(listen_fd);

  return 0;
}