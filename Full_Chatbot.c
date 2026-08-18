// CHATBOT CLIENT

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
    char message[MAX_LINE];
    char reply[MAX_LINE];
    int s;

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

    printf("Connected to chatbot server.\n");
    printf("Type 'exit' to quit.\n\n");

    while (1)
    {
        /* Client sends message */

        printf("You: ");
        fgets(message, sizeof(message), stdin);

        send(
            s,
            message,
            strlen(message) + 1,
            0
        );

        if (strncmp(message, "exit", 4) == 0)
            break;

        /* Client waits for server */

        recv(
            s,
            reply,
            sizeof(reply),
            0
        );

        printf("Bot: %s\n", reply);

        if (strncmp(reply, "exit", 4) == 0)
            break;
    }

    close(s);

    return 0;
}














// CHATBOT SERVER

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
    char message[MAX_LINE];
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

    if (bind(
        s,
        (struct sockaddr *)&sin,
        sizeof(sin)
    ) < 0)
    {
        perror("bind");
        exit(1);
    }

    listen(s, MAX_PENDING);

    printf("Chatbot server waiting for client...\n");

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

    printf("Client connected.\n");
    printf("Type 'exit' to close the chat.\n\n");

    while (1)
    {
        /* Receive message from client */

        recv(
            new_s,
            message,
            sizeof(message),
            0
        );

        printf("You: %s", message);

        if (strncmp(message, "exit", 4) == 0)
            break;

        /*
         * Simple chatbot responses
         */

        if (strstr(message, "hello") != NULL ||
            strstr(message, "Hello") != NULL)
        {
            strcpy(reply, "Hello! How can I help you?");
        }
        else if (strstr(message, "how are you") != NULL)
        {
            strcpy(reply, "I am fine! Thanks for asking.");
        }
        else if (strstr(message, "name") != NULL)
        {
            strcpy(reply, "I am a simple socket chatbot.");
        }
        else if (strstr(message, "bye") != NULL ||
                 strstr(message, "Bye") != NULL)
        {
            strcpy(reply, "Goodbye! Have a nice day.");
        }
        else
        {
            strcpy(reply, "I received your message.");
        }

        /* Send chatbot response */

        send(
            new_s,
            reply,
            strlen(reply) + 1,
            0
        );

        printf("Bot: %s\n", reply);
    }

    close(new_s);
    close(s);

    return 0;
}
