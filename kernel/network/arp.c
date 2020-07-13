
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
#include <stdio.h>

void
handle_arp(ethernet_frame_t *frame) 
{
  log_debug("Found ARP packet.");
  log_debug("Dest Mac: ");
  read_mac(frame->dest_mac);
  log_debug("Src mac: ");
  read_mac(frame->source_mac);
}

void
read_mac(uint8_t* mac_address)
{
  // TODO
  printf("%x", mac_address[0]);
  for(uint16_t i = 1; i < 6; i++)
    {
      printf(":%x", mac_address[i]);
    }
  printf("\n");
}