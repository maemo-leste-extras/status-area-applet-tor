/*
 * Copyright (c) 2020 Ivan Jelincic <parazyd@dyne.org>
 * Copyricht (c) 2019 Christoph Lohmann <20h@r-36.net>
 *
 * This file is part of status-area-applet-tor
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ind.h"

void edie(char *fmt, ...)
{
	va_list fmtargs;

	va_start(fmtargs, fmt);
	vfprintf(stderr, fmt, fmtargs);
	va_end(fmtargs);
	fprintf(stderr, ": ");

	perror(NULL);

	exit(1);
}

void *reallocz(void *p, int l, int z)
{
	p = realloc(p, l);
	if (p == NULL)
		edie("realloc");
	if (z)
		memset(p, 0, l);

	return p;
}

void *mallocz(int l, int z)
{
	return reallocz(NULL, l, z);
}

void *memdups(void *p)
{
	char *ret;
	int l;

	l = strlen(p) + 1;
	ret = reallocz(NULL, l, 2);
	memmove(ret, p, l);

	return (void *)ret;
}

char *vsmprintf(char *fmt, va_list fmtargs, int size)
{
	char *ret;

	ret = reallocz(NULL, ++size, 2);
	vsnprintf(ret, size, fmt, fmtargs);

	return ret;
}

char *smprintf(char *fmt, ...)
{
	va_list fmtargs;
	char *ret;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	if (len == INT_MAX)
		return NULL;

	va_start(fmtargs, fmt);
	ret = vsmprintf(fmt, fmtargs, len);
	va_end(fmtargs);

	return ret;
}
