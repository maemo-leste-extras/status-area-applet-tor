/*
 * Copyright (c) 2020 Ivan Jelincic <parazyd@dyne.org>
 * Copyright (c) 2015 Ivaylo Dimitrov <ivo.g.dimitrov.75@gmail.com>
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <hildon/hildon.h>
#include <libhildondesktop/libhildondesktop.h>

#include "tctl.h"
#include "socket.h"

#define STATUS_AREA_APPLET_TOR_TYPE (status_area_applet_tor_get_type())
#define STATUS_AREA_APPLET_TOR(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
			STATUS_AREA_APPLET_TOR_TYPE, StatusAreaAppletTor))

typedef struct _StatusAreaAppletTor StatusAreaAppletTor;
typedef struct _StatusAreaAppletTorClass StatusAreaAppletTorClass;
typedef struct _StatusAreaAppletTorPrivate StatusAreaAppletTorPrivate;

struct _StatusAreaAppletTor
{
	HDStatusMenuItem parent;
	StatusAreaAppletTorPrivate *priv;
};

struct _StatusAreaAppletTorClass
{
	HDStatusMenuItemClass parent;
};

struct _StatusAreaAppletTorPrivate
{
	GtkWidget *conn_icon;
	GtkWidget *menu_button;
	GtkWidget *proxy_chkbutton;
	GtkWidget *settings_dialog;
	gboolean proxying_state;
	gint connection_state;
	gchar *auth_cookie;
	sock_t *sock;
};

HD_DEFINE_PLUGIN_MODULE_EXTENDED(StatusAreaAppletTor, status_area_applet_tor,
		HD_TYPE_STATUS_MENU_ITEM, G_ADD_PRIVATE_DYNAMIC(StatusAreaAppletTor),,);

static void status_area_applet_tor_set_status_icon(gpointer data, GdkPixbuf *pixbuf)
{
	StatusAreaAppletTor *item = STATUS_AREA_APPLET_TOR(data);
	g_return_if_fail(item != NULL && pixbuf != NULL);
	hd_status_plugin_item_set_status_area_icon(HD_STATUS_PLUGIN_ITEM(item), pixbuf);
	pixbuf = NULL;
}

static void status_area_applet_tor_set_menu_icon(StatusAreaAppletTor *self, GdkPixbuf *pixbuf)
{
	StatusAreaAppletTorPrivate *priv = status_area_applet_tor_get_instance_private(self);
	priv->conn_icon = gtk_image_new_from_pixbuf(pixbuf);
	hildon_button_set_image(HILDON_BUTTON(priv->menu_button), priv->conn_icon);
	pixbuf = NULL;
}

static void status_area_applet_tor_set_icons(StatusAreaAppletTor *self)
{
	StatusAreaAppletTorPrivate *priv = status_area_applet_tor_get_instance_private(self);
	GdkPixbuf *pixbuf = NULL;
	GtkIconTheme *theme = gtk_icon_theme_get_default();

	if (priv->connection_state)
		pixbuf = gtk_icon_theme_load_icon(theme, "statusarea_tor_enabled", 48, 0, NULL);
	else
		pixbuf = gtk_icon_theme_load_icon(theme, "statusarea_tor_disabled", 48, 0, NULL);
	status_area_applet_tor_set_menu_icon(self, pixbuf);

	if (priv->proxying_state) {
		pixbuf = gtk_icon_theme_load_icon(theme, "statusarea_tor_connected", 18, 0, NULL);
		status_area_applet_tor_set_status_icon(self, pixbuf);
	}

	g_object_unref(pixbuf);

	hildon_button_set_value(HILDON_BUTTON(priv->menu_button),
			priv->connection_state ? "Connected" : "Disconnected");
}

static void status_area_applet_tor_tctl_set_status(StatusAreaAppletTor *self)
{
	StatusAreaAppletTorPrivate *priv = status_area_applet_tor_get_instance_private(self);
	gchar *res;

	/* TODO: Refactor into multiple functions */

	if (sock_connect(priv->sock, "127.0.0.1", 9051)) {
		priv->connection_state = 0;
		return;
	}

	tctl_cmd(priv->sock, "authenticate", priv->auth_cookie);
	res = sock_gets(priv->sock);
	if (tctl_check_error(res))
		goto setstatusend;

	tctl_simplecmd(priv->sock, "getinfo network-liveness");
	res = sock_gets(priv->sock);
	if (tctl_check_error(res))
		goto setstatusend;

	if (!strncmp(res, "250-network-liveness=up", 23))
		priv->connection_state = 1;
	else
		priv->connection_state = 0;

setstatusend:
	sock_close(priv->sock);
	if (res != NULL)
		g_free(res);
	return;
}

static void status_area_applet_tor_proxy_chkbutton_toggled(GtkWidget *button,
		StatusAreaAppletTor *self)
{
	StatusAreaAppletTorPrivate *priv = status_area_applet_tor_get_instance_private(self);

	priv->proxying_state = !priv->proxying_state;

	if (priv->proxying_state)
		system("sudo /usr/sbin/tor_transparent_proxy.sh enable");
	else
		system("sudo /usr/sbin/tor_transparent_proxy.sh disable");
}

static void status_area_applet_tor_clicked(GtkWidget *button, StatusAreaAppletTor *self)
{
	StatusAreaAppletTorPrivate *priv = status_area_applet_tor_get_instance_private(self);

	GtkWidget *toplevel = gtk_widget_get_toplevel(button);
	gtk_widget_hide(toplevel);

	priv->settings_dialog = gtk_dialog_new();
	gtk_window_set_title(GTK_WINDOW(priv->settings_dialog), "Tor Status");
	gtk_window_set_transient_for(GTK_WINDOW(priv->settings_dialog), GTK_WINDOW(toplevel));
	gtk_window_set_destroy_with_parent(GTK_WINDOW(priv->settings_dialog), TRUE);

	priv->proxy_chkbutton = hildon_check_button_new(
			HILDON_SIZE_FINGER_HEIGHT|HILDON_SIZE_AUTO_WIDTH);
	gtk_button_set_label(GTK_BUTTON(priv->proxy_chkbutton), "Enable transparent system proxying");
	gtk_button_set_alignment(GTK_BUTTON(priv->proxy_chkbutton), 0.5f, 0.5f);
	hildon_check_button_set_active(HILDON_CHECK_BUTTON(priv->proxy_chkbutton),
			priv->proxying_state);
	gtk_box_pack_start(GTK_BOX(GTK_DIALOG(priv->settings_dialog)->vbox),
			priv->proxy_chkbutton, TRUE, TRUE, 0);

	g_signal_connect(priv->proxy_chkbutton, "toggled",
			G_CALLBACK(status_area_applet_tor_proxy_chkbutton_toggled), self);

	gtk_widget_show_all(priv->settings_dialog);
}

static void status_area_applet_tor_init(StatusAreaAppletTor *self)
{
	StatusAreaAppletTorPrivate *priv = status_area_applet_tor_get_instance_private(self);
	GdkPixbuf *pixbuf = NULL;
	GtkIconTheme *theme = gtk_icon_theme_get_default();

	self->priv = priv;
	priv->sock = sock_new();
	priv->connection_state = 0;
	priv->auth_cookie = tctl_read_auth_cookie("/var/run/tor/control.authcookie");
	priv->menu_button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
			HILDON_BUTTON_ARRANGEMENT_VERTICAL);

	status_area_applet_tor_tctl_set_status(self);
	status_area_applet_tor_set_icons(self);

	hildon_button_set_style(HILDON_BUTTON(priv->menu_button), HILDON_BUTTON_STYLE_PICKER);
	hildon_button_set_alignment(HILDON_BUTTON(priv->menu_button), 0.0, 0.0, 1.0, 1.0);
	hildon_button_set_title(HILDON_BUTTON(priv->menu_button), "Tor");

	g_signal_connect(priv->menu_button, "clicked",
			G_CALLBACK(status_area_applet_tor_clicked), self);

	gtk_container_add(GTK_CONTAINER(self), priv->menu_button);
	gtk_widget_show_all(GTK_WIDGET(self));
}

static void status_area_applet_tor_finalize(GObject *self)
{
	return;
}

static void status_area_applet_tor_class_init(StatusAreaAppletTorClass *class)
{
	GObjectClass *object_class = G_OBJECT_CLASS(class);
	object_class->finalize = status_area_applet_tor_finalize;
}

static void status_area_applet_tor_class_finalize(StatusAreaAppletTorClass *class)
{
	return;
}

StatusAreaAppletTor *status_area_applet_tor_new(void)
{
	return g_object_new(STATUS_AREA_APPLET_TOR_TYPE, NULL);
}
