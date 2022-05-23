// DNS resource records
#define T_A 1     // IPv4 address
#define T_NS 2    // Name Server
#define T_CNAME 5 // Cannonical name
#define T_SOA 6   // State of Authority
#define T_PTR 12  // Domain name pointer
#define T_MX 15   // Mail exchange
#define T_TXT 16  // Text record
#define T_AAAA 28 // IPv6 address

struct timespec resolve(unsigned char* buf, char* hostname, char* dns_ip,
                        int query_type);
void print_packet(unsigned char* buf);
