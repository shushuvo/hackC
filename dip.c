#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void printIPAddress(const char *hostname, int addressFamily) {
    struct addrinfo hints, *result, *res;
    char ipStr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = addressFamily;

    int status = getaddrinfo(hostname, NULL, &hints, &result);
    if (status != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        return;
    }

    for (res = result; res != NULL; res = res->ai_next) {
        void *addr;
        char *ipVersion;

        if (res->ai_family == AF_INET) {  // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)res->ai_addr;
            addr = &(ipv4->sin_addr);
            ipVersion = "IPv4";
        } else {  // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)res->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipVersion = "IPv6";
        }

        inet_ntop(res->ai_family, addr, ipStr, sizeof(ipStr));
        printf("%s %s: %s\n", ipVersion, hostname, ipStr);
    }

    freeaddrinfo(result);
}

int main() {
    const char *hostname = "du.ac.bd";  // Replace with your desired domain

    printf("Performing DNS enumeration for %s:\n", hostname);

    // Enumerate IPv4 addresses
    printf("IPv4 addresses:\n");
    printIPAddress(hostname, AF_INET);

    // Enumerate IPv6 addresses
    printf("\nIPv6 addresses:\n");
    printIPAddress(hostname, AF_INET6);

    return 0;
}
