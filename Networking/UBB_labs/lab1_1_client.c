//lab1_1 client


#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>



int main() {

       int c;
       struct sockaddr_in server;
       uint16_t a[50], b, suma;

       c = socket(AF_INET, SOCK_STREAM, 0);
       if (c < 0) {
              printf("Eroare la crearea socketului client\n");
              return 1;

       }

       memset(&server, 0, sizeof(server));
       server.sin_port = htons(1234);
       server.sin_family = AF_INET;
       server.sin_addr.s_addr = inet_addr("127.0.0.1");


       if (connect(c, (struct sockaddr *) &server, sizeof(server)) < 0) {
              printf("Eroare la conectarea la server\n");
              return 1;
       }


       printf("You can enter up to 50 numbers. 0 ends it.");

       for(int i=0; i<50; i++){
       		printf("Enter number.");
		scanf("%hu", &a[i]);
		if(a[i] == 0){
			i = 50;
		}
       }
       for(int i=0; i<50; i++){
       		a[i] = htons(a[i]);
   		send(c, &a[i], sizeof(a[i]), 0);
	}
       recv(c, &suma, sizeof(suma), 0);
       suma = ntohs(suma);
       printf("Suma este %hu\n", suma);

       close(c);
}

