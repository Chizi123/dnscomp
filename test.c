#include <stdio.h>
#include <stdlib.h>
#include "dns.h"
#include <time.h>
#include <netinet/ip.h>

int main(void)
{
    char* hostname = "google.com";
    char* IP = "127.0.0.1";
    int type = T_A;
    unsigned char buf[65536];
    struct timespec ret;
    int r;

    r = reachable(buf, "10.0.0.0");
    printf("%d\n", r);
    /* print_packet(buf + sizeof(struct ip)); */
    r = reachable(buf, "127.0.0.2");
    printf("%d\n", r);

    /* ret = resolve(buf, hostname, IP, type); */

    /* printf("%ld, %ld\n", ret.tv_sec, ret.tv_nsec); */

    /* print_packet(buf); */
    /* for (int i = 0; i < 98; i++) { */
    /*     printf("%X", buf[i]); */
    /* } */
}
