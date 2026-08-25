#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>


mkfifo(const char *pathname, mode_t mode);
key = ftok("progfile", 65); // ftok to generate unique key
msgid = msgget(key, 0666 | IPC_CREAT); // msgget creates a message queue and returns identifier
msgsnd(msgid, &message, sizeof(message), 0); // msgsnd to send message
msgrcv(msgid, &message, sizeof(message), 1, 0); // msgrcv to receive message
msgctl(msgid, IPC_RMID, NULL); // msgctl to destroy the message queue

// Functions for fork() in C/ C++
pid_t fork(void);
pid_t pid = fork();
  if (pid == 0) {
  // child process — e.g. Vikram
} else if (pid > 0) {
  // parent process — e.g. Arjun
} else {
  perror("fork failed");
}
