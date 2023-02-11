#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "gstgzdec.h"

GST_DEBUG_CATEGORY_STATIC(gst_gzdec_debug);
#define GST_CAT_DEFAULT gst_gzdec_debug

static GstStaticPadTemplate src_template =
GST_STATIC_PAD_TEMPLATE("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS("application/unknown")
);

static GstStaticPadTemplate sink_template =
GST_STATIC_PAD_TEMPLATE("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS("application/x-gzip")
);

#if GST_CHECK_VERSION(1,0,0)
#define gst_gzdec_parent_class parent_class
G_DEFINE_TYPE(GstGzdec, gst_gzdec, GST_TYPE_ELEMENT);
#else
GST_BOILERPLATE(GstGzdec, gst_gzdec, GstElement, GST_TYPE_ELEMENT);
#endif

static void gst_gzdec_set_property(GObject* object,
    guint property_id, const GValue* value, GParamSpec* pspec);
static void gst_gzdec_get_property(GObject* object,
    guint property_id, GValue* value, GParamSpec* pspec);
static void gst_gzdec_state_changed(GstElement* element, GstState oldstate,
    GstState newstate, GstState pending);
#if GST_CHECK_VERSION(1,0,0)
static GstFlowReturn gst_gzdec_chain(GstPad* pad, GstObject* parent,
    GstBuffer* buffer);
#else
static GstFlowReturn gst_gzdec_chain(GstPad* pad, GstBuffer* in_buf);
#endif
enum
{
    PROP_0
};

static void
gst_gzdec_class_init(GstGzdecClass* klass)
{
    GObjectClass* gobject_class = G_OBJECT_CLASS(klass);
    GstElementClass* gstelement_class = GST_ELEMENT_CLASS(klass);

    GST_DEBUG_CATEGORY_INIT(gst_gzdec_debug, "gzdec", 0, "gzdec element");
#if GST_CHECK_VERSION(1,0,0)
    gst_element_class_set_static_metadata(gstelement_class,
        "gzip decoder", "Generic", "gzip decoder",
        "Aboobeker <aboosidhik@gmail.com");

    gst_element_class_add_static_pad_template(gstelement_class, &sink_template);
    gst_element_class_add_static_pad_template(gstelement_class, &src_template);
#endif
    gobject_class->set_property = gst_gzdec_set_property;
    gobject_class->get_property = gst_gzdec_get_property;
    gstelement_class->state_changed = gst_gzdec_state_changed;
}
#if !GST_CHECK_VERSION(1,0,0)
static void
gst_gzdec_base_init(gpointer g_class)
{
    GstElementClass* gstelement_class = GST_ELEMENT_CLASS(g_class);

    gst_element_class_add_static_pad_template(gstelement_class, &sink_template);
    gst_element_class_add_static_pad_template(gstelement_class, &src_template);
    gst_element_class_set_details_simple(gstelement_class, "gzip decoder",
        "Generic", "gzip decoder",
        "Aboobeker <aboosidhik@gmail.com>");
}
#endif

static void
#if GST_CHECK_VERSION(1,0,0)
        gst_gzdec_init(GstGzdec* gzdec)
#else
        gst_gzdec_init(GstGzdec* gzdec, GstGzdecClass* klass)
#endif
{
    /* sinkpad */
    gzdec->sinkpad = gst_pad_new_from_static_template(&sink_template, "sink");
    gst_pad_set_chain_function(gzdec->sinkpad,
        GST_DEBUG_FUNCPTR(gst_gzdec_chain));
    gst_element_add_pad(GST_ELEMENT(gzdec), gzdec->sinkpad);

    /* srcpad */
    gzdec->srcpad = gst_pad_new_from_static_template(&src_template, "src");
    gst_element_add_pad(GST_ELEMENT(gzdec), gzdec->srcpad);
    gst_pad_use_fixed_caps(gzdec->srcpad);

    gzdec->out_buf_boo = TRUE;
    GST_DEBUG_OBJECT(gzdec, "zlib init");
    gzdec->zstrm.next_in = Z_NULL;
    gzdec->zstrm.avail_in = 0;
    gzdec->zstrm.next_out = Z_NULL;
    gzdec->zstrm.avail_out = 0;
    gzdec->zstrm.zalloc = NULL;
    gzdec->zstrm.zfree = NULL;
    switch (inflateInit2(&gzdec->zstrm, MAX_WBITS + 16))
    {
        case Z_OK:
            GST_DEBUG_OBJECT(gzdec, "Zlib init is Z_OK");
            gzdec->init = TRUE;
            return;
        case Z_MEM_ERROR:
            GST_DEBUG_OBJECT(gzdec, "zlib init error: Z_MEM_ERROR");
            break;
        case Z_VERSION_ERROR:
            GST_DEBUG_OBJECT(gzdec, "zlib init error: Z_VERSION_ERROR");
            break;
        case Z_STREAM_ERROR:
            GST_DEBUG_OBJECT(gzdec, "zlib init error: Z_STREAM_ERROR");
            break;
        default:
            GST_DEBUG_OBJECT(gzdec, "zlib init error: unknown error");
            break;
    }
    gzdec->init = FALSE;
}

void
gst_gzdec_set_property(GObject* object, guint property_id,
    const GValue* value, GParamSpec* pspec)
{
    GstGzdec* gzdec = GST_GZDEC(object);

    GST_DEBUG_OBJECT(gzdec, "set_property");

    switch (property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

void
gst_gzdec_get_property(GObject* object, guint property_id,
    GValue* value, GParamSpec* pspec)
{
    GstGzdec* gzdec = GST_GZDEC(object);

    GST_DEBUG_OBJECT(gzdec, "get_property");

    switch (property_id) {
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, property_id, pspec);
        break;
    }
}

static void
gst_gzdec_state_changed(GstElement* element, GstState oldstate,
    GstState newstate, GstState pending)
{
    GstGzdec* gzdec = GST_GZDEC(element);

    if ((newstate == GST_STATE_NULL) && (gzdec->init)) {
        GST_DEBUG_OBJECT(gzdec, "zlib free");
        inflateEnd(&gzdec->zstrm);
    }
}

static gboolean
plugin_init(GstPlugin* plugin)
{
    return gst_element_register(plugin, "gzdec", GST_RANK_NONE, GST_TYPE_GZDEC);
}


static GstFlowReturn
#if GST_CHECK_VERSION(1,0,0)
gst_gzdec_chain(GstPad* pad, GstObject* parent, GstBuffer* in_buf)
#else
gst_gzdec_chain(GstPad* pad, GstBuffer* in_buf)
#endif
{
    GstGzdec* gzdec;
    #if GST_CHECK_VERSION(1,0,0)
    gzdec = GST_GZDEC(parent);
    GstMapInfo in_buf_map;
    #else
    gzdec = GST_GZDEC(gst_pad_get_parent(pad));
    #endif
    GstFlowReturn ret_flow = GST_FLOW_ERROR;
    GstEvent* eos;
    int ret;

    GST_DEBUG_OBJECT(gzdec, "input buffer");
#if GST_CHECK_VERSION(1,0,0)
    if (!gst_buffer_map(in_buf, &in_buf_map, GST_MAP_READ))
        goto free_in;
    gzdec->zstrm.next_in = in_buf_map.data;
    gzdec->zstrm.avail_in = in_buf_map.size;
#else
    gzdec->zstrm.next_in = in_buf->malloc_data;
    gzdec->zstrm.avail_in = in_buf->size;
#endif
    do {
        
        if (gzdec->out_buf_boo) {
            // Allocate out buffer 
            GST_DEBUG_OBJECT(gzdec, "output buffer");
#if GST_CHECK_VERSION(1,0,0)
            gzdec->out_buf_capacity = in_buf_map.size;
            gzdec->out_buf = gst_buffer_new_allocate(NULL, gzdec->out_buf_capacity, NULL);
            if (!gzdec->out_buf)
                goto unmap_in;
            if (!gst_buffer_map(gzdec->out_buf, &gzdec->out_buf_map, GST_MAP_WRITE)) {
                gst_buffer_unref(gzdec->out_buf);
                goto unmap_in;
            }
            
            gzdec->zstrm.next_out = gzdec->out_buf_map.data;
            gzdec->zstrm.avail_out = gzdec->out_buf_map.size;
#else
            gzdec->out_buf_capacity = in_buf->size;
            gzdec->out_buf = gst_buffer_new_and_alloc(gzdec->out_buf_capacity);
            if (!gzdec->out_buf)
                goto free_in;
            gzdec->zstrm.next_out = gzdec->out_buf->malloc_data;
            gzdec->zstrm.avail_out = gzdec->out_buf->size;
            
#endif
            gzdec->out_buf_boo = FALSE;
        }

        // Decode the file 
        GST_DEBUG_OBJECT(gzdec, "Decode step");
        int err;
        err = inflate(&gzdec->zstrm, Z_SYNC_FLUSH);
        switch (err)
        {
            case Z_OK:
                GST_DEBUG_OBJECT(gzdec, "decodes correctley Z_OK [%s]", gzdec->zstrm.msg);
                break;
            case Z_STREAM_END:
                GST_DEBUG_OBJECT(gzdec, "decoder ended Z_STREAM_END [%s]", gzdec->zstrm.msg);
                break;
            case Z_NEED_DICT:
                GST_DEBUG_OBJECT(gzdec, "decoder Z_NEED_DICT [%s]", gzdec->zstrm.msg);
                break;
            case Z_ERRNO:
                GST_DEBUG_OBJECT(gzdec, "decoder error: Z_ERRNO [%s]", gzdec->zstrm.msg);
                break;
            case Z_STREAM_ERROR:
                GST_DEBUG_OBJECT(gzdec, "decoder error: Z_STREAM_ERROR [%s]", gzdec->zstrm.msg);
                break;
            case Z_DATA_ERROR:
                GST_DEBUG_OBJECT(gzdec, "decoder error: Z_DATA_ERROR [%s]", gzdec->zstrm.msg);
                break;
            case Z_MEM_ERROR:
                GST_DEBUG_OBJECT(gzdec, "decoder error: Z_MEM_ERROR [%s]", gzdec->zstrm.msg);
                break;
            case Z_BUF_ERROR:
                GST_DEBUG_OBJECT(gzdec, "decoder error: Z_BUF_ERROR [%s]", gzdec->zstrm.msg);
                break;
            case Z_VERSION_ERROR:
                GST_DEBUG_OBJECT(gzdec, "decoder error: Z_VERSION_ERROR [%s]", gzdec->zstrm.msg);
            default:
                GST_DEBUG_OBJECT(gzdec, "decoder error: unknow code (%d) [%s]", err, gzdec->zstrm.msg);
                break;
        }
        if (err == Z_STREAM_ERROR) {
            GST_DEBUG_OBJECT(gzdec, "Faled to decompress data (error: %s) \n", zError(err));
            goto free_out;
        }

        // Output buf is full
        if (gzdec->zstrm.avail_out >= 0) {
            GST_DEBUG_OBJECT(gzdec, "Out buffer ready. Push it");
#if GST_CHECK_VERSION(1,0,0)
            gst_buffer_unmap(gzdec->out_buf, &gzdec->out_buf_map);
            gst_buffer_set_size(gzdec->out_buf, (gzdec->out_buf_capacity - gzdec->zstrm.avail_out));
#else
            gzdec->out_buf->size = gzdec->out_buf_capacity - gzdec->zstrm.avail_out;

#endif           
            gzdec->out_buf_boo = TRUE;
            ret_flow = gst_pad_push(gzdec->srcpad, gzdec->out_buf);
            if (ret_flow < 0) {
#if GST_CHECK_VERSION(1,0,0)
                goto unmap_in;
#else
                goto free_in;
#endif    
            }
        }
        if (err == Z_STREAM_END) {
            GST_DEBUG_OBJECT(gzdec, "Decompression finish. Send EOS");
            eos = gst_event_new_eos();
            gst_pad_push_event(gzdec->srcpad, eos);
          
#if GST_CHECK_VERSION(1,0,0)
            ret_flow = GST_FLOW_EOS;
            goto unmap_in;
            
#else
            ret_flow = GST_FLOW_UNEXPECTED;
            goto free_in;
#endif
        }
    } while (ret != Z_STREAM_END);

    ret_flow = GST_FLOW_OK;

#if GST_CHECK_VERSION(1,0,0)
    goto unmap_in;
    free_out:
        gst_buffer_unmap(gzdec->out_buf, &gzdec->out_buf_map);
        gst_buffer_unref(gzdec->out_buf);
    unmap_in:
        gst_buffer_unmap(in_buf, &in_buf_map);
    free_in:
        gst_buffer_unref(in_buf);
#else
    goto free_in;
    free_out:
        gst_buffer_unref(gzdec->out_buf);
    free_in:
        gst_buffer_unref(in_buf);
        gst_object_unref(gzdec);
#endif
    return ret_flow;
}


