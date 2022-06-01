#include <stdio.h>
#include <stdlib.h>
#include "dns.h"
#include <time.h>
#include <netinet/ip.h>

int main(void)
{
    unsigned char buf[65536];
    int error_count;

    error_count = 0;
    for (int i = 0; i < 100; i++) {
        error_count += (reachable(buf, "1.1.1.1") != 0);
    }
    printf("1.1.1.1: %d\n", error_count);
    error_count = 0;
    for (int i = 0; i < 100; i++) {
        error_count += (reachable(buf, "192.168.0.101") != 0);
    }
    printf("192.168.0.101: %d\n", error_count);
    error_count = 0;
    for (int i = 0; i < 100; i++) {
        error_count += (reachable(buf, "192.168.0.100") != 0);
    }
    printf("192.168.0.100: %d\n", error_count);
    error_count = 0;
    for (int i = 0; i < 100; i++) {
        error_count += (reachable(buf, "129.146.153.226") != 0);
    }
    printf("129.146.153.226: %d\n", error_count);
}
