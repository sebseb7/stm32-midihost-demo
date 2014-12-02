// from newlib_stubs.c

#if STM32F == 2
#include "stm32f2xx.h"
#endif
#if STM32F == 4
#include "stm32f4xx.h"
#endif
#include <sys/types.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/unistd.h>

caddr_t _sbrk(int incr) {

	extern char _ebss; // Defined by the linker
	static char *heap_end;
	char *prev_heap_end;

	if (heap_end == 0) {
		heap_end = &_ebss;
	}
	prev_heap_end = heap_end;

	//todo: check consumtion
	//char * stack = (char*) __get_MSP();
	if (heap_end + incr >  (char*)0x20020000)
	{
		//_write (STDERR_FILENO, "Heap and stack collision\n", 25);
		errno = ENOMEM;
		return  (caddr_t) -1;
		//abort ();
	}

	heap_end += incr;
	return (caddr_t) prev_heap_end;

}

int _write(__attribute__((unused)) int file, char *ptr, int len)
{
	int i=0;
	for(i=0 ; i<len ; i++)
		ITM_SendChar((*ptr++));
	return len;
}

int _fstat(__attribute__((unused)) int file, struct stat *st)
{
	st->st_mode = S_IFCHR;
	return 0;
}

int _isatty(int file)
{
	switch (file)
	{
		case STDOUT_FILENO:
		case STDERR_FILENO:
		case STDIN_FILENO:
			return 1;
		default:
			errno = EBADF;
			return 0;
	}
}

int _close(__attribute__((unused)) int file)
{
	return -1;
}

int _lseek(__attribute__((unused)) int file, __attribute__((unused)) int ptr, __attribute__((unused)) int dir)
{
	return 0;
}

int _read(__attribute__((unused)) int file, __attribute__((unused)) char *ptr, __attribute__((unused)) int len)
{
	errno = EBADF;
	return -1;
}
