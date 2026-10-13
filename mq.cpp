#include <cstring> // strcpy()
#include <iostream>
#include <sys/ipc.h>   // IPC functions like ftok()
#include <sys/msg.h>   // Message Queue functions
#include <sys/types.h> // System data types
#include <sys/wait.h>  // wait()
#include <unistd.h>    // fork()

using namespace std;

struct Message {
  long mtype;      // Message type
  char mtext[100]; // Actual message
};

int main() {

  key_t key = ftok("progfile", 65);

  if (key == -1) {
    perror("ftok");
    return 1;
  }

  int msgid = msgget(key, 0666 | IPC_CREAT);

  // Check whether msgget() failed
  if (msgid == -1) {
    perror("msgget");
    return 1;
  }

  pid_t pid = fork();

  if (pid < 0) {
    perror("fork");
    msgctl(msgid, IPC_RMID, NULL);
    return 1;
  }

  if (pid > 0) {
    Message msg;

    msg.mtype = 1;

    strcpy(msg.mtext, "KEY");

    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
      perror("msgsnd");

      // Remove message queue if sending fails
      msgctl(msgid, IPC_RMID, NULL);

      return 1;
    }

    // Display what Arjun sent
    cout << "Arjun sent: " << msg.mtext << endl;

    wait(NULL);
    msgctl(msgid, IPC_RMID, NULL);
  }

  else {
    Message msg;

    if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
      perror("msgrcv");

      // Remove queue if receiving fails
      msgctl(msgid, IPC_RMID, NULL);

      return 1;
    }

    // Display the received message
    cout << "Vikram received: " << msg.mtext << endl;
  }

  // Program completed successfully
  return 0;
}