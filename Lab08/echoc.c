#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sockop.h"

#define BUFSIZE	1024

int main(int argc, char *argv[])
{
	int connfd;	/* socket descriptor */
	int n;
	char bufw[BUFSIZE], bufr[BUFSIZE];

	if (argc != 4)
		errexit("Usage: %s host_address host_port message\n", argv[0]);

	sprintf(bufw, "I'm Client %s",argv[3]);

	/* create socket and connect to server */
	connfd = connectsock(argv[1], argv[2], "tcp");

	while(1) {

		/* write message to server */
		if ((n = write(connfd, bufw, strlen(bufw))) == -1)
			errexit("Error: write()\n");

		/* read message from the server and print */
		memset(bufr, 0, BUFSIZE);
		if ((n = read(connfd, bufr, BUFSIZE)) == -1)
			errexit("Error: read()\n");
		printf("Server Reply: %s\n", bufr);
		sleep(1);
	}
	/* close client socket */
	close(connfd);

	return 0;
}
