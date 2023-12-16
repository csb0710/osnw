#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#define MAXLINE 1024
#define PORTNUM 3600

int main(int argc, char **argv)
{
	int listen_fd, client_fd;
	pid_t pid;
	socklen_t addrlen;
	int readn;
	char buf[MAXLINE];
	char *result;
	struct sockaddr_in client_addr, server_addr;

	if( (listen_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		return 1;
	}
	memset((void *)&server_addr, 0x00, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNUM);

	if(bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==-1)
	{
		perror("bind error");
		return 1;
	}
	if(listen(listen_fd, 5) == -1)
	{
		perror("listen error");
		return 1;
	}

	signal(SIGCHLD, SIG_IGN);
	while(1)
	{
		addrlen = sizeof(client_addr);
		client_fd = accept(listen_fd,
			(struct sockaddr *)&client_addr, &addrlen);
		if(client_fd == -1)
		{
			printf("accept error\n");
			break;
		}
		printf("Connected Client %s(%d)\n", inet_ntoa(client_addr.sin_addr),
                                                client_addr.sin_port);
		pid = fork();
		if(pid == 0)
		{
			close( listen_fd );
			memset(buf, 0x00, MAXLINE);
			result = malloc(1);
			result[0] = '\0';
			while((readn = read(client_fd, buf, MAXLINE)) > 0)
			{
				if (strcmp(buf, "quit\n") != 0) {
					printf("Read Data %s(%d) : %s",
                                               inet_ntoa(client_addr.sin_addr),
                                               client_addr.sin_port,
                                               buf);
					result = strncat(result, buf, strlen(buf)-1);
					result = strcat(result, " ");
					memset(buf, 0x00, MAXLINE);
					continue;
				}
				result = strcat(result, "\n");
				memset(buf, 0x00, MAXLINE);
				write(client_fd, result, strlen(result));
				printf("Disconneted Client %s(%d)\n", inet_ntoa(client_addr.sin_addr),
                                                client_addr.sin_port);
				break;
			}
			close(client_fd);
			return 0;
		}
		else if( pid > 0)
			close(client_fd);
	}
	return 0;
}
