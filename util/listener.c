#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include "plumber.h"

#define BUFSIZE 2048


static void *start_listening(void *ud)
{
    sporth_listener *sl = ud;
    int portno = sl->portno;
    int sockfd; /* socket */
    socklen_t clientlen; /* byte size of client's address */
    struct sockaddr_in serveraddr; /* server's addr */
    struct sockaddr_in clientaddr; /* client addr */
    struct hostent *hostp; /* client host info */
    char buf1[BUFSIZE]; /* message buf */
    char buf2[BUFSIZE]; /* message buf */
    int whichbuf = 0;
    char *buf; /* message buf */
    char *hostaddrp; /* dotted decimal host addr string */
    int optval; /* flag value for setsockopt */
    uint32_t n; /* message byte size */
    plumber_data *pd = sl->pd;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
    fprintf(stderr, "ERROR opening socket");

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
    (const void *)&optval , sizeof(int));

    memset((char *) &serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons((unsigned short)portno);
    if (bind(sockfd, (struct sockaddr *) &serveraddr,
    sizeof(serveraddr)) < 0)
    fprintf(stderr, "ERROR on binding");

    clientlen = sizeof(clientaddr);
    while (sl->start) {

        if(whichbuf == 0) {
            buf = buf1;
        } else {
            buf = buf2;
        }
        memset(buf, 0, BUFSIZE);
        n = recvfrom(sockfd, buf, BUFSIZE, 0,
            (struct sockaddr *) &clientaddr, &clientlen);

        if (n < 0) fprintf(stderr, "ERROR in recvfrom");

        hostp = gethostbyaddr((const char *)&clientaddr.sin_addr.s_addr,

        sizeof(clientaddr.sin_addr.s_addr), AF_INET);

        if (hostp == NULL) fprintf(stderr, "ERROR on gethostbyaddr");

        hostaddrp = inet_ntoa(clientaddr.sin_addr);

        if (hostaddrp == NULL) fprintf(stderr, "ERROR on inet_ntoa\n");

        printf("server received datagram from %s (%s)\n",
        hostp->h_name, hostaddrp);
        printf("server received %lu/%d bytes: %s\n", strlen(buf), n, buf);
        /* n = sendto(sockfd, buf, strlen(buf), 0,
        (struct sockaddr *) &clientaddr, clientlen); */
        if (n < 0) fprintf(stderr, "ERROR in sendto");
        pd->str = buf;
        whichbuf = (whichbuf == 0) ? 1 : 0;
        pd->recompile = 2;
    }

    pthread_exit(NULL);
}


void sporth_start_listener(sporth_listener *sl)
{
    pthread_create(&sl->thread, NULL, start_listening, sl);
}
