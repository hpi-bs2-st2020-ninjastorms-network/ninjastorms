#include <sys/types.h>

typedef enum {
  ARP = 0x0806,
  IPv4 = 0x0800,
  IPv6 = 0x86dd
} ether_type;

struct __ethernet_frame {
  volatile uint8_t source_mac [6];
  volatile uint8_t dest_mac [6];
  volatile ether_type ether_type;
  volatile uint8_t payload[8];
} __attribute__((packed));
typedef struct __ethernet_frame ethernet_frame_t;
