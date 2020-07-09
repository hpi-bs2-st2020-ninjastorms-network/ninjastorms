#include "pdu_handler.h"
#include "kernel/network/network_io.h"
#include "kernel/logger/logger.h"

void 
start_pdu_encapsulation(uint8_t* buf) 
{
  ethernet_frame_t* frame = (ethernet_frame_t*) buf;
  
  uint16_t ether_type = switch_endian16(frame->ether_type);
  
  // we don't wanna support ipv6 yet ;)
  if(ether_type == ARP)
    {
      log_debug("Found ARP packet.")
      log_debug("Dest Mac: %x", frame->dest_mac);
      log_debug("Src mac: %x", frame->source_mac);
    }
  else if(ether_type == IPv4) 
    {
      // log_debug("Found IPv4 packet.")

    }

}

void 
handle_arp(ethernet_frame_t frame) 
{
  // not yet 
}

void
print_mac(uint8_t* mac_address)
{ 
  // TODO
  printf("%x", mac_address[0]);
  for(uint16_t i = 1; i < 6; i++)
    {
      printf(":%x", mac_address[i]);
    }
  printf("\n");
}