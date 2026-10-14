#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char** argv){


	struct sockaddr_in server;
	int fdmax, fd_count, fd_server;
	char buff[512];

	//create the socket file descriptor
	fd_server = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_server < 0){
	
		perror("Error creating server \n");
		return -1;
	}
	printf("Socket created...\n");

	//do server settings 
	memset(&server, 0, sizeof(server)); 

	server.sin_family =  AF_INET;
	server.sin_port = htons(6767);
	server.sin_addr.s_addr = INADDR_ANY;


	//bind server 
	if(bind(fd_server, (struct sockaddr *)&server, sizeof(server)) < 0){
		perror("Error binding socket\n");
		return -2;
	}
	printf("Port binded...\n");


	//start listening for clients
	if (listen(fd_server, 5) < 0){
		perror("Error listening to server\n");
		return -3;
	}

	//initialze the fd_set's 
	fd_set master;
	fd_set fds_read;

	FD_ZERO(&master);
	FD_ZERO(&fds_read);

	FD_SET(fd_server, &master);//add the current file descriptor to the master;

	fdmax= fd_server; //the biggest file descriptor we have is the server socket fd
	fd_count= 0; //we have 0 clients
	
	printf("Server succsefully started! Listenting:\n");

	for(;;){

		fds_read = master; //update the reading fd_set
				
		if(select(fdmax+1, &fds_read,NULL, NULL, NULL) == -1){
			perror("Select error.\n");
			return -4;
		}

		int i;

		for(i = 0; i<= fdmax; i++){
			if (FD_ISSET (i, &fds_read)){
				
				if (i == fd_server){
					//if we get something on the server, then we have a new client and we accept it 
					struct sockaddr_in client_addr;
					int len = sizeof(client_addr);
					int client = accept(fd_server,(struct sockaddr*)&client_addr,&len);					
					
					if(client < 0) 
					{
						perror("Accept error\n");
						return -5;
					}

					printf("New client connected; %s:%d.\n",
						       	inet_ntoa(client_addr.sin_addr),
						       	client_addr.sin_port);

					FD_SET(client, &master); //we add client fd to the master fd_set
					
					if (client > fdmax){
						fdmax= client;
					}

					fd_count++;

					sprintf(buff, "Weclome to the server client %d !!!\n", client-3);
					send(client, buff, strlen(buff), 0);
				}

				else {// if a client sent a message
					
					int rcv = recv(i, buff, sizeof(buff)-1, 0);
					if (rcv == 0){
						printf("Client %d hanged up.\n", i-3);
						close (i); // always close a fd when done with them.
						FD_CLR(i, &master);// take it out of the master
						fd_count--;//decrease the count
						continue;
					}
					else if(rcv < 0){
						printf("Receive error for client %d\n", i-3);
						close(i);
						FD_CLR(i, &master); 
						fd_count--;
						continue;
					}
					else{
						char tempbuf[1024];
						buff[rcv] = '\0';
						int snd = sprintf(tempbuf, "Client %d says %s", i-3, buff);

						int j; 
						for(j=3; j<= fdmax; j++){
							if(FD_ISSET(j, &master)){
								if(j != fd_server && j!=i){
									if(send(j, tempbuf, snd, 0) == -1){
										printf("Error sending to clinet %d\n", j-3);
									}
								}

							}
						
						}
					
					}
				
				}
			}
		}
	}
}
