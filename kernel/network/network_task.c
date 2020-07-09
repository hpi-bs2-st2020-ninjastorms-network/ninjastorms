#include "network_task.h"
#include "kernel/network/e1000.h"

#include <sys/types.h>

static int packet_count = 0;
static int buffer_start = 0;
static int buffer_end   = 0;
static raw_packet_t ring_buffer[MAX_PACKET_COUNT] = { 0 };

void
network_task(void)
{
  // ARP request: Who has 10.0.2.15? Tell 10.0.2.10
  const char * data = "\xff\xff\xff\xff\xff\xff\x52\x54\x00\x12\x34\x56\x08\x06\x00\x01" \
                      "\x08\x00\x06\x04\x00\x01\x52\x54\x00\x12\x34\x56\x0a\x00\x02\x0a" \
                      "\x00\x00\x00\x00\x00\x00\x0a\x00\x02\x0f";
  for(;;) 
   {
      send_packet(data, 42);
      // Send packet too TCP /IP Stack
      // wait some time before next packet
      for (int j = 0; j < 100000000; ++j);
    }
}

void
add_packet(void* data, size_t len)
{
  /*
  if(queue_full){
    print error
    return
  }
  memcpy(data, some_place_to_copy, len);
  some_place_to_copy.length = len;
  packet_received++;
  */
}