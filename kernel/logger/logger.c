#include <stdio.h>
#include <stdarg.h>
#include "logger.h"
#include "kernel/time.h"

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
