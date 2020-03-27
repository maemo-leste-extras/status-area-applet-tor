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
	GtkWidget *button;
	int connection_state;
};

HD_DEFINE_PLUGIN_MODULE(StatusAreaAppletTor, status_area_applet_tor,
		HD_TYPE_STATUS_MENU_ITEM)

static void status_area_applet_tor_set_status_icon(gpointer data, GdkPixbuf *pixbuf)
{
	StatusAreaAppletTor *item = STATUS_AREA_APPLET_TOR(data);
	g_return_if_fail(item != NULL && pixbuf != NULL);
	hd_status_plugin_item_set_status_area_icon(HD_STATUS_PLUGIN_ITEM(item), pixbuf);
	pixbuf = NULL;
}

static void status_area_applet_tor_set_menu_icon(StatusAreaAppletTor *self, GdkPixbuf *pixbuf)
{
	StatusAreaAppletTorPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(self,
			STATUS_AREA_APPLET_TOR_TYPE, StatusAreaAppletTorPrivate);
	priv->conn_icon = gtk_image_new_from_pixbuf(pixbuf);
	hildon_button_set_image(HILDON_BUTTON(priv->button), priv->conn_icon);
	pixbuf = NULL;
}

static void status_area_applet_tor_init(StatusAreaAppletTor *self)
{
	StatusAreaAppletTorPrivate *priv = G_TYPE_INSTANCE_GET_PRIVATE(self,
			STATUS_AREA_APPLET_TOR_TYPE, StatusAreaAppletTorPrivate);
	GdkPixbuf *pixbuf = NULL;
	GtkIconTheme *theme = gtk_icon_theme_get_default();

	self->priv = priv;
	priv->connection_state = 0;

	priv->button = hildon_button_new(HILDON_SIZE_FINGER_HEIGHT,
			HILDON_BUTTON_ARRANGEMENT_VERTICAL);

	pixbuf = gtk_icon_theme_load_icon(theme, "statusarea_tor_disabled", 48, 0, NULL);
	status_area_applet_tor_set_menu_icon(self, pixbuf);
	pixbuf = gtk_icon_theme_load_icon(theme, "statusarea_tor_connecting", 18, 0, NULL);
	status_area_applet_tor_set_status_icon(self, pixbuf);
	g_object_unref(pixbuf);

	hildon_button_set_style(HILDON_BUTTON(priv->button), HILDON_BUTTON_STYLE_PICKER);
	hildon_button_set_alignment(HILDON_BUTTON(priv->button), 0.0, 0.0, 1.0, 1.0);
	hildon_button_set_title(HILDON_BUTTON(priv->button), "Tor");
	hildon_button_set_value(HILDON_BUTTON(priv->button),
			priv->connection_state ? "Connected" : "Disconnected");

	gtk_container_add(GTK_CONTAINER(self), priv->button);
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
	g_type_class_add_private(class, sizeof(StatusAreaAppletTorPrivate));
}

static void status_area_applet_tor_class_finalize(StatusAreaAppletTorClass *class)
{
	return;
}

StatusAreaAppletTor *status_area_applet_tor_new(void)
{
	return g_object_new(STATUS_AREA_APPLET_TOR_TYPE, NULL);
}
