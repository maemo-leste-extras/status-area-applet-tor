/*
 * Copyright (c) 2020 Ivan Jelincic <parazyd@dyne.org>
 * Copyright (c) 2019 Christoph Lohmann <20h@r-36.net>
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

#ifndef __IND_H__
#define __IND_H__

#define nelem(x) (sizeof(x) / sizeof((x)[0]))

#define MAXLINESIZE 1048576

void edie(char *fmt, ...);
void *reallocz(void *p, int l, int z);
void *mallocz(int l, int z);
void *memdups(void *p);
char *vsmprintf(char *fmt, va_list fmtargs, int size);
char *smprintf(char *fmt, ...);

#endif
