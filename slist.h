#include "time.h"

struct hosts_list {
	char* server;
	struct hosts_list* next;
};

struct dns_list {
	char* server;
	struct timespec time;
	int errors;
	struct dns_list* next;
};

int add_hosts_server(struct hosts_list** head, char* server);
int free_hosts_list(struct hosts_list** head);
int add_dns_server(struct dns_list** head, char* server);
int free_dns_list(struct dns_list** head);
int sort_servers(struct dns_list** headRef);
int print_servers(struct dns_list* head);
