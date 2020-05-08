#ifndef FRAME_H
#define FRAME_H 1

#define SPREADSHEET_X 6.0
#define FRAME_THUMBNAIL_SIZE 60

#include "frame_types.h"

frame_t* frame_create_new(gint32 image_ID, gint32 width, gint32 height, gboolean copy_last);
frame_t* frame_create_new_from_layer(gint32 layer_id, gint32 width, gint32 height);
gboolean frame_is_valid(frame_t* frame);
void frames_check_all_valid_and_remove(gint32 image_ID, gboolean onion_skin, gint32 opacity);
void frame_set_active(gint32 image_ID, frame_t* frame, gboolean onion_skin, gint opacity);
frame_t* frame_remove(gint32 image_ID, frame_t* frame_to_remove, gboolean is_active_frame,
                        gboolean onion_skin_active, gint opacity);
void frame_add_to_timeline(frame_t* frame_to_add, GtkWidget* timeline_widget, void* active_frame_cb);
gboolean frame_update_thumb(frame_t* frame, gint32 image_width, gint32 image_height);
gint32 frame_get_last_layer_id();
frame_t* frame_get_active();
gint32 frame_get_number_of_frames();
frame_t* frame_get_first_frame();
void frame_play_preview(gint32 width, gint32 height, gint32 framerate);

#endif