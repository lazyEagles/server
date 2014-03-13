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
	struct sockaddr_in cliaddr;
	int buf[MAX_LINE];
	int len = sizeof(cliaddr);
	int sd, new_sd;
	FILE *fd;
	struct stat st;
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
 
	listen(sd, MAX_PENDING);
 
	while(1) {
		if ((new_sd = accept(sd, (struct sockaddr*) &sin, &len)) < 0) {
			perror("simplex-talk: accept");
			exit(EXIT_FAILURE);
		}
		int child = fork();
		if (child == 0) {
		const char *picname = "\"FILE:102062999.jpg\"";
		const char get[20] = "\"GET:102062999.jpg\"";
		const char *upload = "102062999.jpg";
		strncpy((char *)buf, picname, MAX_LINE);
		send(new_sd, buf, sizeof(buf), 0); // server send FILE:xx.jpg to client
		recv(new_sd, buf, sizeof(buf), 0); // server recive GET:xxx.jpg
		if (strncmp((char *)buf, get, strlen(get)) == 0) {
			//stat(upload, &st);
			//file_size = st.st_size; // how many bytes to send
			fd = fopen(upload, "r");
			int c;
			int i;
			do {
				for (i = 0, c = 0; i < MAX_LINE && c != EOF; i++) {
					c = fgetc(fd);
					buf[i] = c;
					//printf("%d:%x ", i, buf[i]);
				}
				int fuck = 0;
				fuck = send(new_sd, buf, sizeof(buf), 0);
				printf("we send %d\n",fuck);
			} while(c != EOF);
			fclose(fd);
			const char *info = "\"CLOSE:\"";
			strncpy((char *)buf, info, MAX_LINE);
			send(new_sd, buf, sizeof(buf), 0);
		}
		printf("close socket\n");
		close(new_sd);

		}
		else 
		{
			close(new_sd);

		}
	}
	return 0;
}
