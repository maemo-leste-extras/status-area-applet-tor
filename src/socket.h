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

#ifndef __TORSOCKET_H__
#define __TORSOCKET_H__

typedef struct sock_t sock_t;
struct sock_t {
	int fd;
	void *data[2];
};

sock_t *sock_new();

int sock_connect(sock_t *sock, char *server, unsigned short port);
int sock_close(sock_t *sock);

int sock_write(sock_t *sock, char *buf, int len);
int sock_writeall(sock_t *sock, char *buf, int len);
int sock_printf(sock_t *sock, char *fmt, ...);
int sock_read(sock_t *sock, char *buf, int len);
int sock_readall(sock_t *sock, char *buf, int len);

char *sock_gets(sock_t *sock);

#endif
