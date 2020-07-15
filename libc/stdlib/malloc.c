
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

#include <errno.h>
#include <stddef.h>
#include <sys/types.h>
#include <unistd.h>

// see https://en.wikipedia.org/wiki/Data_structure_alignment#Computing_padding
#define ALIGN(val, alignment) (((val) + (alignment - 1)) & -(alignment))
#define ALIGN_16(val) (ALIGN((val), 16))

struct block {
  struct block *next;
  size_t size;
};
typedef struct block block_t;

static block_t free_block_list_head = { NULL, 0 };
// make sure overhead is aligned 16-byte to allow malloc easy 16-byte alignment
static const size_t overhead = ALIGN_16(sizeof(block_t));

/*
 * A very basic malloc that uses a sbrk with global memory. Use with care!
 * It always returns 16-byte aligned memory addresses.
 * It is also NOT thread safe!
 *
 * Space layout is as follows:
 *
 * Offset              Memory
 *  0x00   +--------------------------------+
 *         | 16-byte aligned header block_t |
 *  0x10   +--------------------------------+
 *         |   previously allocated space   |
 *         |         (e.g. 4 byte)          |
 *  0x14   +--------------------------------+
 *            unused space to align header
 *  0x20   +--------------------------------+
 *         |  16-byte aligned next header   |
 *  0x30   +--------------------------------+
 *         |       .................        |
 *         |     requested program space    |
 *         |       .................        |
 *         +--------------------------------+
 * In this example the return value of malloc is a pointer to 0x30 so header
 * at 0x20 won't be overwritten by the requester.
 * free can then calculate back from 0x30 to 0x20 to get original header.
 */
void *
malloc(size_t size)
{
  size = ALIGN_16(size+overhead);
  block_t *block = (block_t *)free_block_list_head.next;
  block_t **head = &(free_block_list_head.next);
  // Let's see if we can find a free block that is sufficiently large
  while(block != NULL)
    {
      if(block->size >= size)
        {
          // remove that block from the list
          *head = block->next;
          // return the free block but make sure program does not overwrite our header
          return ((uint8_t*)block) + overhead;
        }
      head = &(block->next);
      block = block->next;
    }

  // No free block found so allocate new space
  block = (block_t*) sbrk(size);
  if(block == NULL)
    {
      errno = ENOMEM;
      return NULL;
    }
  block->size = size;

  return ((uint8_t*)block) + overhead;
}

void
free(void *ptr)
{
  // Reset pointer to real start so we have its header
  block_t* block = (block_t*)(((uint8_t*)ptr) - overhead);
  // Add block to beginning of our free_block_list
  block->next = free_block_list_head.next;
  free_block_list_head.next = block;
}
