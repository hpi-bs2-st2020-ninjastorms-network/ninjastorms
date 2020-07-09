#include "network_io.h"

uint16_t
switch_endian16(uint16_t val)
{
  return (val>>8) | (val<<8);
}

uint32_t
switch_endian32(uint32_t val)
{
  return 
    ((val>>24)&0xff)      |
    ((val<<8)&0xff0000)   |
    ((val>>8)&0xff00)     |
    ((val<<24)&0xff000000);
}