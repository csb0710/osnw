#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#define MAXLINE 1024


union semun
{
	int val;
};

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
	int shmid;
	int semid;
	char *input;
    	char *left;
    	char *right;
    	char *op;


	struct cal_data *data;
	void *shared_memory = NULL;
	union semun sem_union;

	struct sembuf semopen = {0, -1, SEM_UNDO};
	struct sembuf semclose = {0, 1, SEM_UNDO};
	
	shmid = shmget((key_t)1234, sizeof(struct cal_data), 0666|IPC_CREAT);
	if (shmid == -1)
	{
		printf("hi1\n");
		return 1;
	}

	semid = semget((key_t)3477, 1, IPC_CREAT|0666);
	if(semid == -1)
	{
		printf("hi2\n");
		return 1;
	}

	shared_memory = shmat(shmid, NULL, 0);
	if (shared_memory == (void *)-1)
	{
		return 1;
	}

	data = (struct cal_data *)shared_memory;

	sem_union.val = 0;
	if ( -1 == semctl(semid, 0, SETVAL, sem_union))
	{
		return 1;
	}
	while(1)
	{
		memset(data, 0x00, MAXLINE);
		input = malloc(100);
        	read(0, input, 100);
        	left = strtok(input, " ");
        	right = strtok(NULL, " ");
       		op = strtok(NULL, " ");


		data->left_num = htonl(atoi(left));
        	data->right_num = htonl(atoi(right));
        	if (op[0] == '+' || op[0] == '-' || op[0] == 'x' || op[0] == '/' || op[0] == '$')
                	data->op = op[0];
		
		for (int i = 0; i < sizeof(*data); i++) {
                       printf("%02x ", *(((unsigned char *)data)+i));
                }
                printf("\n");

		
		if(semop(semid, &semclose, 1) == -1)
                {
                        return 1;
                }
	
		sleep(2);
		
		if(semop(semid, &semopen, 1) == -1)
                {
                        return 1;
                }

		//data->left_num = ntohl(data->left_num);
                //data->right_num = ntohl(data->right_num);
                //data->result = ntohl(data->result);
                //data->error = ntohl(data->error);

		if (data->error != 0)
        	{
               		 printf("CALC Error %d\n", ntohs(data->error));

        	}
        	else {
			printf("%d %d %c %d\n", ntohl(data->left_num), 
				  ntohl(data->right_num), data->op, ntohl(data->result));
		}
		for (int i = 0; i < sizeof(*data); i++) {
                       printf("%02x ", *(((unsigned char *)data)+i));
                }
                printf("\n");

	}
	return 1;
}

