#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 3600
#define IP "127.0.0.1"

struct cal_data
{
    int left_num;
    int right_num;
    char op;
    int result;
    short int error;
};

int main(int argc, char **argv)
{
    struct sockaddr_in addr;
    int s;
    int len;
    int sbyte, rbyte;
    struct cal_data sdata;
    char *input;
    char *left;
    char *right;
    char *op;

    memset((void *)&sdata, 0x00, sizeof(sdata));

    s = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (s == -1)
    {
   	 return 1;
    }
   
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);

    if ( connect(s, (struct sockaddr *)&addr, sizeof(addr)) == -1 )
    {
   	 printf("fail to connect\n");
   	 close(s);
   	 return 1;
    }

    while(1) {
	input = malloc(100);
	read(0, input, 100);
	left = strtok(input, " ");
	right = strtok(NULL, " ");
	op = strtok(NULL, " ");
	memset((void *)&sdata, 0x00, sizeof(sdata));
	sdata.left_num = atoi(left);
	sdata.right_num = atoi(right);
    	if (op[0] == '+' || op[0] == '-' || op[0] == 'x' || op[0] == '/' || op[0] == '$')
    		sdata.op = op[0];    
  	len = sizeof(sdata);
    	sdata.left_num = htonl(sdata.left_num);
    	sdata.right_num = htonl(sdata.right_num);
    	sbyte = write(s, (char *)&sdata, len);
    
    	if(sbyte != len)
    	{
   		 return 1;
    	}

    	rbyte = read(s, (char *)&sdata, len);
    	if(rbyte != len)
    	{
   		 return 1;
    	}

	free(input);

    	if (op[0] == '$')
            break;

    	if (ntohs(sdata.error != 0))
    	{
   		 printf("CALC Error %d\n", ntohs(sdata.error));
    	}
    	else {
	    printf("%d %c %d = %d\n", ntohl(sdata.left_num), sdata.op, ntohl(sdata.right_num), ntohl(sdata.result));
    	}
    }
    close(s);
    return 0;
}
