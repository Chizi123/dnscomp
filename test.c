#include <stdio.h>
#include <stdlib.h>
#include "dns.h"
#include <time.h>

int main(void)
{
    char* hostname = "google.com";
    char* IP = "127.0.0.1";
    int type = T_A;
    unsigned char buf[65536];
    struct timespec ret;
    int r;

    r = reachable(buf, IP);

    /* ret = resolve(buf, hostname, IP, type); */

    /* printf("%ld, %ld\n", ret.tv_sec, ret.tv_nsec); */

    /* print_packet(buf); */
    /* for (int i = 0; i < 98; i++) { */
    /*     printf("%X", buf[i]); */
    /* } */
}
