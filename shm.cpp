#include <iostream>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <cstring>

using namespace std;

int main()
{
    key_t key = ftok("progfile", 65);

    if (key == -1)
    {
        perror("ftok");
        return 1;
    }

    int shmid = shmget(key, 1024, 0666 | IPC_CREAT);

    if (shmid == -1)
    {
        perror("shmget");
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    if (pid > 0)
    {
        char *str = (char *)shmat(shmid, NULL, 0);

        strcpy(str, "KEY");
        cout << "Arjun sent: " << str << endl;

        shmdt(str);

        wait(NULL);

        shmctl(shmid, IPC_RMID, NULL);
    }
    else
    {
        sleep(1);

        char *str = (char *)shmat(shmid, NULL, 0);

        cout << "Vikram received: " << str << endl;

        shmdt(str);
    }

    return 0;
}
