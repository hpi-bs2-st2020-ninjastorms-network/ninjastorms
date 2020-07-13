
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

#include "pdu_handler.h"
#include "kernel/network/network_io.h"
#include "kernel/network/arp.h"
#include "kernel/logger/logger.h"

#include <stdio.h>

void
start_pdu_encapsulation(raw_packet_t *buf) 
{
  ethernet_frame_t* frame = (ethernet_frame_t*) &(buf->data);
  
  uint16_t ether_type = switch_endian16(frame->ether_type);
  
  switch (ether_type)
    {
      case ARP:
        handle_arp(frame);
        break;
      case IPv4:
      case IPv6: // we don't wanna support ipv6 yet ;)
      default:
        break;
    }
}
