
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
#include "kernel/network/ethernet.h"
#include "kernel/network/arp.h"

#include <stdio.h>

/*
 * Takes a raw_packet and converts it data to an ethernet frame.
 * Reads the ether type and passes the packet to the corresponding protocol.
 */
void
start_pdu_encapsulation(raw_packet_t * buf)
{
  /* *INDENT-OFF* */
  ethernet_frame_t *frame = (ethernet_frame_t *) &(buf->data);
  /* *INDENT-ON* */

  uint16_t ether_type = ntohs(frame->ether_type);

  switch (ether_type)
    {
    case TYPE_ARP:
      arp_receive(frame);
      break;
    case TYPE_IPv4:
    case TYPE_IPv6:            // we don't wanna support ipv6 yet ;)
    default:
      break;
    }
}
