#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main(int argc, char *argv[])
{  
  char *address;
  address = argv[1];  
   //printf("%s\n", *(argv+1)+2);
  int client_socket;
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
   //printf("%d\n",client_socket);
  struct sockaddr_in remote_address;
   remote_address.sin_family = AF_INET;
   remote_address.sin_port = htons(80);
   inet_aton(address, &remote_address.sin_addr);

   connect(client_socket, (struct sockaddr *)&remote_address, sizeof(remote_address));
   
   char request[]="GET / HTTP /1.1\r\n\r\n";
   char response[4096]; 

    send(client_socket, request, sizeof(request), 0);
    recv(client_socket, &response, sizeof(response), 0);

    printf("%s\n",response);
    close(client_socket);
    

   return 0;
}
