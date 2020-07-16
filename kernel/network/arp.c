
/******************************************************************************
 *       ninjastorms - shuriken operating system                              *
 *                                                                            *
 *    Copyright (C) 2013 - 2016  Andreas Grapentin et al.                     *
 *                                                                            *
 *    This program is free software: you can redistribute it and/or modify    *
 *    it under the terms of the GNU General Public License as published by    *
 *    the Free Software Foundation, either version 3 of the License, or       *
 *    (at your option) any later version.                                     *
 *                                                                            *
 *    This program is distributed in the hope that it will be useful,         *
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *    GNU General Public License for more details.                            *
 *                                                                            *
 *    You should have received a copy of the GNU General Public License       *
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.   *
 ******************************************************************************/

#include "arp.h"
#include "kernel/logger/logger.h"
#include "kernel/network/ethernet.h"
#include "kernel/network/e1000.h"
#include "kernel/network/ipv4.h"
#include "kernel/network/network_io.h"

#include <stdlib.h>
#include <string.h>

void
arp_receive(ethernet_frame_t *frame) 
{

  arp_frame_t *arp_frame = (arp_frame_t*) frame->payload;

  if(ntohs(arp_frame->hardware_type) != HTYPE_ETHERNET || arp_frame->hardware_addr_len != ETH_MAC_ADDRESS_LENGTH)
    {
      log_warn("Hardware type is not supported! (no ethernet) hwtype: %x addr_len: %x", arp_frame->hardware_type, arp_frame->hardware_addr_len)
      return;
    }
  if(ntohs(arp_frame->protocol_type) != TYPE_IPv4 || arp_frame->protocol_addr_len != IPV4_ADDR_LEN)
    {
      log_warn("Protocol type is not supported! (no ipv4)");
      return;
    }

  log_debug("Found ARP packet.");
  log_debug("Destination HW: %y", ntoh_mac(arp_frame->dest_hardware_addr));
  log_debug("Source HW: %y", ntoh_mac(arp_frame->src_hardware_addr);
  
  switch(ntohs(arp_frame->opcode))
    {
      case ARP_REQUEST:
        arp_handle_request(arp_frame);
        break;
      case ARP_REPLY:
        arp_handle_reply(arp_frame);
        break;
      default:
        break;
    }
}

void
arp_handle_request(arp_frame_t *frame)
{
  if(ntohl(frame->dest_ip_address) == OWN_IPV4_ADDR)
    {
      // ITS ME
      log_debug("I have been requested... let's answer.");

      uint64_t src_hw_address = ntoh_mac(arp_frame->src_hardware_addr);

      arp_frame_t arp_frame = arp_build_frame(
        arp_frame,
        ARP_REPLY,
        ntoh_mac(frame->src_hardware_addr), // use original sender as destination
        ntohl(frame->src_ip_address)
      );

      send_ethernet(src_hw_address, TYPE_ARP, &arp_frame, sizeof(arp_frame_t));
    }
}

void
arp_handle_reply(arp_frame_t *frame)
{
  // Put response into arp table
}

arp_frame_t
arp_build_frame( uint16_t opcode, mac_address_t dest_hw, uint32_t dest_ip)
{
  arp_frame_t frame;
  frame.hardware_type = htons(HTYPE_ETHERNET);
  frame.protocol_type = htons(TYPE_IPv4);
  frame.hardware_addr_len = ETH_MAC_ADDRESS_LENGTH;
  frame.protocol_addr_len = IPV4_ADDR_LEN;
  frame.opcode = htons(opcode);

  frame.src_hardware_addr = hton_mac(my_mac());
  frame.src_ip_address = htonl(OWN_IPV4_ADDR);
  frame.dest_hardware_addr = hton_mac(dest_hw)
  frame.dest_ip_address = htonl(dest_ip);
  return frame;
}
