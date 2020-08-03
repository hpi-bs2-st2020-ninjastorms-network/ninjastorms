
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

#pragma once

// value calculated according to https://documentation-service.arm.com/static/5e8e2102fd977155116a4aef?token=
// assuming 1MHz clock frequency
#define TIMER_SECOND_INTERVAL 0xf4240
#define TIMER_MILLIS_INTERVAL 0x3e8

typedef unsigned long long clock_t;

void irq_handler_clock();
void time_init();

// clock is time since system start
clock_t clock_millis();
clock_t clock_seconds();
clock_t clock_minutes();
clock_t clock_hours();
const char * clock_formatted_msms();
const char * clock_time_to_str(clock_t time);
