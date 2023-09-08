#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PACKET_SIZE 64
#define MAX_IP_COUNT 100

// ICMP packet structure
struct icmp_packet {
    struct icmphdr header;
    char data[PACKET_SIZE - sizeof(struct icmphdr)];
};

// Calculate the time difference between two timeval structures
long long timeval_diff(struct timeval *start, struct timeval *end) {
    return ((end->tv_sec - start->tv_sec) * 1000LL + (end->tv_usec - start->tv_usec) / 1000LL);
}

// Send ICMP echo request to the specified IP address
void send_icmp_request(int sockfd, struct sockaddr_in *targetAddress) {
    // Create ICMP packet
    struct icmp_packet packet;
    memset(&packet, 0, sizeof(struct icmp_packet));
    packet.header.type = ICMP_ECHO;
    packet.header.code = 0;
    packet.header.un.echo.id = getpid();

    // Calculate checksum for ICMP packet
    packet.header.checksum = 0;
    packet.header.checksum = htons(~(packet.header.type + packet.header.code + htons(packet.header.checksum)));

    // Send ICMP packet
    if (sendto(sockfd, &packet, sizeof(struct icmp_packet), 0, (struct sockaddr*)targetAddress, sizeof(struct sockaddr_in)) <= 0) {
        perror("Failed to send ICMP request");
        exit(EXIT_FAILURE);
    }
}

// Receive ICMP echo reply and check if it matches the sent request
int receive_icmp_reply(int sockfd, struct sockaddr_in *targetAddress, struct timeval *startTime) {
    struct sockaddr_in responseAddress;
    socklen_t responseLength = sizeof(responseAddress);
    unsigned char buffer[PACKET_SIZE];

    // Receive ICMP reply
    ssize_t bytesRead = recvfrom(sockfd, buffer, PACKET_SIZE, 0, (struct sockaddr*)&responseAddress, &responseLength);
    if (bytesRead <= 0) {
        perror("Failed to receive ICMP reply");
        return 0;
    }

    // Check if the received ICMP reply matches the sent request
    struct ip *ipHeader = (struct ip*)buffer;
    struct icmphdr *icmpHeader = (struct icmphdr*)(buffer + (ipHeader->ip_hl << 2));
    if (icmpHeader->type == ICMP_ECHOREPLY && icmpHeader->un.echo.id == getpid()) {
        struct timeval endTime;
        gettimeofday(&endTime, NULL);

        // Calculate round-trip time
        long long rtt = timeval_diff(startTime, &endTime);
        printf("IP address %s is active (RTT: %lld ms)\n", inet_ntoa(targetAddress->sin_addr), rtt);
        return 1;
    }

    return 0;
}

int main() {
    // Open the file containing IP addresses
    FILE *file = fopen("ip.txt", "r");
    if (file == NULL) {
        perror("Failed to open the file");
        exit(EXIT_FAILURE);
    }

    char ipAddress[16];  // Assuming IP addresses are in the format xxx.xxx.xxx.xxx
    char *ipAddresses[MAX_IP_COUNT];
    int ipCount = 0;

    // Read IP addresses from the file until the end is reached or maximum IP count is reached
    while (fscanf(file, "%s", ipAddress) == 1 && ipCount < MAX_IP_COUNT) {
        ipAddresses[ipCount] = strdup(ipAddress);
        ipCount++;
    }

    // Close the file
    fclose(file);

    // Create raw socket for ICMP
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Failed to create raw socket");
        exit(EXIT_FAILURE);
    }

    // Set socket timeout to receive responses
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&timeout, sizeof(timeout)) < 0) {
        perror("Failed to set socket timeout");
        exit(EXIT_FAILURE);
    }

    // Iterate over the IP addresses and send ICMP echo request
    for (int i = 0; i < ipCount; i++) {
        // Create target address structure
        struct sockaddr_in targetAddress;
        memset(&targetAddress, 0, sizeof(struct sockaddr_in));
        targetAddress.sin_family = AF_INET;
        inet_pton(AF_INET, ipAddresses[i], &(targetAddress.sin_addr));

        // Send ICMP echo request
        send_icmp_request(sockfd, &targetAddress);

        struct timeval startTime;
        gettimeofday(&startTime, NULL);

        // Receive ICMP echo reply
        if (receive_icmp_reply(sockfd, &targetAddress, &startTime)) {
            // Perform further actions (e.g., check port) for active IP address
            // ...
        }
    }

    // Close the socket
    close(sockfd);

    // Free allocated memory for IP addresses
    for (int i = 0; i < ipCount; i++) {
        free(ipAddresses[i]);
    }

    return 0;
}

