
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

  if(arp_get_hw_type(arp_frame) != HTYPE_ETHERNET || arp_frame->hardware_addr_len != ETH_MAC_ADDRESS_LENGTH)
    {
      log_warn("Hardware type is not supported! (no ethernet) hwtype: %x addr_len: %x", arp_frame->hardware_type, arp_frame->hardware_addr_len)
      return;
    }
  if(arp_get_protocol_type(arp_frame) != TYPE_IPv4 || arp_frame->protocol_addr_len != IPV4_ADDR_LEN)
    {
      log_warn("Protocol type is not supported! (no ipv4)");
      return;
    }

  log_debug("Found ARP packet.");
  log_debug("Destination HW: %y", arp_get_dest_hw_address(arp_frame));
  log_debug("Source HW: %y", arp_get_src_hw_address(arp_frame));
  
  switch(arp_get_opcode(arp_frame))
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
  if(arp_get_dest_ip_addr(frame) == OWN_IPV4_ADDR)
    {
      // ITS ME
      log_debug("I have been requested... let's answer.");

      uint64_t src_hw_address = arp_get_src_hw_address(frame);

      arp_frame_t *arp_frame = (arp_frame_t*) malloc(sizeof(arp_frame_t));
      arp_build_frame(
        arp_frame,
        ARP_REPLY, 
        src_hw_address, // use original sender as destination
        arp_get_src_ip_addr(frame)
      );

      send_ethernet(src_hw_address, TYPE_ARP, arp_frame, sizeof(arp_frame_t));
      free(arp_frame);
    }
}

void
arp_handle_reply(arp_frame_t *frame)
{
  // Put response into arp table
}

uint8_t *
arp_convert_hw_addr(uint64_t hw_addr)
{
  uint64_t new_hw_address = switch_endian64(hw_addr) >> 16;
  static uint8_t hardware_addr[6];
  for(int i = 0; i < 6; i++)
    hardware_addr[i] = (uint8_t) (new_hw_address >> (i*8));
  return hardware_addr;
}

void
arp_build_frame(arp_frame_t *frame, uint16_t opcode, uint64_t dest_hw, uint32_t dest_ip)
{
  frame->hardware_type = switch_endian16(HTYPE_ETHERNET);
  frame->protocol_type = switch_endian16(TYPE_IPv4);
  frame->hardware_addr_len = ETH_MAC_ADDRESS_LENGTH;
  frame->protocol_addr_len = IPV4_ADDR_LEN;
  frame->opcode = switch_endian16(opcode);

  copy_my_mac(frame->src_hardware_addr);
  frame->src_ip_address = switch_endian32(OWN_IPV4_ADDR);
  uint8_t *converted_dest_hw_address = arp_convert_hw_addr(dest_hw);
  memcpy(frame->dest_hardware_addr, converted_dest_hw_address, 6);
  frame->dest_ip_address = switch_endian32(dest_ip);
}

uint64_t
arp_get_src_hw_address(arp_frame_t *frame)
{
  return switch_endian64(*(uint64_t *) frame->src_hardware_addr) >> 16;
}

uint64_t
arp_get_dest_hw_address(arp_frame_t *frame)
{
  return switch_endian64(*(uint64_t *) frame->dest_hardware_addr) >> 16;
}

uint16_t
arp_get_hw_type(arp_frame_t *frame)
{
  return switch_endian16(frame->hardware_type);
}

uint16_t
arp_get_protocol_type(arp_frame_t *frame)
{
  return switch_endian16(frame->protocol_type);
}

uint32_t
arp_get_src_ip_addr(arp_frame_t *frame)
{
  return switch_endian32(frame->src_ip_address);
}

uint32_t
arp_get_dest_ip_addr(arp_frame_t *frame)
{
  return switch_endian32(frame->dest_ip_address);
}

uint32_t
arp_get_opcode(arp_frame_t *frame)
{
  return switch_endian16(frame->opcode);
}
