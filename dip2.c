#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>

int main() {
    const char* domain = "example.com";  // Replace with the desired domain name

    // Resolve the domain to obtain host information
    struct hostent* host = gethostbyname(domain);
    if (host == NULL) {
        printf("Failed to resolve the domain.\n");
        exit(EXIT_FAILURE);
    }

    // Extract and print the IP addresses associated with the domain
    struct in_addr** addr_list = (struct in_addr**)host->h_addr_list;
    for (int i = 0; addr_list[i] != NULL; i++) {
        printf("IP Address %d: %s\n", i+1, inet_ntoa(*addr_list[i]));
    }

    return 0;
}

