#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>

int socket_des;

struct message{
	char name[15];
	char text[1000];

} msg;

struct sockaddr_in socket_addr;

void* pthr_f(void *arg){
	
	
	struct message msg1;	
	while(1){
		if(recv(socket_des, &msg1, 1015, 0)<=0){
			printf("Не могу получить сообщение\n");
			exit(-1);
		  
		}
	printf("%s: %s \n", msg1.name, msg1.text);
	}
	
	
	return NULL;
}


pthread_t pthr;

int main(int argc, char *argv[]) {

	if(argc != 3){
		printf("Некорректный ввод\n");
		exit(-1);
	}

	if((socket_des = socket(PF_INET, SOCK_STREAM, 0))<0){
		printf("Не могу создать сокет\n");
		exit(-1);
	}
	
	socket_addr.sin_port = htons(6000);
	socket_addr.sin_family = PF_INET;

	if(inet_pton(PF_INET, argv[2], &socket_addr.sin_addr)<0){
		printf(" inet_pton error %s \n", strerror(errno));
		exit(-1);
	}

	if(connect(socket_des, (struct sockaddr *)&socket_addr, sizeof(socket_addr))<0){
		printf("Не могу подключиться к серверу %s \n", strerror(errno));
		exit(-1);
	}
	strcpy(msg.name, argv[1]);
	msg.name[strlen(argv[1])]=0;

	if(send(socket_des, &msg.name, sizeof(msg.name)+1,0)<0){
		printf("Не могу отправить имя\n");
		exit(-1);
	}
	
	if(pthread_create(&pthr, (pthread_attr_t *)NULL, pthr_f, &msg)>0){
		printf("Не могу создать нить\n");
		exit(-1);
	}
	
	printf("Добро пожаловать в чат, %s! \n", msg.name);

	while(1){
		scanf("%s", msg.text);
		if(send(socket_des, &msg.text, sizeof(msg.text)+1 , 0)<0){
			printf("Не могу отправить сообщение\n");
			exit(-1);
		}
	}

	return 0;
}

