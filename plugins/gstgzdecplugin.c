#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstgzdec.h"

#include <gst/gstplugin.h>

#include <string.h>

static gboolean
plugin_init(GstPlugin* p)
{
    if (!gst_element_register(p, "gzdec", GST_RANK_NONE, GST_TYPE_GZDEC))
        return FALSE;
    return TRUE;
}


GST_PLUGIN_DEFINE(GST_VERSION_MAJOR, GST_VERSION_MINOR,
#if GST_CHECK_VERSION(1,0,0)
    gzdec,
#else
    "gzdec",
#endif
    "Decompress streams",
    plugin_init, VERSION, "LGPL", PACKAGE_NAME, "http://gstreamer.net/")