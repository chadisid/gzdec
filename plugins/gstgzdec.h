#ifndef _GST_GZDEC_H_
#define _GST_GZDEC_H_

#include <gst/gst.h>
#include <zlib.h>
G_BEGIN_DECLS

#define GST_TYPE_GZDEC          (gst_gzdec_get_type ())
#define GST_GZDEC(obj)          (G_TYPE_CHECK_INSTANCE_CAST ((obj), GST_TYPE_GZDEC, GstGzdec))
#define GST_GZDEC_CLASS(klass)  (G_TYPE_CHECK_CLASS_CAST ((klass), GST_TYPE_GZDEC, GstGzdecClass))
#define GST_IS_GZDEC(obj)       (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GST_TYPE_GZDEC))
#define GST_IS_GZDEC_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE ((klass), GST_TYPE_GZDEC))

typedef struct _GstGzdec GstGzdec;
typedef struct _GstGzdecClass GstGzdecClass;
struct _GstGzdec
{
    GstElement element;
    GstPad* sinkpad;
    GstPad* srcpad;
    GstBuffer* out_buf;
#if GST_CHECK_VERSION(1,0,0)
    GstMapInfo out_buf_map;
#endif
    z_stream zstrm;
    gboolean init;
    gboolean out_buf_boo;
    size_t out_buf_capacity;
};

struct _GstGzdecClass
{
    GstElementClass parent_class;
};

GType gst_gzdec_get_type(void);

G_END_DECLS
#endif