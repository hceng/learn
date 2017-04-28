#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <sys/msg.h>
#include <string.h>

struct msgbuf
{
	long type;
	char text[32];
};

#define MSG_SIZE (sizeof(struct msgbuf) - sizeof(long))

int main(int argc, const char *argv[])
{
	key_t key;
	int msgid;
	struct msgbuf mymsg;

	key = ftok(".", 'a');
	if(key < 0)
	{
		perror("Unable to ftok");
		exit(1);
	}
	
	msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
	if(msgid < 0)
	{
		if(errno == EEXIST)
		{
			msgid = msgget(key, 0666);
		}
		else
		{
			perror("Unable to msgget");
			exit(1);
		}
	}
	while(1)
	{
		if(msgrcv(msgid, &mymsg, MSG_SIZE, 100, 0) < 0)
		{
			perror("Unable to msgsnd");
			exit(1);
		}
		system("ipcs -q");

		printf("-->%ld  %s \n", mymsg.type, mymsg.text);
	}
	if(msgctl(msgid, IPC_RMID, NULL) < 0)
	{
		perror("Unable to msgctl");
		exit(1);
	}
	system("ipcs -q");
	return 0;
}
