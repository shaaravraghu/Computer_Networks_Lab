#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <sys/wait.h>

using namespace std;

int main()
{
    mkfifo("p1p2", 0666);
    mkfifo("p2p1", 0666);

    pid_t pid = fork();

    if (pid < 0)
    {
        cout << "fork failed" << endl;
        return 1;
    }
    else if (pid == 0)
    {
        char buffer[100];

        int fd1 = open("p1p2", O_RDONLY);
        read(fd1, buffer, sizeof(buffer));
        close(fd1);

        cout << "Vikram received: " << buffer << endl;

        int fd2 = open("p2p1", O_WRONLY);
        strcpy(buffer, "KEY");
        write(fd2, buffer, strlen(buffer) + 1);
        close(fd2);

        cout << "Vikram sent." << endl;
    }
    else
    {
        char buffer[100];

        strcpy(buffer, "KEY");

        int fd1 = open("p1p2", O_WRONLY);
        write(fd1, buffer, strlen(buffer) + 1);
        close(fd1);

        cout << "Arjun sent." << endl;

        int fd2 = open("p2p1", O_RDONLY);
        read(fd2, buffer, sizeof(buffer));
        close(fd2);

        cout << "Arjun received: " << buffer << endl;

        wait(NULL);

        unlink("p1p2");
        unlink("p2p1");
    }

    return 0;
}