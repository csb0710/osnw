#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAXBUF 1024

int main(int argc, char **argv)
{
	int server_sockfd, client_sockfd;
	int client_len, n;
	char buf[MAXBUF];
	struct sockaddr_in clientaddr, serveraddr;
	fd_set reads, temps;
	
	client_len = sizeof(clientaddr);
	if ((server_sockfd = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP )) == -1)
	{
		perror("socket error : ");
		exit(0);
	}
	memset(&serveraddr, 0x00, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi(argv[1]));
	bind (server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	listen(server_sockfd, 5);

	FD_ZERO(&reads);
	FD_SET(server_sockfd, &reads);
	int i = 0;
	while(1)
	{
		
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr,
			&client_len);
		FD_ZERO(&reads);
        	FD_SET(client_sockfd, &reads);
		printf("New Client Connect: %s\n", inet_ntoa(clientaddr.sin_addr));
		while(1) {
			temps = reads;
			memset(buf, 0x00, MAXBUF);:
			if (select(client_sockfd+1, &temps, 0, 0, NULL) < 0) {
				printf("실패!\n");
			}

			for (i = 0; i < client_sockfd + 1; i++ {
				if(FD_ISSET(i, &temps)) {
				if ((n = read(i, buf, MAXBUF)) <= 0)
                                {
                                        close(client_sockfd);
                                       	 continue;
                                }
				if (write(client_sockfd, buf, MAXBUF) <=0)
				{
					perror("write error : ");
					close(client_sockfd);
				}
				if (strcmp(buf, "quit\n")==0)
                       	 	{
                                	printf("Disconnected: %s\n", inet_ntoa(clientaddr.sin_addr));
                                	break;
                        	}
				}
				else {
					printf("hi\n");
				}
			}
		}
		close(client_sockfd);

	}
	close(server_sockfd);
	return 0;
}
