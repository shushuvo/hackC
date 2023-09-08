#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <string.h>

#define MAX_SUBDOMAIN_LENGTH 10  // Maximum length of subdomain name
#define CHAR_SET "abcdefghijklmnopqrstuvwxyz0123456789-_"  // Character set for subdomain names

int main() {
    const char* domain = "sust.edu.bd";  // Replace with the desired domain name
    const char* dns_server = "108.162.194.2";  // Replace with your custom DNS server

    // Set the resolver options to use a specific DNS server
    res_init();
    _res.nsaddr_list[0].sin_addr.s_addr = inet_addr(dns_server);
    _res.nsaddr_list[0].sin_family = AF_INET;
    _res.nscount = 1;

    // Generate all possible subdomains
    size_t char_set_length = strlen(CHAR_SET);
    for (size_t len = 1; len <= MAX_SUBDOMAIN_LENGTH; len++) {
        size_t num_combinations = 1;
        for (size_t i = 0; i < len; i++) {
            num_combinations *= char_set_length;
        }

        for (size_t i = 0; i < num_combinations; i++) {
            char subdomain[MAX_SUBDOMAIN_LENGTH + 1];
            size_t temp = i;
            for (size_t j = 0; j < len; j++) {
                subdomain[j] = CHAR_SET[temp % char_set_length];
                temp /= char_set_length;
            }
            subdomain[len] = '\0';

            char full_domain[256];
            snprintf(full_domain, sizeof(full_domain), "%s.%s", subdomain, domain);

            struct addrinfo* result;
            struct addrinfo hints = {0};
            hints.ai_family = AF_INET;

            int status = getaddrinfo(full_domain, NULL, &hints, &result);
            if (status == 0) {
                // Subdomain exists, print its IP addresses
                struct addrinfo* addr = result;
                int j = 1;
                while (addr != NULL) {
                    struct sockaddr_in* addr_in = (struct sockaddr_in*)addr->ai_addr;
                    printf("IP Address for %s: %d: %s\n", full_domain, j, inet_ntoa(addr_in->sin_addr));
                    fflush(stdout);  // Flush the output buffer to display the print statement immediately
                    addr = addr->ai_next;
                    j++;
                }

                freeaddrinfo(result);
            }
        }
    }

    return 0;
}
