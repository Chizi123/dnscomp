#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "dns.h"

int main(int argc, char** argv)
{
	 int option, type = T_A;
	 char* server = "1.1.1.1", *hostname = "google.com";
	 while((option = getopt(argc, argv, "s:h:t:")) != -1) {
		  switch (option) {
		  case 's': //server to use
			   server = optarg;
			   break;
		  case 'h': //hostname to search
			   hostname = optarg;
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
					type = T_MX;
			   } else {
					printf("Error: %s is not a valid DNS record type\n", optarg);
					exit(1);
			   }
			   break;
		  case '?':
		  default:
			   printf("Error: invalid option -%c\n", optopt);
			   exit(1);
		  }
	 }
	 unsigned char buf[65536];
	 resolve(buf, hostname, server, type);
	 print_packet(buf);
	 return 0;
}
