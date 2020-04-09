#include "Debug.h"

#ifdef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#endif

void debug_puts(char* string)
{
#ifdef WIN32
	puts(string);
#endif
}

void debug_printf(char* pszFormat, ...)
{
#ifdef WIN32
	va_list p;
	va_start(p, pszFormat);

	char t[1024];
	vsprintf_s(t, sizeof(t) / sizeof(t[0]) - 1, pszFormat, p);

	va_end(p);

	printf(t);
#endif
}

void debug_cls()
{
#ifdef WIN32
	system("cls");
#endif
}