/* Support files for GNU libc.  Files in the system namespace go here.
   Files in the C namespace (ie those that do not start with an
   underscore) go in .c.  */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>
#include <errno.h>
#include <unistd.h>
#include <sys/wait.h>

int _write(int file, char* ptr, int len)
{
  return -1;
}

int _close(int file)
{
  return -1;
}

int _lseek(int file, int ptr, int dir)
{
  return 0;
}

int _read(int file, char* ptr, int len)
{
  return -1;
}

void _exit (int status)
{
	while (1) {}		/* Make sure we hang here */
}

