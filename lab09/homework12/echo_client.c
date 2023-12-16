#include <sys/socket.h>  
#include <arpa/inet.h> 
#include <sys/stat.h>
#include <stdio.h>      
#include <string.h>     
#include <unistd.h> 

#define MAXLINE 1024

int main(void) {
	struct sockaddr_in serveraddr;
	int server_sockfd;
	int client_len;
	char buf[MAXLINE];
	fd_set readfds, tempfds;

	if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket error");
		return 1;
	}

	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	serveraddr.sin_port = htons(3600);

	client_len = sizeof(serveraddr);

	if (connect(server_sockfd, (struct sockaddr *)&serveraddr, client_len) == -1) {
		perror("connect error :");
		return 1;
	}

	FD_ZERO(&readfds);
	FD_SET(0, &readfds);
	FD_SET(server_sockfd, &readfds);

	while (1) {
		tempfds = readfds;
		if ((select(server_sockfd + 1, &tempfds, (fd_set *)0, (fd_set *)0, NULL) > 0)) {
			if (FD_ISSET(0, &tempfds)) {
				memset((void *)buf, 0x00, MAXLINE);
				read(0, buf, MAXLINE);
				if (write(server_sockfd, buf, MAXLINE) <= 0) {
					perror("write error");
					return 1;
				}
			}
			
			if (FD_ISSET(server_sockfd, &tempfds)) {
				memset((void *)buf, 0x00, MAXLINE);
				if (read(server_sockfd, buf, MAXLINE) <= 0) {
					perror("read error");
					return 1;
				}

				if (strcmp(buf, "quit") == 0) {
					break;
				}

				printf("%s\n", buf);
			}
		}
	}

	close(server_sockfd);
	return 0;
}
