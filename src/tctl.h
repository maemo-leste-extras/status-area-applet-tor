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

#ifndef __TCTL_H__
#define __TCTL_H__

#include "socket.h"

int tctl_check_error(char *err);

void tctl_cmd(sock_t *sock, char *cmd, ...);
void tctl_simplecmd(sock_t *sock, char *cmd);

char *tctl_read_auth_cookie(char *path);

#endif
