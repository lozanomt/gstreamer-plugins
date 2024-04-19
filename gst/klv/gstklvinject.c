/* GStreamer
 * Copyright (C) 2016 William Manley <will@williammanley.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Suite 500,
 * Boston, MA 02110-1335, USA.
 */
/**
 * SECTION:element-gstklvinject
 *
 * The klvinject element injects KLV metadata on passing buffers.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v fakesrc ! klvinject ! fakesink
 * ]|
 * FIXME Describe what the pipeline does.
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gst/gst.h>
#include <gst/base/gstbasetransform.h>
#include <gst/base/gstbytewriter.h>
#include "gstklvinject.h"
#include "klv.h"

GST_DEBUG_CATEGORY_STATIC (gst_klvinject_debug_category);
#define GST_CAT_DEFAULT gst_klvinject_debug_category

/* prototypes */
static GstFlowReturn gst_klvinject_transform_ip (GstBaseTransform * trans,
    GstBuffer * inbuf);

static GstFlowReturn gst_klvinject_prepare_output_buffer (GstBaseTransform *
    trans, GstBuffer * input, GstBuffer ** outbuf);

enum
{
  PROP_0
};

/* pad templates */

#define SRC_CAPS "ANY"
#define SINK_CAPS "ANY"


/* class initialization */

G_DEFINE_TYPE_WITH_CODE (GstKlvInject, gst_klvinject, GST_TYPE_BASE_TRANSFORM,
    GST_DEBUG_CATEGORY_INIT (gst_klvinject_debug_category, "klvinject", 0,
        "debug category for klvinject element"));

static void
gst_klvinject_class_init (GstKlvInjectClass * klass)
{
  GstBaseTransformClass *base_transform_class =
      GST_BASE_TRANSFORM_CLASS (klass);

  /* Setting up pads and setting metadata should be moved to
     base_class_init if you intend to subclass this class. */
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
      gst_pad_template_new ("src", GST_PAD_SRC, GST_PAD_ALWAYS,
          gst_caps_from_string (SRC_CAPS)));
  gst_element_class_add_pad_template (GST_ELEMENT_CLASS (klass),
      gst_pad_template_new ("sink", GST_PAD_SINK, GST_PAD_ALWAYS,
          gst_caps_from_string (SINK_CAPS)));

  gst_element_class_set_static_metadata (GST_ELEMENT_CLASS (klass),
      "Inject KLV", "Filter", "Inject KLV metadata",
      "Joshua M. Doe <oss@nvl.army.mil>");

  base_transform_class->transform_ip =
      GST_DEBUG_FUNCPTR (gst_klvinject_transform_ip);
  base_transform_class->prepare_output_buffer =
      GST_DEBUG_FUNCPTR (gst_klvinject_prepare_output_buffer);
}

static void
gst_klvinject_init (GstKlvInject * filt)
{
}

static void
gst_klvinject_dispose (GstKlvInject * filt)
{
}

static GstStaticCaps unix_reference = GST_STATIC_CAPS ("timestamp/x-unix");

static void
gst_klvinject_add_test_meta (GstKlvInject * filt, GstBuffer * buf)
{
/* Add KLV meta for testing, here: Motion Imagery Standards Board (MISB)
 * Engineering Guideline MISB EG 0902 - MISB Minimum Metadata Set.
 * Also see: SMPTE S336M for KLV specification, also ITU-R BT.1563-1 */
  const guint8 klv_header[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x0b, 0x01,
    0x01, 0x0e, 0x01, 0x03, 0x01, 0x01, 0x00, 0x00, 0x00
  };
  GstByteWriter bw;
  /* NOTE: MISB defines MISP time, which is NOT UTC, but use UTC for now */
  guint64 utc_us = -1;

#if GST_CHECK_VERSION(1,14,0)
  GstReferenceTimestampMeta *time_meta;
  time_meta =
      gst_buffer_get_reference_timestamp_meta (buf,
      gst_static_caps_get (&unix_reference));
  if (time_meta) {
    utc_us = time_meta->timestamp / 1000;
    GST_LOG_OBJECT (filt, "Found timestamp meta: %d.%06d sec", utc_us / 1000000,
        utc_us % 1000000);
  }
#endif

  if (utc_us == -1) {
    GDateTime *dt = g_date_time_new_now_utc ();
    utc_us = g_date_time_to_unix (dt) * 1000000;        /* microseconds */
    utc_us += g_date_time_get_microsecond (dt);
    g_date_time_unref (dt);
    GST_LOG_OBJECT (filt, "Grabbed time now: %d.%06d sec", utc_us / 1000000,
        utc_us % 1000000);
  }

  gst_byte_writer_init (&bw);

  /* KLV header */
  gst_byte_writer_put_data (&bw, klv_header, 16);

  /* Total length in BER long form (verbatim as long as <= 127) */
  gst_byte_writer_put_uint8 (&bw,
      (1 + 1 + 8) + (1 + 1 + 14) + (1 + 1 + 4) + (1 + 1 + 4) + (1 + 1 + 2));

  /* Tag 1: unix timestamp */
  gst_byte_writer_put_uint8 (&bw, 2);   /* local tag: unix timestamp    */
  gst_byte_writer_put_uint8 (&bw, 8);   /* data length (BER short form) */
  gst_byte_writer_put_uint64_be (&bw, utc_us);

  /* Tag 2: Image Coordinate System */
  gst_byte_writer_put_uint8 (&bw, 12);  /* local tag: unix timestamp    */
  gst_byte_writer_put_uint8 (&bw, 14);  /* data length (BER short form) */
  gst_byte_writer_put_data (&bw, (guint8 *) "Geodetic WGS84", 14);

  /* Tag 3: Sensor Latitude */
  gst_byte_writer_put_uint8 (&bw, 13);  /* local tag: latitude */
  gst_byte_writer_put_uint8 (&bw, 4);   /* data length (BER short form) */
  {
    /* Map -(2^31-1)..(2^31-1) to +/-90 with 0x80000000 = error */
    gdouble latitude = 51.449825;
    gint32 val = (gint) ((latitude / 90.0) * 2147483647.0);
    gst_byte_writer_put_int32_be (&bw, val);
  }

  /* Tag 4: Sensor Longitude */
  gst_byte_writer_put_uint8 (&bw, 14);  /* local tag: longitude */
  gst_byte_writer_put_uint8 (&bw, 4);   /* data length (BER short form) */
  {
    /* Map -(2^31-1)..(2^31-1) to +/-180 with 0x80000000 = error */
    gdouble longitude = -2.600439;
    gint32 val = (gint) ((longitude / 180.0) * 2147483647.0);
    gst_byte_writer_put_int32_be (&bw, val);
  }

  /* Tag 5: Sensor True Altitude (MSL) (elevation) */
  gst_byte_writer_put_uint8 (&bw, 15);  /* local tag: elevation */
  gst_byte_writer_put_uint8 (&bw, 2);   /* data length (BER short form) */
  {
    /* Map 0..(2^16-1) to -900..19000 meters, so resolution 0.303654536m */
    gdouble elevation = 10.0;
    guint16 val = ((elevation + 900.0 + 0.151827268) / 19900.0) * 65535.0;
    gst_byte_writer_put_int16_be (&bw, val);
  }

  {
    gsize klv_size = gst_byte_writer_get_size (&bw);
    guint8 *klv_data = gst_byte_writer_reset_and_get_data (&bw);

    gst_buffer_add_klv_meta_take_data (buf, klv_data, klv_size);
  }
}

static GstFlowReturn
gst_klvinject_prepare_output_buffer (GstBaseTransform * trans,
    GstBuffer * inbuf, GstBuffer ** outbuf)
{
  GstFlowReturn ret = GST_FLOW_OK;
  GstKlvInject *filt = GST_KLVINJECT (trans);

  *outbuf = gst_buffer_copy (inbuf);

  return GST_FLOW_OK;
}

static GstFlowReturn
gst_klvinject_transform_ip (GstBaseTransform * trans, GstBuffer * buf)
{
  GstKlvInject *filt = GST_KLVINJECT (trans);

  GST_LOG_OBJECT (filt, "Injecting test KLV metadata");
  gst_klvinject_add_test_meta (filt, buf);

  return GST_FLOW_OK;
}