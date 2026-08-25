#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>


mkfifo(const char *pathname, mode_t mode);
key = ftok("progfile", 65); // ftok to generate unique key
msgid = msgget(key, 0666 | IPC_CREAT); // msgget creates a message queue and returns identifier
msgsnd(msgid, &message, sizeof(message), 0); // msgsnd to send message
msgrcv(msgid, &message, sizeof(message), 1, 0); // msgrcv to receive message
msgctl(msgid, IPC_RMID, NULL); // msgctl to destroy the message queue


// Shared Memory
// ftok() — key_t ftok(const char *pathname, int proj_id); generates a unique key.
// shmget() — int shmget(key_t key, size_t size, int shmflg); returns an identifier for the shared memory segment.
// shmat() — void *shmat(int shmid, void *shmaddr, int shmflg); attaches the process to the shared memory segment (pass shmaddr = NULL and the OS chooses the address).
// shmdt() — int shmdt(void *shmaddr); detaches the process from the shared memory segment once it is no longer needed.
// shmctl() — shmctl(int shmid, IPC_RMID, NULL); destroys the shared memory segment (detaching alone does not destroy it).
// semget() — int semget(key_t key, int nsems, int semflg); creates or accesses a semaphore set, used here to protect the shared memory segment.
// semop() — int semop(int semid, struct sembuf *sops, size_t nsops); performs wait/signal operations on the semaphore so that only one process accesses the shared memory segment at a time.
// semctl() — semctl(int semid, int semnum, int cmd, ...); used with IPC_RMID to remove the semaphore set once you are done.

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
