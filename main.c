#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "dns.h"
#include "servers.h"
#include "slist.h"

#define NUM_TESTS 10

struct timespec test_server(char *dns_addr, struct hosts_list *hosts);

int main(int argc, char** argv)
{
	int option, type = T_A;
	char* server = "1.1.1.1", *hostname = "google.com";
	struct hosts_list *servers_hosts = NULL;
	struct dns_list *servers_dns = NULL;
	while((option = getopt(argc, argv, "s:h:t:")) != -1) {
		switch (option) {
		case 's': //server to use
			add_dns_server(&servers_dns, optarg);
			break;
		case 'h': //hostname to search
			add_hosts_server(&servers_hosts, optarg);
			break;
		case 't':
			if (!strcmp(optarg, "A")) {
				type = T_A;
			} else if (!strcmp(optarg, "AAAA")) {
				type = T_AAAA;
			} else if (!strcmp(optarg, "NS")) {
				type = T_NS;
			} else if (!strcmp(optarg, "CNAME")) {
				type = T_CNAME;
			} else if (!strcmp(optarg, "SOA")) {
				type = T_SOA;
			} else if (!strcmp(optarg, "PTR")) {
				type = T_PTR;
			} else if (!strcmp(optarg, "MX")) {
				type = T_MX;
			} else if (!strcmp(optarg, "TXT")) {
				type = T_TXT;
			} else {
				printf("Error: %s is not a valid DNS record type\n", optarg);
				free_hosts_list(&servers_hosts);
				free_dns_list(&servers_dns);
				exit(1);
			}
			break;
		case '?':
		default:
			printf("Error: invalid option -%c\n", optopt);
			exit(1);
		}
	}
	struct timespec total;
	total = test_server(server, servers_hosts);
	printf("Test took %ld.%09lds\n", total.tv_sec, total.tv_nsec);
	free_dns_list(&servers_dns);
	free_hosts_list(&servers_hosts);
	return 0;
}

struct timespec test_server(char* dns_addr, struct hosts_list *hosts)
{
	struct timespec total;
	total.tv_sec = 0; total.tv_nsec = 0;
	for (int i = 0; i < NUM_TESTS; i++) {
		struct hosts_list *curr = hosts;
		while (curr) {
			struct timespec run;
			unsigned char buf[65536];
			run = resolve(buf, curr->server, dns_addr, T_A);
			total.tv_sec += run.tv_sec;
			total.tv_nsec += run.tv_nsec;
			if (total.tv_nsec >= 1000000000) {
				total.tv_sec += 1;
				total.tv_nsec -= 1000000000;
			}
			curr = curr->next;
		}
	}
	return total;
}
