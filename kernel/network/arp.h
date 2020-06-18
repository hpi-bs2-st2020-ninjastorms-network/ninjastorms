#include <sys/types.h>
#include "ethernet_frame.h" 

// OPCODES
#define ARP_REQUEST = 0x0001;
#define ARP_RESPONSE = 0x0002;

// https://wiki.osdev.org/ARP
struct arp {
  volatile uint16_t hardware_type;
  volatile uint16_t protocol_type;
  volatile uint16_t opcode; // ARP OP Code: see above

  volatile uint8_t src_mac_addr[6];
  volatile uint8_t dest_mac_addr[6];

  volatile uint32_t src_ip_address[4];
  volatile uint32_t dest_ip_address[4];
} __attribute__((packed));

void process_arp(const ethernet_frame_t* frame);
