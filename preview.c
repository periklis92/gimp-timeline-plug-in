#include "preview.h"

typedef struct preview_info{
  timeline_t* timeline;
  GtkWidget* image;
  frame_t* cur_frame;
  gint32 image_width;
  gint32 image_height;
  gboolean isPlaying;
}preview_info_t;

gboolean next_frame_cb(gpointer user_data)
{
  preview_info_t* pr = (preview_info_t*)user_data;
  GdkPixbuf* pixbuf = gimp_drawable_get_thumbnail(pr->cur_frame->layer_group_id,
                        pr->image_width, pr->image_height, GIMP_PIXBUF_SMALL_CHECKS);
  gtk_image_set_from_pixbuf(GTK_IMAGE(pr->image), pixbuf);
  g_object_unref(G_OBJECT(pixbuf));
  pr->cur_frame = pr->cur_frame->next_frame;
  if (pr->cur_frame == NULL)
    pr->cur_frame = pr->timeline->first_frame;
  return pr->isPlaying;
}

void window_close_cb(GtkWidget *widget, gpointer user_data)
{
  preview_info_t* pr = (preview_info_t*)user_data;
  g_object_unref(G_OBJECT(pr->image));
  pr->isPlaying = FALSE;
}

void preview_open(gint32 width, gint32 height, timeline_t* timeline, gint32 framerate)
{
  GtkWidget* preview_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(preview_window), width, height);
  gtk_window_set_position(GTK_WINDOW(preview_window), GTK_WIN_POS_CENTER);
  gtk_window_set_keep_above(GTK_WINDOW(preview_window), TRUE);

  GtkWidget* layout = gtk_layout_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER (preview_window), layout);
  gtk_widget_show(layout);

  GtkWidget* preview_image = gtk_image_new();
  gtk_layout_put(GTK_LAYOUT(layout), preview_image, 0, 0);
  gtk_widget_show(preview_image);

  preview_info_t* pr = g_malloc(sizeof(preview_info_t));
  pr->image = preview_image;
  pr->image_width = width;
  pr->image_height = height;
  pr->timeline = timeline;
  pr->cur_frame = timeline->first_frame;
  pr->isPlaying = TRUE;

  gtk_widget_show_all(preview_window);

  g_signal_connect(G_OBJECT(preview_window), "destroy", G_CALLBACK(window_close_cb), (gpointer)pr);
  g_timeout_add(1000 / framerate, next_frame_cb, (gpointer)pr);
}