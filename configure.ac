AC_PREREQ[(2.69)]
AC_INIT([status-area-applet-tor], [0.1], [parazyd@dyne.org])
AC_CONFIG_SRCDIR([src/status-area-applet-tor.c])
AM_INIT_AUTOMAKE
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_LIBTOOL

PKG_CHECK_MODULES(SAAT, hildon-1 libhildondesktop-1 gtk+-2.0 glib-2.0 gobject-2.0 gconf-2.0 libosso)
AC_SUBST(SAAT_CFLAGS)
AC_SUBST(SAAT_LIBS)

icon48dir=`pkg-config libhildondesktop-1 --variable=prefix`/share/icons/hicolor/48x48/hildon
icon18dir=`pkg-config libhildondesktop-1 --variable=prefix`/share/icons/hicolor/18x18/hildon

hildonhomedesktopdir=`pkg-config libhildondesktop-1 --variable=hildonstatusmenudesktopentrydir`
hildondesktoplibdir=`pkg-config libhildondesktop-1 --variable=hildondesktoplibdir`

AC_SUBST(icon48dir)
AC_SUBST(icon18dir)
AC_SUBST(hildonhomedesktopdir)
AC_SUBST(hildondesktoplibdir)

AC_OUTPUT([Makefile src/Makefile data/Makefile])
