#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <pthread.h>
#include <signal.h>

struct mymsgbuf {
                long type;
                int a,b;
                int pidc;
        } garr;//Global ARRay

int n=0;//номер работающего треда


pthread_t thread_id[2]; // array
int tid = 0; // index of thread_id
pthread_mutex_t lock;

key_t key = 303;

void* my_thread(void* arg) {
        //pthread_mutex_lock(&lock);

        struct mymsgbuf iget = *(struct mymsgbuf*)arg;
        int k = iget.type;

        //pthread_mutex_unlock(&lock);
        sleep(iget.a + iget.b);
        iget.a += iget.b;

        iget.type = iget.pidc + 1;
        int despid; // descriptor   


        if((despid = msgget(key, 0666 | IPC_CREAT)) < 0) { 
                        printf("error: key\n");
                        exit(-1);
	}
        msgsnd(despid,(struct msgbuf *) &iget,16,0); // sending
        printf("Result is sent to %d \n",iget.pidc);

        thread_id[k] = 0;
        n--; //thread ends
        return NULL;
}

int main() {

        int despid; // descriptor   

	if((despid = msgget(key, 0666 | IPC_CREAT)) < 0) {
                        printf("error: key \n");
                        exit(-1);
        }
	int i;
	thread_id[0] = 0;
	thread_id[1] = 0;
	pthread_mutex_init(&lock, NULL);
	
	while(1) {
	
	if (msgrcv(despid,(struct msgbuf*) &garr,16,1,0) < 0){
		printf("error: receive \n");
		msgctl(despid, IPC_RMID, (struct msqid_ds*)NULL);
		exit(-1);
	}
                        
	if (n < 2) {

		if (thread_id[0] == 0) n = 0; 
		else tid = 1; 

		garr.type = tid;
		i = pthread_create(&thread_id[n] ,(pthread_attr_t *)NULL ,my_thread ,&garr);

		if (i) {
			printf("error: create");
			exit(-1);
		}	

		printf("message is received from %d\n",garr.pidc);
                n++;
	} else
			{
				garr.type = garr.pidc + 1;
				printf("busy\n");
				garr.pidc = 0;
				msgsnd(despid,(struct msgbuf *) &garr,16,0);
			}
                }
        
	pthread_mutex_destroy(&lock);

        return 0;
}

