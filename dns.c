#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define DNS_SERVER "8.8.8.8"  // Google DNS server
#define DNS_PORT 53

// DNS header structure
struct DNSHeader {
    unsigned short id;      // ID
    unsigned short flags;   // Flags
    unsigned short qcount;  // Question count
    unsigned short ancount; // Answer count
    unsigned short nscount; // Authority (Name Server) count
    unsigned short arcount; // Additional record count
};

// DNS question structure
struct DNSQuestion {
    unsigned short qtype;  // Query type (e.g., A, AAAA, MX)
    unsigned short qclass; // Query class (usually IN for internet)
};
// DNS answer structure
struct DNSAnswer {
    unsigned short name;
    unsigned short type;
    unsigned short class;
    unsigned int ttl;
    unsigned short dataLength;
    unsigned char *data;
};
int main() {
    // Create a socket for DNS communication
    int socket_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if (socket_fd < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Set up the destination DNS server
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DNS_PORT);
    if (inet_pton(AF_INET, DNS_SERVER, &(server_addr.sin_addr)) <= 0) {
        perror("Invalid DNS server address");
        close(socket_fd);
        return 1;
    }

    // Prepare the DNS query packet
    struct DNSHeader header;
    memset(&header, 0, sizeof(header));
    header.id = htons(1234);  // Random ID
    header.flags = htons(0x0100);  // Standard query with recursion
    header.qcount = htons(255);  // Only one question

    struct DNSQuestion question;
    memset(&question, 0, sizeof(question));
    question.qtype = htons(255);  // Query type: A (IPv4)
    question.qclass = htons(255); // Query class: IN (Internet)

    // Combine the header and question into the query buffer
    unsigned char query[1024];
    unsigned char *qbuffer = query;
    memcpy(qbuffer, &header, sizeof(header));
    qbuffer += sizeof(header);
    // Assuming the domain name you want to query is "example.com"
    const char *domain = "du.ac.bd";
    while (*domain) {
        if (*domain == '.') {
            *qbuffer++ = qbuffer - query - 1;
        } else {
            *qbuffer++ = *domain;
        }
        domain++;
    }
    *qbuffer++ = 0x00;  // End of domain name
    memcpy(qbuffer, &question, sizeof(question));
    qbuffer += sizeof(question);

    // Send the DNS query
    if (sendto(socket_fd, query, qbuffer - query, 0,
               (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Failed to send DNS query");
        close(socket_fd);
        return 1;
    }

    // Receive the DNS response
    unsigned char response[4096];
    struct sockaddr_in response_addr;
    socklen_t addr_length = sizeof(response_addr);
    ssize_t response_len = recvfrom(socket_fd, response, sizeof(response), 0,
                                    (struct sockaddr *)&response_addr, &addr_length);
    if (response_len < 0) {
        perror("Failed to receive DNS response");
        close(socket_fd);
        return 1;
    }

    // Print the received DNS response
    printf("Received DNS response (%zd bytes):\n", response_len);
    for (int i = 0; i < response_len; i++) {
        printf("%02X ", response[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    printf("\n");


// Function to print the answer section of a DNS response
void printAnswerSection(unsigned char *response, ssize_t response_len) {
    // Skip the DNS header (12 bytes) to reach the answer section
    unsigned char *answerSection = response + 12;

    // Iterate over the answer section until the end of the response
    while (answerSection < response + response_len) {
        // Extract the relevant fields from the answer record
        struct DNSAnswer *answer = (struct DNSAnswer *)answerSection;

        // Print the fields of the answer record
        printf("Name: %04X\n", ntohs(answer->name));
        printf("Type: %04X\n", ntohs(answer->type));
        printf("Class: %04X\n", ntohs(answer->class));
        printf("TTL: %08X\n", ntohl(answer->ttl));
        printf("Data Length: %04X\n", ntohs(answer->dataLength));

        // Move to the next answer record
        answerSection += 12 + ntohs(answer->dataLength);
    }
}

// Call the function to print the answer section
printAnswerSection(response, response_len);


// Receive the DNS response
unsigned char response2[4096];
// ... (existing code)

// Define the maximum number of answer records
#define MAX_ANSWERS 10
struct DNSAnswer answers[MAX_ANSWERS];
int numAnswers = 0;

// Parse the DNS response and extract answer records
unsigned char *responsePtr = response2;
while (responsePtr < response2 + response_len) {
    // Extract the fields from the response
    struct DNSAnswer answer;
    answer.name = ntohs(*((unsigned short *)responsePtr));
    responsePtr += 2;
    answer.type = ntohs(*((unsigned short *)responsePtr));
    responsePtr += 2;
    answer.class = ntohs(*((unsigned short *)responsePtr));
    responsePtr += 2;
    answer.ttl = ntohl(*((unsigned int *)responsePtr));
    responsePtr += 4;
    answer.dataLength = ntohs(*((unsigned short *)responsePtr));
    responsePtr += 2;
    answer.data = responsePtr;
    responsePtr += answer.dataLength;

    // Store the answer in the array
    if (numAnswers < MAX_ANSWERS) {
        answers[numAnswers] = answer;
        numAnswers++;
    }
}

// Print the extracted answer records
for (int i = 0; i < numAnswers; i++) {
    printf("Answer %d:\n", i + 1);
    printf("Name: %04X\n", answers[i].name);
    printf("Type: %04X\n", answers[i].type);
    printf("Class: %04X\n", answers[i].class);
    printf("TTL: %08X\n", answers[i].ttl);
    printf("Data Length: %04X\n", answers[i].dataLength);
    printf("Data: ");
    for (int j = 0; j < answers[i].dataLength; j++) {
        printf("%02X ", answers[i].data[j]);
    }
    printf("\n");
}





    // Close the socket
    close(socket_fd);

    return 0;
}

