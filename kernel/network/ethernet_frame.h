#include <sys/types.h>

typedef enum {
    ARP = 0x0806,
    IPv4 = 0x0800
} ether_type;

struct ethernet_frame {
    uint8_t source_mac [6];
    uint8_t dest_mac [6];
    ether_type ether_type;
    uint8_t payload[8];
} __attribute__((packed));