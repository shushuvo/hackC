#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>


int main(int argc, char *argv[])
{  
  if (argc != 2) {
    printf("Usage: %s <IP address>\n", argv[0]);
    return 1;
  }
  
  char *address = argv[1];  

  int client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    perror("Socket creation failed");
    return 1;
  }
  
  struct sockaddr_in remote_address;
  remote_address.sin_family = AF_INET;
  remote_address.sin_port = htons(80);
  if (inet_aton(address, &remote_address.sin_addr) == 0) {
    printf("Invalid address\n");
    return 1;
  }

  if (connect(client_socket, (struct sockaddr *)&remote_address, sizeof(remote_address)) < 0) {
    perror("Connection failed");
    return 1;
  }
   
  char request[] = "GET / HTTP/1.1\r\n\r\n";
  char response[4096]; 

  if (send(client_socket, request, sizeof(request) - 1, 0) < 0) {
    perror("Send failed");
    return 1;
  }
  
  if (recv(client_socket, response, sizeof(response) - 1, 0) < 0) {
    perror("Receive failed");
    return 1;
  }

  printf("%s\n", response);
  
  close(client_socket);

  return 0;
}
