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






















































#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/sem.h>

/* ---------------- MESSAGE QUEUE STRUCTURE ---------------- */

struct message
{
    long msg_type;
    char msg_text[100];
};

/* ---------------- SEMAPHORE UNION ---------------- */

union semun
{
    int val;
};

/* ---------------- MAIN FUNCTION ---------------- */

int main()
{
    /* ---------- VARIABLES ---------- */

    int pipe_fd[2];

    pid_t pid;

    key_t key;

    int msgid;
    int shmid;
    int semid;

    char *shared_memory;

    struct message msg;

    struct sembuf wait_operation;
    struct sembuf signal_operation;

    union semun sem_union;

    char buffer[100];


    /* =====================================================
       CREATE IPC OBJECTS
       ===================================================== */


    /* ---------- 1. CREATE PIPE ---------- */

    if (pipe(pipe_fd) == -1)
    {
        perror("Pipe creation failed");
        exit(1);
    }


    /* ---------- 2. CREATE FIFO ---------- */

    mkfifo("myfifo", 0666);


    /* ---------- 3. GENERATE UNIQUE KEY ---------- */

    key = ftok("progfile", 65);

    if (key == -1)
    {
        perror("ftok failed");
        exit(1);
    }


    /* ---------- 4. CREATE MESSAGE QUEUE ---------- */

    msgid = msgget(
        key,
        0666 | IPC_CREAT
    );

    if (msgid == -1)
    {
        perror("msgget failed");
        exit(1);
    }


    /* ---------- 5. CREATE SHARED MEMORY ---------- */

    shmid = shmget(
        key,
        1024,
        0666 | IPC_CREAT
    );

    if (shmid == -1)
    {
        perror("shmget failed");
        exit(1);
    }


    /* ---------- 6. ATTACH SHARED MEMORY ---------- */

    shared_memory = (char *)shmat(
        shmid,
        NULL,
        0
    );

    if (shared_memory == (char *)-1)
    {
        perror("shmat failed");
        exit(1);
    }


    /* ---------- 7. CREATE SEMAPHORE ---------- */

    semid = semget(
        key,
        1,
        0666 | IPC_CREAT
    );

    if (semid == -1)
    {
        perror("semget failed");
        exit(1);
    }


    /* ---------- 8. INITIALIZE SEMAPHORE ---------- */

    sem_union.val = 1;

    semctl(
        semid,
        0,
        SETVAL,
        sem_union
    );


    /* ---------- WAIT OPERATION ---------- */

    wait_operation.sem_num = 0;
    wait_operation.sem_op = -1;
    wait_operation.sem_flg = 0;


    /* ---------- SIGNAL OPERATION ---------- */

    signal_operation.sem_num = 0;
    signal_operation.sem_op = 1;
    signal_operation.sem_flg = 0;


    /* =====================================================
       CREATE CHILD PROCESS
       ===================================================== */

    pid = fork();


    /* =====================================================
       CHILD PROCESS
       ===================================================== */

    if (pid == 0)
    {
        printf("\n========== CHILD PROCESS ==========\n");


        /* -----------------------------------------
           1. RECEIVE DATA USING PIPE
           ----------------------------------------- */

        close(pipe_fd[1]);

        read(
            pipe_fd[0],
            buffer,
            sizeof(buffer)
        );

        printf(
            "Child received through PIPE: %s\n",
            buffer
        );

        close(pipe_fd[0]);


        /* -----------------------------------------
           2. RECEIVE DATA USING FIFO
           ----------------------------------------- */

        int fifo_fd;

        fifo_fd = open(
            "myfifo",
            O_RDONLY
        );

        read(
            fifo_fd,
            buffer,
            sizeof(buffer)
        );

        printf(
            "Child received through FIFO: %s\n",
            buffer
        );

        close(fifo_fd);


        /* -----------------------------------------
           3. RECEIVE DATA USING MESSAGE QUEUE
           ----------------------------------------- */

        msgrcv(
            msgid,
            &msg,
            sizeof(msg.msg_text),
            1,
            0
        );

        printf(
            "Child received through MESSAGE QUEUE: %s\n",
            msg.msg_text
        );


        /* -----------------------------------------
           4. ACCESS SHARED MEMORY USING SEMAPHORE
           ----------------------------------------- */


        /* WAIT / LOCK */

        semop(
            semid,
            &wait_operation,
            1
        );


        printf(
            "Child read from SHARED MEMORY: %s\n",
            shared_memory
        );


        /* SIGNAL / UNLOCK */

        semop(
            semid,
            &signal_operation,
            1
        );


        /* DETACH SHARED MEMORY */

        shmdt(
            shared_memory
        );


        printf(
            "\nChild process finished.\n"
        );
    }


    /* =====================================================
       PARENT PROCESS
       ===================================================== */

    else if (pid > 0)
    {
        printf("\n========== PARENT PROCESS ==========\n");


        /* -----------------------------------------
           1. SEND DATA USING PIPE
           ----------------------------------------- */

        close(pipe_fd[0]);

        strcpy(
            buffer,
            "Hello Child from Parent using PIPE"
        );

        write(
            pipe_fd[1],
            buffer,
            strlen(buffer) + 1
        );

        printf(
            "Parent sent through PIPE.\n"
        );

        close(pipe_fd[1]);


        /* -----------------------------------------
           2. SEND DATA USING FIFO
           ----------------------------------------- */

        int fifo_fd;

        fifo_fd = open(
            "myfifo",
            O_WRONLY
        );

        strcpy(
            buffer,
            "Hello Child from Parent using FIFO"
        );

        write(
            fifo_fd,
            buffer,
            strlen(buffer) + 1
        );

        printf(
            "Parent sent through FIFO.\n"
        );

        close(fifo_fd);


        /* -----------------------------------------
           3. SEND DATA USING MESSAGE QUEUE
           ----------------------------------------- */

        msg.msg_type = 1;

        strcpy(
            msg.msg_text,
            "Hello Child from Parent using MESSAGE QUEUE"
        );

        msgsnd(
            msgid,
            &msg,
            sizeof(msg.msg_text),
            0
        );

        printf(
            "Parent sent through MESSAGE QUEUE.\n"
        );


        /* -----------------------------------------
           4. WRITE DATA INTO SHARED MEMORY
              USING SEMAPHORE
           ----------------------------------------- */


        /* WAIT / LOCK */

        semop(
            semid,
            &wait_operation,
            1
        );


        strcpy(
            shared_memory,
            "Hello Child from Parent using SHARED MEMORY"
        );


        printf(
            "Parent wrote into SHARED MEMORY.\n"
        );


        /* SIGNAL / UNLOCK */

        semop(
            semid,
            &signal_operation,
            1
        );


        /* DETACH SHARED MEMORY */

        shmdt(
            shared_memory
        );


        /* WAIT FOR CHILD */

        wait(NULL);


        /* =================================================
           REMOVE IPC RESOURCES
           ================================================= */


        /* REMOVE FIFO */

        unlink(
            "myfifo"
        );


        /* REMOVE MESSAGE QUEUE */

        msgctl(
            msgid,
            IPC_RMID,
            NULL
        );


        /* REMOVE SHARED MEMORY */

        shmctl(
            shmid,
            IPC_RMID,
            NULL
        );


        /* REMOVE SEMAPHORE */

        semctl(
            semid,
            0,
            IPC_RMID
        );


        printf(
            "\nParent cleaned up all IPC resources.\n"
        );
    }


    /* =====================================================
       FORK FAILED
       ===================================================== */

    else
    {
        perror("fork failed");
        exit(1);
    }


    return 0;
}
