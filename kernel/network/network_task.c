
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

#include "network_task.h"
#include "kernel/network/e1000.h"
#include "kernel/logger/logger.h"
#include "kernel/network/pdu_handler.h"

#include <sys/types.h>
#include <string.h>

static int recv_queue_start  = 0;
static int recv_queue_end    = 0;
static raw_packet_t recv_packet_queue[MAX_PACKET_COUNT] = { 0 };

static int send_queue_start  = 0;
static int send_queue_end    = 0;
static raw_packet_t send_packet_queue[MAX_PACKET_COUNT] = { 0 };

void
network_task_recv (void)
{
  while(1)
    {
      if(new_packet_available())
        {
          raw_packet_t *packet = remove_packet();
          start_pdu_encapsulation(packet);
        }
    }
}

void
insert_packet (uint8_t *data, size_t len)
{
  int new_end = (recv_queue_end + 1) % MAX_PACKET_COUNT;
  if (new_end != recv_queue_start)
    {
      recv_packet_queue[recv_queue_end].length = len;
      memcpy(&recv_packet_queue[recv_queue_end].data, data, len);
      recv_queue_end = new_end;
    }
  else
    {
      log_warn("Packet Queue full!");
    }
}

raw_packet_t*
remove_packet (void)
{
  if (recv_queue_start == recv_queue_end)
    return 0;

  raw_packet_t* packet = &recv_packet_queue[recv_queue_start];
  recv_queue_start = (recv_queue_start + 1) % MAX_PACKET_COUNT;
  return packet;
}

uint8_t
new_packet_available (void) 
{
  return recv_queue_start != recv_queue_end;
}








