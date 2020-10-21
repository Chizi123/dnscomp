#include <stdio.h>
#include <stdlib.h>
#include "slist.h"

void split(struct dns_list *head, struct dns_list **a, struct dns_list **b);
struct dns_list *merge(struct dns_list *a, struct dns_list *b);
int comp_times(struct timespec a, struct timespec b);

int add_hosts_server(struct hosts_list **head, char *server)
{
	 struct hosts_list *end;
	 if (!(*head)) {
		  *head = malloc(sizeof(struct hosts_list));
		  end = *head;
	 } else {
		  end = *head;
		  while (end->next)
			   end = end->next;
		  end->next = malloc(sizeof(struct hosts_list));
		  end = end->next;
	 }
	 end->next = NULL;
	 end->server = server;
	 return 0;
}

int free_hosts_list(struct hosts_list **head)
{
	 struct hosts_list *temp;
	 while (*head) {
		  temp = (*head)->next;
		  free(*head);
		  *head = temp;
	 }
	 return 0;
}

int add_dns_server(struct dns_list **head, char* server)
{
	 struct dns_list *end;
	 if (!(*head)) {
		  *head = malloc(sizeof(struct dns_list));
		  end = *head;
	 } else {
		  end = *head;
		  while (end->next)
			   end = end->next;
		  end->next = malloc(sizeof(struct dns_list));
		  end = end->next;
	 }
	 end->next = NULL;
	 end->server = server;
	 end->time.tv_nsec = 0; end->time.tv_sec = 0;
	 return 0;
}

int free_dns_list(struct dns_list **head)
{
	 struct dns_list *temp;
	 while (*head) {
		  temp = (*head)->next;
		  free(*head);
		  *head = temp;
	 }
	 return 0;
}

// Sort with merge sort as works well for linked lists
// Copied from https://www.geeksforgeeks.org/merge-sort-for-linked-list/
int sort_servers(struct dns_list **headRef)
{
	struct dns_list *head = *headRef;
	struct dns_list *a, *b;
	if (!head || !(head->next)) { //Empty list or containing one element
		return 0;
	}
	split(head, &a, &b);
	sort_servers(&a);
	sort_servers(&b);
	*headRef = merge(a, b);
	return 0;
}

void split(struct dns_list *head, struct dns_list **a, struct dns_list **b)
{
	struct dns_list *fast = head->next, *slow = head;
	while(fast) {
		fast = fast->next;
		if (fast) {
			slow = slow->next;
			fast = fast->next;
		}
	}
	*a = head;
	*b = slow->next;
	slow->next = NULL;
}

struct dns_list *merge(struct dns_list *a, struct dns_list *b)
{
	struct dns_list *out = NULL;
	if (!a)
		return b;
	if (!b)
		return a;

	if (comp_times(a->time, b->time) > 0) {
		out = b;
		out->next = merge(a, b->next);
	} else {
		out = a;
		out->next = merge(a->next, b);
	}
	return out;
}

int comp_times(struct timespec a, struct timespec b)
{
	if (a.tv_sec == b.tv_sec) {
		if (a.tv_nsec >= b.tv_nsec)
			return 1;
		else
			return -1;
	} else if (a.tv_sec > b.tv_sec) {
		return 1;
	} else
		return -1;
}

int print_servers(struct dns_list *head)
{
	printf("%-20s | %s\n", "Server", "Time");
	while (head) {
		printf("%-20s | %ld.%09ld\n", head->server, head->time.tv_sec, head->time.tv_nsec);
		head = head->next;
	}
	return 0;
}
