#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

void findSubdomains(const char *domain) {
    // DNS query type: A (IPv4 address)
    int queryType = AF_INET;

    struct hostent *host = gethostbyname(domain);
    if (host == NULL) {
        herror("DNS resolution failed");
        return;
    }

    // Iterate over the aliases
    char **alias = host->h_aliases;
    while (*alias) {
        printf("Alias: %s\n", *alias);
        alias++;
    }

    // Iterate over the IP addresses
    char **ip = host->h_addr_list;
    while (*ip) {
        struct sockaddr_in addr;
        char ipStr[INET_ADDRSTRLEN];

        memcpy(&addr.sin_addr, *ip, sizeof(struct in_addr));
        inet_ntop(AF_INET, &(addr.sin_addr), ipStr, INET_ADDRSTRLEN);
        printf("IP: %s\n", ipStr);

        ip++;
    }
}

int main() {
    const char *domain = "du.ac.bd";  // Replace with your desired domain

    printf("Finding subdomains of %s:\n", domain);
    findSubdomains(domain);

    return 0;
}
