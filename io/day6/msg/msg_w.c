#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/msg.h>

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
	char buf[32] = {0};

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
	system("ipcs -q");

	mymsg.type = 100;

	//strcpy(mymsg.text,"hello world");
	while(1)
	{
		fgets(buf, 32, stdin);
		strcpy(mymsg.text, buf);
		
		if(msgsnd(msgid, &mymsg, MSG_SIZE, 0) < 0)
		{
			perror("Unable to msgsnd");
			exit(1);
		}
	}
	system("ipcs -q");

	return 0;
}
