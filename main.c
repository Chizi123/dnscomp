/**
 * A basic dns benchmark testing program
 * Tests servers in parallel so reasonably fast
 * Tried to only use POSIX functions to ensure cross platform compatibility
 **/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <pthread.h>
#include "dns.h"
#include "servers.h"
#include "slist.h"

#define NUM_TESTS 10

int test_dns(void);
void *test_server(void *in);
void *print_progress(void *in);

// All global variables as passing to threads in structs caused corruption
// Variables are only modified before threads are created and as such are thread-safe
// tests_done is modified to provide a rough count of number of tests being completed for progress measurement
int tests_done = 0, num_tests = NUM_TESTS, num_servers = NUM_DNS, num_hosts = 0;
struct hosts_list *hosts = NULL;
struct dns_list *servers = NULL;

int main(int argc, char** argv)
{
	int option, added_hosts = 0;
	while((option = getopt(argc, argv, "s:a:t:n:h")) != -1) {
		switch (option) {
		case 's': //server to use
			add_dns_server(&servers, optarg);
			num_servers++;
			break;
		case 'a': //hostname to search
			add_hosts_server(&hosts, optarg);
			added_hosts++;
			break;
		case 't': //set number of hosts to test
			num_hosts = atoi(optarg);
			break;
		case 'n': //number of tests to perform
			num_tests = atoi(optarg);
			break;
		case '?':
		case 'h':
		default:
			printf("Usage: %s [options]\n", argv[0]);
			printf("Options:\n");
			printf("\t-s <server>\tadd a DNS server to be tested\n");
			printf("\t-a <hostname>\tadd a hostname to be tested\n");
			printf("\t-t <number>\tspecify the number of hostnames to be tested, capped at 50 + number manually added\n\t\t\tdefaults to test all available\n");
			printf("\t-n <number>\tspecify the number of tests to perform on each hostname\n\t\t\tdefaults to 10\n");
			printf("\t-h\t\tShow this help\n");
			free_dns_list(&servers);
			free_hosts_list(&hosts);
			exit(1);
		}
	}
	if (num_hosts == 0 || num_hosts > NUM_HOSTNAMES+added_hosts) {
		num_hosts = NUM_HOSTNAMES + added_hosts;
	}
	for (int i = added_hosts; i < num_hosts; i++) {
		add_hosts_server(&hosts, (char*)HOSTNAMES[i-added_hosts]);
	}
	for (int i = 0; i < NUM_DNS; i++) {
		add_dns_server(&servers, (char*)DNS_SERVERS[i]);
	}
	test_dns();
	sort_servers(&servers);
	print_servers(servers);
	free_dns_list(&servers);
	free_hosts_list(&hosts);
	return 0;
}

// Test each dns server individually
// Each test runs in its own thread and results are written to the respective dns_list
int test_dns(void)
{
	struct dns_list *curr = servers;
	int i = 0;
	pthread_t *threads = malloc(num_servers*sizeof(pthread_t));
	pthread_t progress;
	pthread_create(&progress, NULL, print_progress, NULL);
	while (curr) {
		pthread_create(&threads[i], NULL, test_server, (void*)curr);
		curr = curr->next;
		i++;
	}
	for (int i = 0; i < num_servers; i++) {
		pthread_join(threads[i], NULL);
	}
	pthread_cancel(progress);
	printf("\r100.00%% done\n");
	free(threads);
	return 0;
}

// Tests an individual dns server with all the hostnames configured
void *test_server(void *in)
{
	unsigned long long avg_nsec = 0;
	struct dns_list *dns = (struct dns_list *)in;
	dns->time.tv_sec = 0; dns->time.tv_nsec = 0;
	for (int i = 0; i < num_tests; i++) {
		struct hosts_list *curr = hosts;
		while (curr) {
			struct timespec run;
			run.tv_sec = -1; run.tv_nsec = 0;
			unsigned char buf[65536];
			for (int j = 0; j < 3 && run.tv_sec == -1; j++) {
				run = resolve(buf, curr->server, dns->server, T_A);
			}
			if (run.tv_sec == -1) //if test has failed 3 times, set time taken to 3s as penalty
				run.tv_sec = 3;
			dns->time.tv_sec += run.tv_sec;
			dns->time.tv_nsec += run.tv_nsec;
			if (dns->time.tv_nsec >= 1000000000) { //nanoseconds have overflowed into seconds
				dns->time.tv_sec += 1;
				dns->time.tv_nsec -= 1000000000;
			}
			tests_done++;
			curr = curr->next;
		}
	}
	avg_nsec = 1000000000*(dns->time.tv_sec%(num_hosts*num_tests))+dns->time.tv_nsec;
	dns->time.tv_sec = dns->time.tv_sec/(num_hosts*num_tests);
	dns->time.tv_nsec = avg_nsec/(num_hosts*num_tests);
	return NULL;
}

// Prints the progress every 0.1s for an indication of speed
// tests_done is being written to in parallel, so may be overwritten but serves as a decent estimate of the progress
void *print_progress(void *in)
{
	struct timespec s;
	s.tv_sec = 0; s.tv_nsec = 100000000;
	while (1) {
		printf("\r%.2f%% done", ((float)tests_done)/(num_servers*num_hosts*num_tests) * 100);
		fflush(stdout);
		nanosleep(&s, NULL);
	}
	return NULL;
}
