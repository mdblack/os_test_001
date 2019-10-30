#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int listenfd;
void *mythreadfunc(void* connfdptr)
{
	char sendBuff[10000];
	char recvBuff[1025];
	char fname[1025];
	char* header="HTTP/1.x 200 OK\n";
	int connfd=*((int*)connfdptr);
	int i;

	recv(connfd,recvBuff,sizeof(recvBuff),0);
	printf("got: %s\n",recvBuff);

	for(i=5; recvBuff[i]!=' '; i++)
		fname[i-5]=recvBuff[i];
	fname[i]=0;

	printf("filename is %s\n",fname);

	FILE* f=fopen(fname,"r");
	for(i=0; i<sizeof(sendBuff)-1 && !feof(f); i++)
	{
		sendBuff[i]=fgetc(f);
	}
	sendBuff[i-1]=0;

//	snprintf(sendBuff,sizeof(sendBuff), "Hello there");

	write(connfd,header,sizeof(header));
	write(connfd,sendBuff,i);
	close(connfd);
	fclose(f);

	return NULL;
}

int main()
{
	struct sockaddr_in serv_addr;

	listenfd=socket(AF_INET,SOCK_STREAM,0);


	serv_addr.sin_family=AF_INET;
	serv_addr.sin_addr.s_addr=htonl(INADDR_ANY);
	serv_addr.sin_port=htons(5000);

	bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));
	listen(listenfd, 10);

	while(1)
	{
		int connfd=accept(listenfd, (struct sockaddr*)NULL, NULL);

		printf("connection made.  spawning thread\n");

		pthread_t mythread;

		pthread_create(&mythread, NULL, mythreadfunc, &connfd);

//		sleep(1);
	}



	return 0;
}
