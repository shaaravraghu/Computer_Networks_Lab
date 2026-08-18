// HALF-DUPLEX CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define SERVER_PORT 5432
#define MAX_LINE 256

int main(int argc, char *argv[])
{
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    char reply[MAX_LINE];
    int s;
    int len;

    if (argc == 2)
    {
        host = argv[1];
    }
    else
    {
        fprintf(stderr, "Usage: client host\n");
        exit(1);
    }

    hp = gethostbyname(host);

    if (hp == NULL)
    {
        fprintf(stderr, "Unknown host: %s\n", host);
        exit(1);
    }

    bzero((char *)&sin, sizeof(sin));

    sin.sin_family = AF_INET;

    bcopy(
        hp->h_addr,
        (char *)&sin.sin_addr,
        hp->h_length
    );

    sin.sin_port = htons(SERVER_PORT);

    s = socket(PF_INET, SOCK_STREAM, 0);

    if (s < 0)
    {
        perror("socket");
        exit(1);
    }

    if (connect(
        s,
        (struct sockaddr *)&sin,
        sizeof(sin)
    ) < 0)
    {
        perror("connect");
        close(s);
        exit(1);
    }

    while (1)
    {
        printf("Client: ");

        fgets(buf, sizeof(buf), stdin);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        /* Send message */
        len = strlen(buf) + 1;
        send(s, buf, len, 0);

        /* Wait for server reply */
        recv(s, reply, sizeof(reply), 0);

        printf("Server: %s\n", reply);
    }

    close(s);

    return 0;
}




// HALF-DUPLEX SERVER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define SERVER_PORT 5432
#define MAX_PENDING 5
#define MAX_LINE 256

int main()
{
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    char reply[MAX_LINE];
    int len;
    int s;
    int new_s;

    bzero((char *)&sin, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    s = socket(PF_INET, SOCK_STREAM, 0);

    if (s < 0)
    {
        perror("socket");
        exit(1);
    }

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
        perror("bind");
        exit(1);
    }

    listen(s, MAX_PENDING);

    printf("Server waiting for client...\n");

    len = sizeof(sin);

    new_s = accept(
        s,
        (struct sockaddr *)&sin,
        &len
    );

    if (new_s < 0)
    {
        perror("accept");
        exit(1);
    }

    while (1)
    {
        /* Receive message from client */
        recv(new_s, buf, sizeof(buf), 0);

        if (strncmp(buf, "exit", 4) == 0)
            break;

        printf("Client: %s", buf);

        /* Prepare reply */
        printf("Server: ");
        fgets(reply, sizeof(reply), stdin);

        /* Send reply */
        send(
            new_s,
            reply,
            strlen(reply) + 1,
            0
        );
    }

    close(new_s);
    close(s);

    return 0;
}
