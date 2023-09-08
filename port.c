#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    // Define the target host and port number
    const char* targetHost = "103.157.135.60";  // Replace with the desired host IP or domain name
    int targetPort = 100;  // Replace with the desired port number

    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up the server address structure
    struct sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(targetPort);
    inet_pton(AF_INET, targetHost, &(serverAddress.sin_addr));

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        printf("Port %d is closed on host %s\n", targetPort, targetHost);
    } else {
        printf("Port %d is open on host %s\n", targetPort, targetHost);
        // Port is open - you can perform further actions here if needed
        // For example, you can send/receive data to/from the open port
    }

    // Close the socket
    close(sockfd);

    return 0;
}
