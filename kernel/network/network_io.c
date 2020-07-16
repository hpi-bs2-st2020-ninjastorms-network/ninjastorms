
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

uint64_t
switch_endian64(uint64_t val)
{
  return __builtin_bswap64(val);
}
