
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
#include "kernel/network/routing.h"

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/*
 * Builds an arp frame with operation opcode, destination mac dest_hw and destination ip dest_ip.
 */
arp_frame_t
arp_build_frame(uint16_t opcode, mac_address_t dest_hw, uint32_t dest_ip)
{
  arp_frame_t frame = {
    .hardware_type = htons(HTYPE_ETHERNET),
    .protocol_type = htons(TYPE_IPv4),
    .hardware_addr_len = ETH_MAC_ADDRESS_LENGTH,
    .protocol_addr_len = IPV4_ADDR_LEN,
    .opcode = htons(opcode),

    .src_hardware_addr = hton_mac(e1000_get_mac()),
    .src_ip_address = htonl(OWN_IPV4_ADDR),
    .dest_hardware_addr = hton_mac(dest_hw),
    .dest_ip_address = htonl(dest_ip)
  };

  return frame;
}

/*
 * Takes an arp frame, builds a reply and hands the reply
 * to the ethernet layer.
 */
void
arp_handle_request(arp_frame_t * frame)
{
#ifdef ARP_DEBUG
  LOG_DEBUG("I have been requested... let's answer.");
#endif
  arp_frame_t arp_frame = arp_build_frame(ARP_REPLY,
                                          ntoh_mac(frame->src_hardware_addr),   // use original sender as destination
                                          ntohl(frame->src_ip_address));

  ethernet_send(ntoh_mac(frame->src_hardware_addr), TYPE_ARP, &arp_frame,
                sizeof(arp_frame_t));
}

/*
 * Unpacks the ethernet_frame and handles the packet according to its opcode.
 * Currently only ETHERNET with IPv4 is supported.
 * Algorithm according to ARP definition in RFC826 https://tools.ietf.org/html/rfc826
 */
void
arp_receive(ethernet_frame_t * frame)
{

  arp_frame_t *arp_frame = (arp_frame_t *) frame->payload;

  if (ntohs(arp_frame->hardware_type) != HTYPE_ETHERNET
      || arp_frame->hardware_addr_len != ETH_MAC_ADDRESS_LENGTH)
    {
#ifdef ARP_DEBUG
      LOG_WARN("Hardware type is not supported! (no ethernet)"
               "hwtype: %x addr_len: %x",
               arp_frame->hardware_type, arp_frame->hardware_addr_len);
#endif
      return;
    }
  if (ntohs(arp_frame->protocol_type) != TYPE_IPv4
      || arp_frame->protocol_addr_len != IPV4_ADDR_LEN)
    {
#ifdef ARP_DEBUG
      LOG_WARN("Protocol type is not supported! (no ipv4)");
#endif
      return;
    }
#ifdef ARP_DEBUG
  LOG_DEBUG("Found ARP packet:");
  LOG_DEBUG("Source HW: %s",
            mac_to_str(ntoh_mac(arp_frame->src_hardware_addr)));
  LOG_DEBUG("Source IP: %x", ntohl(arp_frame->src_ip_address));
  LOG_DEBUG("Destination HW: %s",
            mac_to_str(ntoh_mac(arp_frame->dest_hardware_addr)));
  LOG_DEBUG("Destination IP: %x", ntohl(arp_frame->dest_ip_address));
#endif

  bool merge_flag = false;
  if (arp_table_update(ntoh_mac(arp_frame->src_hardware_addr),
                       ntohl(arp_frame->src_ip_address)))
    merge_flag = true;

  if (ntohl(arp_frame->dest_ip_address) == OWN_IPV4_ADDR)
    {
      if (!merge_flag)
        arp_table_add_entry(ntoh_mac(arp_frame->src_hardware_addr),
                            ntohl(arp_frame->src_ip_address));

      if (ntohs(arp_frame->opcode) == ARP_REQUEST)
        arp_handle_request(arp_frame);
    }
}

/*
 * Takes an ip address, builds an arp request and hands it on to the ethernet layer.
 */
void
arp_send_request(uint32_t ip)
{
  arp_frame_t arp_frame = arp_build_frame(ARP_REQUEST,
                                          NULL_MAC,
                                          ip);
#ifdef ARP_DEBUG
  LOG_DEBUG("Sending arp request for ip %x", ip);
#endif
  ethernet_send(BROADCAST_MAC, TYPE_ARP, &arp_frame, sizeof(arp_frame_t));
}
