/*
 * Copyright (c) 2020 Ivan Jelincic <parazyd@dyne.org>
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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#include "ind.h"
#include "socket.h"
#include "tctl.h"

int tctl_check_error(char *err)
{
	if (err != NULL)
		/* TODO: Error handling */
		if (strncmp("250", err, 3))
			return 1;

	return 0;
}

void tctl_cmd(sock_t *sock, char *cmd, ...)
{
	va_list ap;
	char *req, *breq, *arg;

	req = smprintf("%s", cmd);

	va_start(ap, cmd);
	for (arg = va_arg(ap, char *); arg; arg = va_arg(ap, char *)) {
		breq = smprintf("%s %s", req, arg);
		free(req);
		req = breq;
	}
	va_end(ap);
	/* printf("%s\n", req); */
	sock_printf(sock, "%s\r\n", req);
	free(req);
}

void tctl_simplecmd(sock_t *sock, char *cmd)
{
	tctl_cmd(sock, cmd, NULL);
}

char *tctl_read_auth_cookie(char *path)
{
	int i, j;
	char cookie[65], *ret;
	unsigned char rbuf[32]; /* 32 is the current size of ControlAuthCookie */
	FILE* file = fopen(path, "rb");

	if (file == NULL) {
		return NULL;
		/* edie("tctl_read_auth_cookie"); */
	}

	fread(rbuf, 32, 1, file);
	fclose(file);

	for (i = 0, j = 0; i < 32; i++, j+=2)
		sprintf(cookie+j, "%02x", rbuf[i] & 0xff);

	ret = memdups(cookie);
	return ret;
}

/*
int main(void)
{
	sock_t *s;
	char *ck, *res;

	ck = tctl_read_auth_cookie("/var/lib/tor/auth_cookie");

	s = sock_new();
	if (sock_connect(s, "127.0.0.1", 9051))
		return 1;

	tctl_cmd(s, "authenticate", ck);
	free(ck);

	res = sock_gets(s);
	printf("got: %s", res);

	printf("%d\n", tctl_check_error(res));

	tctl_simplecmd(s, "getinfo network-liveness");
	res = sock_gets(s);
	printf("got: %s", res);
	printf("%d\n", tctl_check_error(res));

	if (sock_close(s) < 0) {
		free(s);
		return 1;
	}

	if (s != NULL)
		free(s);

	return 0;
}
*/
