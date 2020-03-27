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

#include <arpa/inet.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ind.h"
#include "socket.h"

sock_t *sock_new()
{
	sock_t *ret;
	ret = mallocz(sizeof(sock_t), 2);
	return ret;
}

int sock_connect(sock_t *sock, char *server, unsigned short port)
{
	struct sockaddr_in remote;

	remote.sin_family = AF_INET;
	remote.sin_port = htons(port);
	remote.sin_addr.s_addr = inet_addr(server);

	sock->fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock->fd < 0)
		return 1;

	if (!connect(sock->fd, (struct sockaddr *)&remote, sizeof(struct sockaddr_in)))
		return 0;

	close(sock->fd);
	return 1;
}

int sock_close(sock_t *sock)
{
	return close(sock->fd);
}

int sock_write(sock_t *sock, char *buf, int len)
{
	return send(sock->fd, buf, len, MSG_NOSIGNAL);
}

int sock_writeall(sock_t *sock, char *buf, int len)
{
	int olen, nlen;

	for (olen = 0;
			(nlen = sock_write(sock, &buf[olen], len-olen))
			&& olen < len;
			olen += nlen);

	return 0;
}

int sock_printf(sock_t *sock, char *fmt, ...)
{
	va_list fmtargs;
	char *buf;
	int len;

	va_start(fmtargs, fmt);
	len = vsnprintf(NULL, 0, fmt, fmtargs);
	va_end(fmtargs);

	va_start(fmtargs, fmt);
	buf = vsmprintf(fmt, fmtargs, len);
	va_end(fmtargs);

	len = sock_writeall(sock, buf, strlen(buf));
	free(buf);
	return len;
}

int sock_read(sock_t *sock, char *buf, int len)
{
	return recv(sock->fd, buf, len, 0);
}

int sock_readall(sock_t *sock, char *buf, int len)
{
	int olen, nlen;

	for (olen = 0;
			(nlen = sock_read(sock, &buf[olen], len-olen))
			&& olen < len;
			olen += nlen);

	return 0;
}

char *sock_gets(sock_t *sock)
{
	char *ret;
	int len;

	len = 1;
	ret = mallocz(len, 2);
	while (sock_read(sock, &ret[len-1], 1) > 0 && ret[len-1] != '\n'
			&& len < MAXLINESIZE) {
		ret = reallocz(ret, ++len + 1, 0);
	}
	/*
	if (ret[len-1] != '\n') {
		free(ret);
		return NULL;
	}
	ret[len-1] = '\0';
	*/
	ret[len] = '\0';
	return ret;
}
