#include "dns.h"
#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <bits/time.h>
#include <errno.h>
#include <linux/if_ether.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

void change_to_DNS_name_format(unsigned char* dns, unsigned char* host);
char* read_name(unsigned char* reader, unsigned char* buffer, int* count);
int fill_DNS_data(unsigned char* buf, int datasize, char* hostname,
                  int query_type);

// DNS code copied from
// https://gist.github.com/fffaraz/9d9170b57791c28ccda9255b48315168

/* DNS header struct */
struct DNS_HEADER {
	unsigned short id;         // ID number
	unsigned char rd : 1;      // recursion
	unsigned char tc : 1;      // truncated message
	unsigned char aa : 1;      // authoritive answer
	unsigned char opcode : 4;  // message purpose
	unsigned char qr : 1;      // query response
	unsigned char rcode : 4;   // response code
	unsigned char cd : 1;      // checking disabled
	unsigned char ad : 1;      // authenticated data
	unsigned char z : 1;       // reserved for future use
	unsigned char ra : 1;      // recursion available
	unsigned short q_count;    // number of question entrise
	unsigned short ans_count;  // number of answer entries
	unsigned short auth_count; // number of authority entries
	unsigned short add_count;  // number of resource entries
};

/* structured for query structure */
struct QUESTION {
	unsigned short qtype;
	unsigned short qclass;
};

/* Query structure */
struct QUERY {
	unsigned char* name;
	struct QUESTION* ques;
};

/* Constant sized fields of record structure */
#pragma pack(push, 1)
struct R_DATA {
	unsigned short type;
	unsigned short _class;
	unsigned int ttl;
	unsigned short data_len;
};
#pragma pack(pop)

/* Pointers to record components */
struct RES_RECORD {
	char* name;
	struct R_DATA resource;
	char* rdata;
};

#define IP_ICMP 1
#define IP_TCP 6
#define IP_UDP 17

// Test if an IP address is hosting a DNS server
int reachable(unsigned char* buf, char* dns_ip)
{
	int s, r, ret, name_len;
	struct sockaddr_in dest;
	socklen_t dest_len = sizeof(dest);
	struct timespec timeout, start, end;
	struct icmphdr* icmp_head;
	struct ip* ip_head;
    unsigned char buf_send[65535];
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;

	dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	dest.sin_addr.s_addr = inet_addr(dns_ip);

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	/* r = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP); */
	r = socket(AF_PACKET, SOCK_DGRAM, htons(ETH_P_ALL));
	if (r < 0) {
		printf("%d, %s\n", r, strerror(errno));
		return -1;
	}
	setsockopt(r, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(struct timespec));

	name_len = fill_DNS_data((unsigned char*)&buf_send, 65535, "google.com", T_A);
	ret = sendto(s, buf_send,
	             sizeof(struct DNS_HEADER) + name_len + 1 +
	                 sizeof(struct QUESTION),
	             0, (struct sockaddr*)&dest, sizeof(dest));

    clock_gettime(CLOCK_MONOTONIC, &start);
    do {
        ret = recvfrom(r, buf, 65535, 0, (struct sockaddr*)&dest, &dest_len);
        clock_gettime(CLOCK_MONOTONIC, &end);
        if (!memcmp(&buf_send, buf, 100)) {
            printf("*1\n");
            break;
        }
    } while (start.tv_sec + 2 > end.tv_sec);

	close(s);
	close(r);

    printf("diff: %d\n", memcmp(&buf_send, buf, 100));

	ip_head = (struct ip*)buf;
	icmp_head = (struct icmphdr*)buf + sizeof(struct ip);

	for (int i = 0; i < 93; i++) {
		printf("%02X", buf[i]);
	}
	printf("\n");

	printf("len: %d, ver: %d, tos: %d, tlen: %d, ident: %d, ff: %d, ttl: %d, "
	       "pro: %d, cs: %d, sip: %d, dip: %d\n",
	       ip_head->ip_hl, ip_head->ip_v, ip_head->ip_tos, ip_head->ip_len,
	       ip_head->ip_id, ip_head->ip_off, ip_head->ip_ttl, ip_head->ip_p,
	       ip_head->ip_sum, ip_head->ip_src.s_addr, ip_head->ip_dst.s_addr);

	if (ip_head->ip_p != IP_UDP) {
		if (ip_head->ip_p == IP_ICMP) {
			return 1;
		} else {
			printf("%d\n", ip_head->ip_p);
			for (int i = 0; i < sizeof(struct ip); i++) {
				printf("%02X", buf[i]);
			}
			printf("\n");
			return 2;
		}
	}
	return 0;
}

// Test server dns_ip as IPv4 string for hostname
// Writes received packet to buf, which is supplied and returns time for request
struct timespec resolve(unsigned char* buf, char* hostname, char* dns_ip,
                        int query_type)
{
	int s, i, name_len;
	struct sockaddr_in dest;
	socklen_t dest_len = sizeof(dest);
	struct timespec start, end, total, timeout;
	timeout.tv_nsec = 0;
	timeout.tv_sec = 1;

	s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	setsockopt(
		s, SOL_SOCKET, SO_RCVTIMEO, &timeout,
		sizeof(struct timespec)); // use a 1 second timeout for receiving,
	                              // should be more than enough and anything
	                              // more is really bad
	dest.sin_family = AF_INET;
	dest.sin_port = htons(53);
	dest.sin_addr.s_addr = inet_addr(dns_ip);

	name_len = fill_DNS_data(buf, 65535, hostname, query_type);

	// send request
	//  return less than 0 is a fail
	clock_gettime(CLOCK_MONOTONIC, &start);
	i = sendto(s, (char*)buf,
	           sizeof(struct DNS_HEADER) + name_len + 1 +
	               sizeof(struct QUESTION),
	           0, (struct sockaddr*)&dest, sizeof(dest));

	if (i >= 0) {
		// receive response
		// negative return is a fail
		i = sizeof(dest);
		i = recvfrom(s, (char*)buf, 65536, 0, (struct sockaddr*)&dest,
		             &dest_len);
		clock_gettime(CLOCK_MONOTONIC, &end);
	}

	// Make sure packet was returned
	if (i == -1)
		total.tv_nsec = -1;
	else
		total.tv_sec = end.tv_sec - start.tv_sec;
	if ((end.tv_nsec - start.tv_nsec) < 0)
		total.tv_nsec = start.tv_nsec - end.tv_nsec;
	else
		total.tv_nsec = end.tv_nsec - start.tv_nsec;
	close(s);
	return total;
}

// Print dns packet content, not terribly reliable but works for testing resolve
// with A requests
void print_packet(unsigned char* buf)
{
	struct RES_RECORD answers[20], auth[20], addit[20];
	struct DNS_HEADER* dns;
	struct sockaddr_in a;
	unsigned char *qname = buf + sizeof(struct DNS_HEADER), *reader;
	int stop, i, j;
	dns = (struct DNS_HEADER*)buf;
	reader = &buf[sizeof(struct DNS_HEADER) + strlen((const char*)qname) + 1 +
	              sizeof(struct QUESTION)];
	printf("Response contains %d Qs, %d ans, %d auth serv, %d add reconds\n",
	       ntohs(dns->q_count), ntohs(dns->ans_count), ntohs(dns->auth_count),
	       ntohs(dns->add_count));
	stop = 0;

	// read answers
	for (i = 0; i < ntohs(dns->ans_count); i++) {
		answers[i].name = read_name(reader, buf, &stop);
		reader = reader + stop;
		memcpy(&(answers[i].resource), reader, sizeof(struct R_DATA));
		reader = reader + sizeof(struct R_DATA);

		if (ntohs(answers[i].resource.type) == T_A) { // IPv4 address
			answers[i].rdata =
				(char*)malloc(ntohs(answers[i].resource.data_len));
			for (j = 0; j < ntohs(answers[i].resource.data_len); j++) {
				answers[i].rdata[j] = reader[j];
			}
			answers[i].rdata[ntohs(answers[i].resource.data_len)] = '\0';
			reader = reader + ntohs(answers[i].resource.data_len);
		} else {
			answers[i].rdata = read_name(reader, buf, &stop);
			reader = reader + stop;
		}
	}

	// read authorities
	for (i = 0; i < ntohs(dns->auth_count); i++) {
		auth[i].name = read_name(reader, buf, &stop);
		reader += stop;
		memcpy(&(auth[i].resource), reader, sizeof(struct R_DATA));
		reader += sizeof(struct R_DATA);
		auth[i].rdata = read_name(reader, buf, &stop);
		reader += stop;
	}

	// read additional
	for (i = 0; i < ntohs(dns->add_count); i++) {
		addit[i].name = read_name(reader, buf, &stop);
		reader += stop;
		memcpy(&(addit[i].resource), reader, sizeof(struct R_DATA));
		reader += sizeof(struct R_DATA);

		if (ntohs(addit[i].resource.type) == 1) {
			addit[i].rdata = malloc(ntohs(addit[i].resource.data_len));
			for (j = 0; j < ntohs(addit[i].resource.data_len); j++)
				addit[i].rdata[j] = reader[j];
			addit[i].rdata[ntohs(addit[i].resource.data_len)] = '\0';
			reader += ntohs(addit[i].resource.data_len);
		} else {
			addit[i].rdata = read_name(reader, buf, &stop);
			reader += stop;
		}
	}

	// print answers
	printf("ans recs: %d\n", ntohs(dns->ans_count));
	for (i = 0; i < ntohs(dns->ans_count); i++) {
		printf("name: %s ", answers[i].name);
		if (ntohs(answers[i].resource.type) == T_A) { // IPv4
			long* p;
			p = (long*)answers[i].rdata;
			a.sin_addr.s_addr = (*p);
			printf("has IPv4 addresss: %s", inet_ntoa(a.sin_addr));
		} else if (ntohs(answers[i].resource.type) == T_CNAME) { // CNAME
			printf("has alias: %s", answers[i].rdata);
		}
		free(answers[i].name);
		free(answers[i].rdata);
		putc('\n', stdout);
	}

	// print authorities
	printf("Auth recs: %d\n", ntohs(dns->auth_count));
	for (i = 0; i < ntohs(dns->auth_count); i++) {
		printf("name: %s ", auth[i].name);
		if (ntohs(auth[i].resource.type) == 1) {
			long* p;
			p = (long*)auth[i].rdata;
			a.sin_addr.s_addr = *p;
			printf("has IPv4 address: %s", inet_ntoa(a.sin_addr));
		}
		free(answers[i].name);
		free(auth[i].rdata);
		putc('\n', stdout);
	}

	printf("Addit recs: %d\n", ntohs(dns->add_count));
	for (i = 0; i < ntohs(dns->add_count); i++) {
		printf("name; %s", addit[i].name);
		if (ntohs(auth[i].resource.type) == 1) {
			long* p;
			p = (long*)addit[i].rdata;
			a.sin_addr.s_addr = *p;
			printf("has IPv4 address: %s", inet_ntoa(a.sin_addr));
		} else {
			printf("has %s", addit[i].rdata);
		}
		free(answers[i].name);
		free(addit[i].rdata);
		putc('\n', stdout);
	}
}

// convert from dot format to dns format
// eg google.com to 6google3com
void change_to_DNS_name_format(unsigned char* dns, unsigned char* host)
{
	int lock = 0;
	char h[300];
	strcpy(h, (char*)host);
	strcat((char*)h, ".");

	for (int i = 0; i < strlen(h); i++) {
		if (h[i] == '.') {
			*dns++ = i - lock;
			for (; lock < i; lock++) {
				*dns++ = h[lock];
			}
			lock++;
		}
	}
	*dns++ = '\0';
}

// Convert from dns to dot format
char* read_name(unsigned char* reader, unsigned char* buffer, int* count)
{
	char* name;
	unsigned int p = 0, jumped = 0, offset;
	int i, j;
	*count = 1;
	name = (char*)malloc(256);
	name[0] = '\0';

	while (*reader != 0) {
		if (*reader >= 192) {
			offset = (*reader) * 256 + *(reader + 1) - 49152;
			reader = buffer + offset - 1;
			jumped = 1;
		} else {
			name[p++] = *reader;
		}
		reader = reader + 1;
		if (jumped == 0) {
			*count = *count + 1;
		}
	}
	name[p] = '\0';
	if (jumped == 1) {
		*count = *count + 1;
	}

	// convert from dns format to normal
	for (i = 0; i < strlen((const char*)name); i++) {
		p = name[i];
		for (j = 0; j < p; j++) {
			name[i] = name[i + 1];
			i++;
		}
		name[i] = '.';
	}
	name[i - 1] = '\0';
	return name;
}

// Populate the sending DNS packet with data
int fill_DNS_data(unsigned char* buf, int datasize, char* hostname,
                  int query_type)
{
	struct DNS_HEADER* dns = (struct DNS_HEADER*)buf;
	struct QUESTION* qinfo;
	unsigned char* qname;

	// dns packet header
	dns->id = (unsigned short)htons(getpid());
	dns->qr = 0;     // make query
	dns->opcode = 0; // standard query
	dns->aa = 0;     // not authoritive
	dns->tc = 0;     // not trucated
	dns->rd = 1;     // want recursion
	dns->ra = 0;     // recursion not available
	dns->z = 0;
	dns->ad = 0;
	dns->cd = 0;
	dns->rcode = 0;
	dns->q_count = htons(1); // one question
	dns->ans_count = 0;
	dns->auth_count = 0;
	dns->add_count = 0;

	// dns packet query
	qname = (unsigned char*)&buf[sizeof(struct DNS_HEADER)];
	change_to_DNS_name_format(qname, (unsigned char*)hostname);
	qinfo = (struct QUESTION*)&buf[sizeof(struct DNS_HEADER) +
	                               strlen((const char*)qname) + 1];
	qinfo->qtype = htons(
		query_type); // type of query from argument (A,AAAA,MX,CNAME,NS,...)
	qinfo->qclass = htons(1); // internet class

	return strlen((const char*)qname);
}
