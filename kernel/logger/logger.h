
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

#include <sys/types.h>

#define LOG_DEBUG(...) general_log(0, __FILE__, __VA_ARGS__);
#define LOG_INFO(...) general_log(1, __FILE__, __VA_ARGS__);
#define LOG_WARN(...) general_log(2, __FILE__, __VA_ARGS__);
#define LOG_ERROR(...) general_log(3, __FILE__, __VA_ARGS__);
#define LOG_FATAL(...) general_log(4, __FILE__, __VA_ARGS__);

void general_log(uint32_t severity, const char *file, const char *format,
                 ...);
