
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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stddef.h>
#include <sys/types.h>

#include "kernel/memory.h"

// see https://en.wikipedia.org/wiki/Data_structure_alignment#Computing_padding
#define ALIGN_ADDR(addr, alignment) (((addr) + (alignment - 1)) & -(alignment))
#define ALIGN_ADDR_16(addr) (ALIGN_ADDR(addr, 16))

uint32_t current_heap_address = HEAP_START;

/*
This is a very basic malloc that does not allow any free. Use with care!
It always returns 16-byte aligned addresses.
*/
void *
malloc(size_t size)
{
  if(current_heap_address + size > HEAP_START + HEAP_SIZE)
    return NULL;

  void *ptr = (void*) ALIGN_ADDR_16(current_heap_address);
  current_heap_address = ((uint32_t) ptr) + size;

  return ptr;
}
