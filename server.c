#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>

static void serve_new_conn(int sk)
{
    printf("New connection\n");

    int counter = 0;
    while (1) {
        sleep(1);
        if ((write(sk, &counter, sizeof(counter))) <= 0) {
            perror("Can't write socket");
            return;
        }
        counter++;
    }
}


static int main_srv(int argc, char **argv)
{
    int sk, port, option = 1;
    struct sockaddr_in addr;

    /* Let kids die themselves */
    signal(SIGCHLD, SIG_IGN);

    sk = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* Allow the port reuse immediately after the server is terminated. */
    setsockopt(sk, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

    if (sk < 0) {
        perror("Can't create socket");
        return -1;
    }

    port = atoi(argv[1]);
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    printf("Binding to port %d\n", port);

    if ((bind(sk, (struct sockaddr *)&addr, sizeof(addr))) < 0) {
        perror("Can't bind socket");
        return -1;
    }

    if ((listen(sk, 16)) < 0) {
        perror("Can't put sock to listen");
        return -1;
    }

    printf("Waiting for connections...\n");
    while (1) {
        int ask, pid;

        ask = accept(sk, NULL, NULL);
        if (ask < 0) {
            perror("Can't accept new conn");
            return -1;
        }

        pid = fork();
        if (pid < 0) {
            perror("Can't fork");
            return -1;
        }

        if (pid > 0)
            close(ask);
        else {
            close(sk);
            serve_new_conn(ask);
            exit(0);
        }
    }
}


int main(int argc, char **argv)
{
    if (argc == 2)
        return main_srv(argc, argv);
    else
        printf("Usage: %s <port>\nExample: %s 8080\n", argv[0], argv[0]);


    return 0;
}
