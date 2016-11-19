#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>

int main(int argc, char* argv[]){
	int desipc; // descriptor ipc
	key_t key=303; // key ipc
	int i=0; 
	int sum;

	struct mymsgbyf{
		long type;
		int a,b;
		int pidc;//pid
	} user;

	if((desipc = msgget(key, 0666 | IPC_CREAT)) < 0){
		printf("error: key \n");
		exit(-1);
	}

	user.a=atoi(argv[1]);
	user.b=atoi(argv[2]);
	user.type=1;
	user.pidc=getpid();

	if(msgsnd(desipc,(struct msgbuf *) &user, 16, 0)< 0) {
		printf("error: send \n");
		msgctl(desipc, IPC_RMID, (struct msqid_ds*)NULL);	
		exit(-1);
	}
	printf("a and b are send \n");
//wait
	if(msgrcv(desipc,(struct msgbuf *) &user, 16, user.pidc + 1, 0) < 0){
		printf("error: receive \n");
		msgctl(desipc, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
	
	if(user.pidc > 0){
		sum=user.a;
		printf("sum=%d \n", sum);
	}
	else printf("server is busy");

return 0;





	
}

