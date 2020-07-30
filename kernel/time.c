
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

#include "time.h"
#include "kernel/memory.h"
#include <sys/types.h>
#include <stdio.h>

static uint64_t ticks_since_start = 0;

void
time_start(unsigned int period)
{
#if BOARD_VERSATILEPB
  *TIMER3_CTRL &= ~(1 << 7);   // disable timer
  *TIMER3_CTRL |= 1 << 6;      // set periodic-mode
  *TIMER3_INTCLR = (char)0x1;  // clear interrupts
  *TIMER3_CTRL |= 1 << 5;      // set IntEnable
  *TIMER3_CTRL |= 1 << 1;      // set 32-bit mode
  *TIMER3_CTRL &= ~(1 << 0);   // set Wrapping-Mode
  *TIMER3_LOAD  = period;      // set timer period
  *TIMER3_CTRL |= 1 << 7;      // start timer
#endif
}

void
time_stop(void)
{
#if BOARD_VERSATILEPB
  *TIMER3_CTRL &= ~(1 << 7);        // disable timer
  *TIMER3_INTCLR = (char)0x1;       // clear interrupts
#endif
}

void
irq_handler_time_inc()
{
  ticks_since_start++;
  *TIMER3_INTCLR = 1;
}

void
init_time()
{
  *PIC_INT_ENABLE |= TIMER3_INTBIT;  // unmask interrupt bit for timer3
  time_stop();
  time_start(TIMER_MILLIS_INTERVAL);
}

uint64_t
millis_since_start()
{
  return ticks_since_start;
}

uint64_t
seconds_since_start()
{
  return ticks_since_start / 1000;
}

uint64_t
minutes_since_start()
{
  return ticks_since_start / (1000 * 60);
}
