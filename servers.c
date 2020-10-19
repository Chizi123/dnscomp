#include <stdlib.h>
#include "slist.h"

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
