
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
#include "kernel/network/ipv4.h"
#include "kernel/network/network_io.h"
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

      arp_frame_for_send_t arp_reply_frame = arp_build_frame(
                                      ARP_REPLY, 
                                      arp_get_src_hw_address(frame), // use original sender as destination
                                      arp_get_src_ip_addr(frame), 
                                      arp_get_dest_hw_address(frame), 
                                      OWN_IPV4_ADDR);
      
      send_ethernet(&arp_reply_frame, sizeof(arp_frame_for_send_t));
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
  uint64_t new_hw_address = __builtin_bswap64(hw_addr) >> 16;
  static uint8_t hardware_addr[6];
  for(int i = 0; i < 6; i++)
    hardware_addr[i] = (uint8_t) (new_hw_address >> (i*8));
  return hardware_addr;
}

arp_frame_for_send_t
arp_build_frame(uint16_t opcode, uint64_t dest_hw, uint32_t dest_ip, uint64_t src_hw, uint32_t src_ip)
{
  arp_frame_ipv4_t frame_ipv4 = { 0 };
  uint8_t *converted_src_hw = arp_convert_hw_addr(src_hw);
  uint8_t *converted_dest_hw = arp_convert_hw_addr(dest_hw);

  memcpy(frame_ipv4.src_hardware_addr, converted_src_hw, 6);
  memcpy(frame_ipv4.dest_hardware_addr, converted_dest_hw, 6);

  frame_ipv4.src_ip_address = src_ip;
  frame_ipv4.dest_ip_address = dest_ip;

  // for(int i = 0; i < 6; i++)
  //   {
  //     frame_ipv4.src_hardware_addr[i] = converted_src_hw[i];
  //     frame_ipv4.dest_hardware_addr[i] = converted_dest_hw[i];
  //   }

  arp_frame_t frame = {
    switch_endian16(HTYPE_ETHERNET),
    switch_endian16(TYPE_IPv4),
    ETH_MAC_ADDRESS_LENGTH,
    IPV4_ADDR_LEN,
    switch_endian16(opcode)
  };
  arp_frame_for_send_t frame_out = {frame, frame_ipv4};
  return frame_out;
}

uint64_t
arp_get_src_hw_address(arp_frame_t *arp_frame) 
{
  arp_frame_ipv4_t *frame = (arp_frame_ipv4_t*) arp_frame->data;
  uint64_t source_hw = *(uint64_t*) (frame->src_hardware_addr);
  return __builtin_bswap64(source_hw) >> 16;
}

uint64_t
arp_get_dest_hw_address(arp_frame_t *arp_frame)
{
  arp_frame_ipv4_t *frame = (arp_frame_ipv4_t*) arp_frame->data;
  uint64_t dest_hw = *(uint64_t*) frame->dest_hardware_addr;
  return __builtin_bswap64(dest_hw) >> 16;
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
arp_get_src_ip_addr(arp_frame_t *arp_frame)
{
  arp_frame_ipv4_t *frame = (arp_frame_ipv4_t*) arp_frame->data;
  return switch_endian32(frame->src_ip_address);
}

uint32_t
arp_get_dest_ip_addr(arp_frame_t *arp_frame)
{
  arp_frame_ipv4_t *frame = (arp_frame_ipv4_t*) arp_frame->data;
  return switch_endian32(frame->dest_ip_address);
}

uint32_t
arp_get_opcode(arp_frame_t *arp_frame)
{
  return switch_endian16(arp_frame->opcode);
}
