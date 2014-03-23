#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
 
#define SERVER_PORT 33568
#define MAX_PENDING 5
#define MAX_LINE 256
 
int main(int argc, char* argv[])
{
	struct sockaddr_in sin;
	int buf[MAX_LINE];
	int len = 0;
	int sd, new_sd;
	FILE *fd;
	int file_size;
 
	bzero((char*)&sin, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = htons(INADDR_ANY);
	sin.sin_port = htons(SERVER_PORT);
 
	/* create a socket */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("simplex-talk: socket");
		exit(EXIT_FAILURE);
	}
	/* bind socket with address */
	if ((bind(sd, (struct sockaddr*)&sin, sizeof(sin))) < 0) {
		perror("simplex-talk: bind");
		exit(EXIT_FAILURE);
	}
	/* listen socket for connection */ 
	listen(sd, MAX_PENDING);
 
	while(1) {
		/* accept a connection */
		if ((new_sd = accept(sd, (struct sockaddr*) &sin, &len)) < 0) {
			perror("simplex-talk: accept");
			exit(EXIT_FAILURE);
		}
		int child = fork();
		if (child == 0) {
			const char *filename = "\"FILE:102062650.txt\"";
			const char get[20] = "\"GET:102062650.txt\"";
			const char *upload = "102062650.txt";
			strncpy((char *)buf, filename, MAX_LINE);
			send(new_sd, buf, sizeof(buf), 0); // server send FILE:xx.jpg to client
			recv(new_sd, buf, sizeof(buf), 0); // server recive GET:xxx.jpg
			if (strncmp((char *)buf, get, strlen(get)) == 0) {
				fd = fopen(upload, "r");
				int c;
				int i;
				do {
					for (i = 0, c = 0; i < MAX_LINE && c != EOF; i++) {
						c = fgetc(fd);
						buf[i] = c;
						//printf("%d:%x ", i, buf[i]);
					}
					int sendsize = 0;
					sendsize = send(new_sd, buf, sizeof(buf), 0);
					printf("we send %d bytes\n",sendsize);
				} while(c != EOF);
				fclose(fd);
				/* cancel fucntion: send "CLOSE:" for spec
				*
				* const char *info = "\"CLOSE:\"";
				* strncpy((char *)buf, info, MAX_LINE);
				* send(new_sd, buf, sizeof(buf), 0);
				*/
			}
			printf("close socket\n");
			close(new_sd);
		} else {
			close(new_sd);
		}
	}
	return 0;
}
