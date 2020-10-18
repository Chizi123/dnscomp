#include "dns.h"

int main(int argc, char** argv)
{
	 unsigned char buf[65536];
	 resolve(buf, "google.com", "1.1.1.1", T_A);
	 print_packet(buf);
	 return 0;
}
