#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define MAXLINE 1024

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

	struct cal_data *data;
	void *shared_memory = NULL;

	int rfd, wfd;
        char *buf;

	struct sembuf semopen = {0, -1, SEM_UNDO};
	struct sembuf semclose = {0, 1, SEM_UNDO};

	if ((rfd = open("/tmp/myfifo_w", O_RDWR)) == -1)
        {
                perror("rfd error");
                return 0;
        }
        if ((wfd = open("/tmp/myfifo_r", O_RDWR)) == -1)
        {
                perror("wfd error");
                return 0;
        }

	shmid = shmget((key_t)1234, sizeof(struct cal_data), 0666);
	if (shmid == -1)
	{
		perror("shmget failed : ");
		exit(0);
	}

	semid = semget((key_t)3477, 0, 0666);
	if(semid == -1)
	{
		perror("semget failed : ");
		return 1;
	}

	shared_memory = shmat(shmid, NULL, 0);
	if (shared_memory == (void *)-1)
	{
		perror("shmat failed : ");
		exit(0);
	}

	data = (struct cal_data *)shared_memory;
	while(1)
	{
		buf = malloc(MAXLINE);
		
		if(semop(semid, &semopen, 1) == -1)
                {
                        return 1;
                }

		for (int i = 0; i < sizeof(*data); i++) {
                       printf("%02x ", *(((unsigned char *)data)+i));
                }
                printf("\n");
		switch(data->op)
                {
                        case '+':
                              data->result = data->left_num + data->right_num;
                              break;
                        case '-':
                              data->result = data->left_num  - data->right_num;
			      break;
			case 'x':
			      data->result = data->left_num * data->right_num;
			      break;
			case '/':
			      if(data->right_num == 0)
			      {
				      data->error = 2;
				      break;
			      }
			      data->result = data->left_num / data->right_num;
			      break;
			default:
			      data->error = 1;
                }
		if (data->error != 0)
                {
                         printf("CALC Error %d\n", data->error);

                }
                else {
                        printf("%d %c %d = %d\n", data->left_num, 
					data->op, data->right_num, data->result);
                }
		for (int i = 0; i < sizeof(*data); i++) {
                       printf("%02x ", *(((unsigned char *)data)+i));
                }
                printf("\n");

		if(semop(semid, &semclose, 1) == -1)
                {
                        perror("semop error : ");
                }
		
		sleep(2);
	}
	return 1;
}

