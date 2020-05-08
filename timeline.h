#ifndef TIMELINE_H
#define TIMELINE_H 1

#include <libgimp/gimp.h>
#include "preview.h"
#include "frame.h"

#define MAX_FRAMES 100

static void set_active_frame_cb(GtkWidget* widget, GdkEvent* event, gpointer data);
static void initialize_frames();
static void remove_active_frame_cb();
static void play_animation_cb();
static void check_image_is_valid_and_update_size();
static void create_spritesheet();
gboolean update_active_frame_thumb_cb();
static void set_active_frame_cb(GtkWidget* widget, GdkEvent* event, gpointer data);
static void onion_skin_toggle_cb (GtkToggleButton *toggle_button, gpointer user_data);
static void onion_slider_change_cb(GtkRange* slider, gpointer user_data);
static gboolean entry_buffer_to_int(GtkEntryBuffer* buffer, int* number);

#endif