
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
#include <config.h>
#endif

#include <errno.h>
#include <stddef.h>
#include <sys/types.h>

#include "kernel/memory.h"

static uint32_t current_heap_address = HEAP_START;

/*
 * Very simple implementation of sbrk that uses a global memory pool.
 * Should probably be changed to real program break in the future.
 */
void *
sbrk(size_t increment)
{
  if (current_heap_address + increment > HEAP_START + HEAP_SIZE)
    {
      // No memory left
      errno = ENOMEM;
      return NULL;
    }
  void *ptr = (void *) current_heap_address;
  current_heap_address += increment;
  return ptr;
}
