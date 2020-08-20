#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/select.h>
#include <string.h>
#include <time.h>

void Clear_Field(char Field[10][10])
{
	for(int i=0;i<10;i++)
	{
		for(int j=0;j<10;j++)
		{
		Field[i][j]=' ';

		}
	}
}

int Socket(int domain,int type,int protocol);

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);

void Listen(int sockfd,int backlog);

int Accept(int sockfd, struct sockaddr *addr,socklen_t *addrlen);

int comp(const void *a,const void *b);

struct data
{
	int *client_socket;
	socklen_t *adrlen;
};


void *Server(void *data_input)
{
	printf("Начало игры\n");
	struct data *input=(struct data *)data_input;
	int valread,i=0,sd,Send_Coord[2];
	char First_Field[10][10];
	char Second_Field[10][10];
	
	for(int i=0;i<2;i++)
	{
		sd = input->client_socket[i];
	}
	srand(time(NULL));
	int player_move;
	player_move=rand() % 2;
	if(player_move)  
	{
		
		sd = input->client_socket[1];      
		send(sd,"0",sizeof(char),0);
		sd = input->client_socket[0];      
		send(sd,"1",sizeof(char),0); 
		
		sd = input->client_socket[0];    
		recv(sd,First_Field,100,0);
		sd = input->client_socket[1];
		send(sd,First_Field,100,0);    	
		recv(sd,Second_Field,100,0);
		sd = input->client_socket[0];
		send(sd,Second_Field,100,0);
		
		
		while(1)
		{
			sd = input->client_socket[0];
			printf("Получаю\n");
			recv(sd,Send_Coord,sizeof(int)*40,0);
			sd = input->client_socket[1];
			printf("отправляю\n");
			send(sd,Send_Coord,sizeof(int)*40,0);
			printf("Получаю\n");
			recv(sd,Send_Coord,sizeof(int)*40,0);
			sd = input->client_socket[0];
			printf("отправляю\n");
			send(sd,Send_Coord,sizeof(int)*40,0);
		}

	}else
		{
			sd = input->client_socket[1];      
			send(sd,"1",sizeof(char),0);
			sd = input->client_socket[0];      
			send(sd,"0",sizeof(char),0);
		
			sd = input->client_socket[1];    
			read(sd,Second_Field,100);
			sd = input->client_socket[0];
			send(sd,Second_Field,100,0);     
			read(sd,First_Field,100);
			sd = input->client_socket[1];
			send(sd,First_Field,100,0);
			
		
		while(1)
		{
			sd = input->client_socket[1];
			read(sd,Send_Coord,sizeof(int)*40);
			sd = input->client_socket[0];
			send(sd,Send_Coord,sizeof(int)*40,0);
			read(sd,Send_Coord,sizeof(int)*40);
			sd = input->client_socket[1];
			send(sd,Send_Coord,sizeof(int)*40,0);

		}

}
}

int main(int argv,char *argc[])
{
	if (argv!=2)
	{
		perror("Not port given");
		exit(EXIT_FAILURE);
	}
	int port = atoi(argc[1]);
	if (port == 0)
	{
		perror ("Not Correct port");
		exit(EXIT_FAILURE);
	}
	
	pthread_t thread;
	int Master_Socket = Socket(AF_INET,SOCK_STREAM,0); //ipv4 tcp
	int New_Socket1,New_Socket2;
	struct sockaddr_in adr={0};
	
	if(setsockopt(Master_Socket,SOL_SOCKET,SO_REUSEADDR,(char *)&adr,sizeof(adr))<0)
	{
		printf("Error setsockopt");
		exit(EXIT_FAILURE);
	}
	
	adr.sin_family = AF_INET;
	adr.sin_port = htons(port);
	
	
	
	
	if (adr.sin_port ==0)
	{
		printf("The entered port is occupied by another port\n");
		printf("New port: %d\n",port);      
		
	}else printf("Listen on port: %d \n",port);
	
	int client_socket[2];
	Bind(Master_Socket,(struct sockaddr *) &adr,sizeof adr);
	Listen(Master_Socket,2); 
	socklen_t adrlen = sizeof adr;
	puts("Ожидание подключения");
	New_Socket1=Accept(Master_Socket,(struct sockaddr *) &adr, &adrlen);
	printf("Подключился игрок\n");
	client_socket[0]=New_Socket1;
	New_Socket2=Accept(Master_Socket,(struct sockaddr *) &adr, &adrlen);
	printf("Подключился игрок\n");
	client_socket[1]=New_Socket2;
	puts("Подсключение успешно\n");
	
	

	struct data threads_data;
	threads_data.client_socket=client_socket;
	threads_data.adrlen=&adrlen;
	pthread_create(&thread,NULL,Server,(void *)&threads_data);
	pthread_join(thread,NULL);
	close(New_Socket1);
	close(New_Socket2);
	close(Master_Socket);
	return 0;
}

int Socket(int domain,int type,int protocol)
{
	 int res = socket(domain,type,protocol);
	 if (res == -1){
		 printf("socket failed");
		 exit(EXIT_FAILURE);
	 }
	 return res;
}
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
{
	int res = bind(sockfd,addr,addrlen);
	if(res == -1){
		printf("Bind failed");
		exit(EXIT_FAILURE);
	}
}

void Listen(int sockfd,int backlog)
{
	int res = listen(sockfd,backlog);
	if (res == -1)
	{
		printf("listen failed");
		exit(EXIT_FAILURE);
	}
	
}

int Accept(int sockfd, struct sockaddr *addr,socklen_t *addrlen)
{
	int res=accept(sockfd,addr,addrlen);
	if (res == -1)
	{
		printf("accept failed");
		exit(EXIT_FAILURE);
	}
	return res;
}
