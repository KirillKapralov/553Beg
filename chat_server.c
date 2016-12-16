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

#define PORT 6000

int n;
int socket_des=0;

struct client{
	
	pthread_t pthr;
	int des;
	
} client[14];

struct msg{

	char name[15];
	char text[1000];
}; 

void* pthr_f(void* arg){
	
	int i=0;
	struct msg to, from;
	int k = *(int *)arg;
	int byte;
	if(recv(client[k].des, from.name, 15, 0)<0){
		printf("Не могу прочесть имя: %s \n", strerror(errno));
		printf("%d", client[k].des);
		exit(-1);
	}

	printf("Добро пожаловать, %s !", from.name);
	while(1){
		byte = recv(client[k].des, from.text, 1000, 0);
		if(byte==0) break;
		if(byte<0){
			printf("Не могу прочесть сообщение\n");
			exit(-1);
		}	
		printf("Принято сообщение от пользователя %s \n", from.name );
	
		strcpy(to.name, from.name);
		strcpy(to.text, from.text);		
		printf("%s: %s\n", to.name, to.text);
		for(i=0;i<14;i++){

			if((i!=k)&&(client[i].pthr!=0)){
				if(send(client[i].des, &to, 1000, 0)<0){
					printf("Не могу отправить сообщение\n");
					exit(-1);
				}		
			}
		}
		
		printf("Сообщение от %s отправлено\n", from.name);
	}

	client[k].pthr=0;
}

struct sockaddr_in socket_addr;

int main(){
	
	 


	int i=0;
  	socket_des = socket(PF_INET, SOCK_STREAM, 0);	
	if(socket_des<0){
		printf("Не могу открыть socket\n");
		exit(-1);
	}
	
	
	socket_addr.sin_family = PF_INET;
	socket_addr.sin_port =htons(PORT);
	socket_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind(socket_des, (struct sockaddr*)&socket_addr, sizeof(socket_addr))<0){
		printf("Не могу связаться с сокетом\n");
		exit(-1);
	}
	
	if(listen(socket_des, 14)<0){
		printf("Не могу прослушать\n");
		exit(-1);
	}
	int client_des;	
	
	for(i=0; i<14; i++){ client[i].pthr = 0; }
	
	printf("Чат запущен\n");
	while(1){
		
		if((client_des = accept(socket_des, (struct sockaddr*)NULL, NULL))<0){
			
			printf("Не могу установить соединение \n");
			exit(-1);
		}

		for(i=0; i<14; i++){if(client[i].pthr==0){break;}}
		if(i==14){ 
			printf("Перегруз\n");	
			exit(-1);
		}
		
		n=i;
		client[i].des=client_des;
		pthread_create(&client[n].pthr, (pthread_attr_t *)NULL, pthr_f, &n);
		};

	return 0;
}
