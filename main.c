#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netint/in.h>
//#include <netdb.h>
#include <sys/time.h>

// DNS resource records
#define T_A 1 // IPv4 address
#define T_NS // Name Server
#define T_CNAME 5 // Cannonical name
#define T_SOA 6 // State of Authority
#define T_PTR 12 // Domain name pointer
#define T_MX 15 // Mail exchange

struct DNS_QUERY
{
	 unsigned short id;			//ID number
	 unsigned char rd :1;		//recursion
	 unsigned char aa :1;		//truncated message
	 unsigned char opcode :4;	//message purpose
	 unsigned char qr :1;		//query/response
	 unsigned char rcode :4;	//response code
	 
}

int main(int argc, char** argv)
{
	 
}

void resolve(char* hostname, char* dns_ip)
{
	 int s;
	 struct sockaddr_in dest;

	 s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	 dest.sin_family = AF_INET;
	 dest.sin_port = htons(53);
	 dest.sin_addr.s_addr = inet_addr(dns_ip);

}
