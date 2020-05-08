#include "preview.h"

typedef struct preview_info{
  timeline_t* timeline;
  GtkWidget* image;
  frame_t* cur_frame;
  gboolean isPlaying;
}preview_info_t;

gboolean next_frame_cb(gpointer user_data)
{
  preview_info_t* pr = (preview_info_t*)user_data;
  gtk_image_set_from_pixbuf(GTK_IMAGE(pr->image), pr->cur_frame->image_buffer);
  pr->cur_frame = pr->cur_frame->next_frame;
  if (pr->cur_frame == NULL)
    pr->cur_frame = pr->timeline->first_frame;
  return pr->isPlaying;
}

void window_close_cb(GtkWidget *widget, gpointer user_data)
{
  preview_info_t* pr = (preview_info_t*)user_data;
  pr->isPlaying = FALSE;
}

void preview_open(gint32 width, gint32 height, timeline_t* timeline, gint32 framerate)
{
  GtkWidget* preview_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_default_size(GTK_WINDOW(preview_window), width, height);
  gtk_widget_show(preview_window);

  GtkWidget* preview_image = gtk_image_new();
  gtk_container_add(GTK_CONTAINER(preview_window), preview_image);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(preview_window))), preview_image, TRUE, TRUE, 0);
  gtk_widget_show(preview_image);
  preview_info_t* pr = g_malloc(sizeof(preview_info_t));
  pr->image = preview_image;
  pr->timeline = timeline;
  pr->cur_frame = timeline->first_frame;
  pr->isPlaying = TRUE;

  g_signal_connect(G_OBJECT(preview_window), "destroy", G_CALLBACK(window_close_cb), (gpointer)pr);
  g_timeout_add(1000 / framerate, next_frame_cb, (gpointer)pr);
}