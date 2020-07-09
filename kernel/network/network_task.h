#pragma once

#include <sys/types.h>

#define MAX_PACKET_COUNT 128

struct raw_packet {
  uint32_t addr;
  uint16_t length;
};
typedef struct raw_packet raw_packet_t;

void network_task (void);
