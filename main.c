#include "dns.h"

int main(int argc, char** argv)
{
	 resolve("google.com", "1.1.1.1", T_A, 1);
	 return 0;
}
