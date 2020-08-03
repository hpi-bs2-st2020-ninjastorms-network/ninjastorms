
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

#include "logger.h"
#include "kernel/time.h"

#include <stdio.h>
#include <stdarg.h>

static const char* log_severities[] =
{
  "DEBUG",
  "INFO",
  "WARN",
  "ERROR",
  "FATAL"
};

static const char* log_colors[] =
{
  "\x1b[37m", // white
  "\x1b[34m", // blue
  "\x1b[33m", // yellow
  "\x1b[91m", // light red
  "\x1b[31m", // red
};

void 
general_log(uint32_t severity, const char* file, const char* format, ...)
{
  va_list args;
  const char *time = clock_formatted_msms();
  // Align shorter severities on the same level
  if(severity == 1 || severity == 2) 
    printf("%s[%s]  [%s]\x1b[0m [%s]: ", log_colors[severity], log_severities[severity], time, file);
  else 
    printf("%s[%s] [%s]\x1b[0m [%s]: ", log_colors[severity], log_severities[severity], time, file);
    
  va_start(args, format);
  vprintf(format, args);
  va_end(args);

  putchar('\n');
}
