#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "dns.h"
#include "servers.h"
#include "slist.h"

#define NUM_TESTS 10

int test_dns(struct dns_list *dnss, struct hosts_list *hosts, int num_hosts, int num_tests);
struct timespec test_server(struct dns_list *dns, struct hosts_list *hosts, int num_hosts, int num_tests);

int main(int argc, char** argv)
{
	int option, num_hosts = 0, added_hosts = 0, num_tests = NUM_TESTS;
	struct hosts_list *servers_hosts = NULL;
	struct dns_list *servers_dns = NULL;
	while((option = getopt(argc, argv, "s:h:t:n:")) != -1) {
		switch (option) {
		case 's': //server to use
			add_dns_server(&servers_dns, optarg);
			break;
		case 'h': //hostname to search
			add_hosts_server(&servers_hosts, optarg);
			added_hosts++;
			break;
		case 't': //set number of hosts to test
			num_hosts = atoi(optarg);
			break;
		case 'n': //number of tests to perform
			num_tests = atoi(optarg);
			break;
		case '?':
		default:
			printf("Error: invalid option -%c\n", optopt);
			free_dns_list(&servers_dns);
			free_hosts_list(&servers_hosts);
			exit(1);
		}
	}
	if (num_hosts == 0 || num_hosts > NUM_HOSTNAMES+added_hosts) {
		num_hosts = NUM_HOSTNAMES + added_hosts;
	}
	for (int i = added_hosts; i < num_hosts; i++) {
		add_hosts_server(&servers_hosts, HOSTNAMES[i-added_hosts]);
	}
	for (int i = 0; i < NUM_DNS; i++) {
		add_dns_server(&servers_dns, DNS_SERVERS[i]);
	}
	test_dns(servers_dns, servers_hosts, num_hosts, num_tests);
	sort_servers(&servers_dns);
	print_servers(servers_dns);
	free_dns_list(&servers_dns);
	free_hosts_list(&servers_hosts);
	return 0;
}

int test_dns(struct dns_list *dnss, struct hosts_list *hosts, int num_hosts, int num_tests)
{
	struct dns_list *curr = dnss;
	while (curr) {
		test_server(curr, hosts, num_hosts, num_tests);
		curr = curr->next;
	}
	return 0;
}

struct timespec test_server(struct dns_list *dns, struct hosts_list *hosts, int num_hosts, int num_tests)
{
	unsigned long long avg_nsec = 0;
	dns->time.tv_sec = 0; dns->time.tv_nsec = 0;
	for (int i = 0; i < num_tests; i++) {
		struct hosts_list *curr = hosts;
		while (curr) {
			struct timespec run;
			unsigned char buf[65536];
			run = resolve(buf, curr->server, dns->server, T_A);
			if (run.tv_sec == -1)
				continue;
			dns->time.tv_sec += run.tv_sec;
			dns->time.tv_nsec += run.tv_nsec;
			if (dns->time.tv_nsec >= 1000000000) {
				dns->time.tv_sec += 1;
				dns->time.tv_nsec -= 1000000000;
			}
			curr = curr->next;
		}
	}
	avg_nsec = 1000000000*(dns->time.tv_sec%(num_hosts*num_tests))+dns->time.tv_nsec;
	dns->time.tv_sec = dns->time.tv_sec/(num_hosts*num_tests);
	dns->time.tv_nsec = avg_nsec/(num_hosts*num_tests);
	return dns->time;
}
