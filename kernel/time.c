
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

#define INTTOCHAR(val) ((val) + '0')

static clock_t clock = 0;

/*
 * Starts the clock timer by setting the according bits.
 */
void
clock_start(unsigned int period)
{
#if BOARD_VERSATILEPB
  *TIMER3_CTRL &= ~(1 << 7);    // disable timer
  *TIMER3_CTRL |= 1 << 6;       // set periodic-mode
  *TIMER3_INTCLR = (char) 0x1;  // clear interrupts
  *TIMER3_CTRL |= 1 << 5;       // set IntEnable
  *TIMER3_CTRL |= 1 << 1;       // set 32-bit mode
  *TIMER3_CTRL &= ~(1 << 0);    // set Wrapping-Mode
  *TIMER3_LOAD = period;        // set timer period
  *TIMER3_CTRL |= 1 << 7;       // start timer
#endif
}

void
clock_stop(void)
{
#if BOARD_VERSATILEPB
  *TIMER3_CTRL &= ~(1 << 7);    // disable timer
  *TIMER3_INTCLR = (char) 0x1;  // clear interrupts
#endif
}

void
irq_handler_clock()
{
  clock++;
  *TIMER3_INTCLR = 1;
}

void
time_init()
{
  clock_stop();
  clock_start(TIMER_MILLIS_INTERVAL);
}

clock_t
clock_millis()
{
  return clock;
}

clock_t
clock_seconds()
{
  return clock / 1000;
}

clock_t
clock_minutes()
{
  return clock / (1000 * 60);
}

clock_t
clock_hours()
{
  return clock / (1000 * 60 * 60);
}

const char *
clock_formatted_msms()
{
  return clock_time_to_str(clock_millis());
}

const char *
clock_time_to_str(clock_t time)
{
  uint64_t mils = time, secs = mils / 1000, mins = mils / 60000;
  char *format = "mm:ss:lll";   // minutes:seconds:millis
  format[0] = INTTOCHAR((mins / 10) % 10);
  format[1] = INTTOCHAR(mins % 10);
  format[3] = INTTOCHAR((secs / 10) % 10);
  format[4] = INTTOCHAR(secs % 10);
  format[6] = INTTOCHAR((mils / 100) % 10);
  format[7] = INTTOCHAR((mils / 10) % 10);
  format[8] = INTTOCHAR(mils % 10);
  return format;
}
