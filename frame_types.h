#ifndef FRAME_TYPES_H
#define FRAME_TYPES_H 1

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>

typedef struct frame_str
{
    gint32 layer_group_id;
    GtkWidget* thumbnail;
    GtkWidget* eventbox;
    gint32 index;
    GdkPixbuf* image_buffer;
    struct frame_str* next_frame;
}frame_t;

typedef struct timeline_str
{
    frame_t* first_frame;
    gint32 number_of_frames;
}timeline_t;

#endif