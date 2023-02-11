plugin_LTLIBRARIES = libgstgzdec.la

libgstgzdec_la_SOURCES = gstgzdecplugin.c gstgzdec.c

libgstgzdec_la_CFLAGS = $(GST_BASE_CFLAGS) $(GST_CFLAGS)
libgstgzdec_la_LIBADD = $(GST_LIBS) $(GST_BASE_LIBS) $(ZLIB_LIBS)
libgstgzdec_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstgzdec_la_LIBTOOLFLAGS = $(GST_PLUGIN_LIBTOOLFLAGS)

noinst_HEADERS = gstgzdec.h