#include "frame.h"
#include "preview.h"


timeline_t timeline = {.first_frame = NULL, .number_of_frames = 0};
frame_t* active_frame = NULL;

gint32 frame_get_number_of_frames()
{
    return timeline.number_of_frames;
}

frame_t* frame_get_first_frame()
{
    return timeline.first_frame;
}

frame_t* frame_get_active()
{
    return active_frame;
}


void frame_play_preview(gint32 width, gint32 height, gint32 framerate)
{
    preview_open(width, height, &timeline, framerate);
}

frame_t* frame_remove(gint32 image_ID, frame_t* frame_to_remove, gboolean is_active_frame,
                        gboolean onion_skin_active, gint opacity)
{
  if (frame_to_remove != NULL){
    frame_t* temp = frame_to_remove;
    gtk_widget_destroy(frame_to_remove->eventbox);
    gtk_widget_destroy(frame_to_remove->thumbnail);
    if (gimp_item_is_valid(frame_to_remove->layer_group_id)){
      gimp_image_remove_layer(image_ID, temp->layer_group_id);
    }

    frame_t* cur = timeline.first_frame;
    frame_t* next = temp->next_frame;
    frame_t* prev = NULL;
  
    if (timeline.number_of_frames > 1) {

      while (cur){
        if (temp->index == 0) timeline.first_frame = temp->next_frame;
        else if (cur->index == temp->index - 1) {prev = cur; prev->next_frame = next;}

        if (cur->index > temp->index ) {
          if (gimp_item_is_valid(cur->layer_group_id)){
            cur->index--;  
            char name[40];
            sprintf(name, "frame %d", cur->index);
            gimp_item_set_name(cur->layer_group_id, name);
          }
        }

        cur = cur->next_frame;
      }
      if (is_active_frame){
        if (next != NULL)
          frame_set_active(image_ID, next, onion_skin_active, opacity);
        else if (timeline.first_frame != NULL)
          frame_set_active(image_ID, timeline.first_frame, onion_skin_active, opacity);
        else
          active_frame = NULL;
      }
    } 
    else {
      timeline.first_frame = NULL;
      active_frame = NULL;
    }
    
    timeline.number_of_frames--;
    g_free(temp);
    gimp_displays_flush();
    return next;
  }
  return NULL;
}

void frame_add_to_timeline(frame_t* frame_to_add, GtkWidget* timeline_widget, void* active_frame_cb)
{
  if (timeline.number_of_frames >= 10){
    gtk_table_resize(GTK_TABLE(timeline_widget), 1, timeline.number_of_frames);
  }
  gtk_container_add(GTK_CONTAINER(frame_to_add->eventbox), frame_to_add->thumbnail);
  gtk_box_pack_start(GTK_BOX(timeline_widget), frame_to_add->eventbox, FALSE, FALSE, 2);

  gtk_event_box_set_above_child(GTK_EVENT_BOX(frame_to_add->eventbox), TRUE);
  gtk_event_box_set_visible_window(GTK_EVENT_BOX(frame_to_add->eventbox), TRUE);
  g_signal_connect(G_OBJECT(frame_to_add->eventbox), "button_press_event", 
    G_CALLBACK(active_frame_cb), frame_to_add);

  gtk_widget_show_all(frame_to_add->eventbox);
  gimp_displays_flush();
}

gboolean frame_update_thumb(frame_t* frame, gint32 image_width, gint32 image_height)
{
  if (frame != NULL){
    GtkWidget* preview = frame->thumbnail;
    GdkPixbuf* pixbuf = gimp_drawable_get_thumbnail(frame->layer_group_id, 
      image_width, image_height, GIMP_PIXBUF_SMALL_CHECKS);
    frame->image_buffer = gdk_pixbuf_copy(pixbuf);
    GdkPixbuf* pixbuf_small = gdk_pixbuf_scale_simple(pixbuf, FRAME_THUMBNAIL_SIZE, FRAME_THUMBNAIL_SIZE, GDK_INTERP_NEAREST);
    gtk_image_set_from_pixbuf(GTK_IMAGE(preview), pixbuf_small);
  }
  return TRUE;
}

void frame_set_active(gint32 image_ID, frame_t* frame, gboolean onion_skin, gint opacity)
{
  if (frame_is_valid(frame)){
    frames_check_all_valid_and_remove(image_ID, onion_skin, opacity);
    frame_t* next = timeline.first_frame;
    while(next != NULL){
        if (onion_skin && next->index == frame->index - 1){
            gimp_item_set_visible(next->layer_group_id, TRUE);
            gimp_layer_set_opacity(next->layer_group_id, opacity);
        }
        else if (next->index != frame->index) 
            gimp_item_set_visible(next->layer_group_id, FALSE);
        else{
            g_message("Active Frame Found");
            active_frame = next;
            gimp_item_set_visible(next->layer_group_id, TRUE);
            gint num_children;
            gint* children = gimp_item_get_children(next->layer_group_id, &num_children);
            gimp_layer_set_opacity(next->layer_group_id, 100);
            if (num_children > 0)
                gimp_image_set_active_layer(image_ID, children[0]);
            else
                gimp_image_set_active_layer(image_ID, next->layer_group_id);
        }
        next = next->next_frame;        
    }
    gimp_displays_flush();
  }
  else {
    frame_remove(image_ID, frame, FALSE, onion_skin, opacity);
  }
}

frame_t* frame_create_new_from_layer(gint32 layer_id, gint32 width, gint32 height)
{  
    GtkWidget* preview = gtk_image_new();
    GtkWidget* eventbox = gtk_event_box_new();
    frame_t* frame_created = g_malloc(sizeof(frame_t));

    frame_created->eventbox = eventbox;
    frame_created->index = timeline.number_of_frames;
    frame_created->layer_group_id = layer_id;
    frame_created->next_frame = NULL;
    frame_created->thumbnail = preview;
    frame_update_thumb(frame_created, width, height);

    if (timeline.first_frame == NULL)
        timeline.first_frame = frame_created;
    else
    {
        frame_t* cur = timeline.first_frame;
        while (cur->next_frame != NULL)
        {
            cur = cur->next_frame;
        }
        cur->next_frame = frame_created;
    }
    timeline.number_of_frames++;
    

    return frame_created;
}

gint32 frame_get_last_layer_id()
{
    if (timeline.number_of_frames > 0){
        frame_t* cur = timeline.first_frame;
        while(cur->next_frame != NULL){
            cur = cur->next_frame;
        }
        return cur->layer_group_id;
    }
    return 0;
}

frame_t* frame_create_new(gint32 image_ID, gint32 width, gint32 height, gboolean copy_last)
{
  char name[40];
  sprintf(name, "frame %d", timeline.number_of_frames);
  gint32 layer_id = 0;
  gint32 group_id = 0;
  if (timeline.number_of_frames == 0 || !copy_last){
    gint num_children = 1;
    gint* children;
    if (active_frame != NULL){
      children = gimp_item_get_children(active_frame->layer_group_id, &num_children);
    }
    group_id = gimp_layer_group_new(image_ID);
    gimp_item_set_name(group_id, name);
    gimp_image_insert_layer(image_ID, group_id, 0, 0);
    for (int i = 0; i < num_children; i++) {
      layer_id = gimp_layer_new(image_ID, name, width, height, GIMP_RGBA_IMAGE, 100, GIMP_LAYER_MODE_NORMAL);
      gimp_image_insert_layer(image_ID, layer_id, group_id, 0);
    }
  } else {
    group_id = gimp_layer_copy(frame_get_last_layer_id());
    gimp_item_set_name(group_id, name);
    gimp_image_insert_layer(image_ID, group_id, 0, 0);
  }
  return frame_create_new_from_layer(group_id, width, height);
}

gboolean frame_is_valid(frame_t* frame){
  return gimp_item_is_valid(frame->layer_group_id);
}

void frames_check_all_valid_and_remove(gint32 image_ID, gboolean onion_skin, gint32 opacity)
{
  frame_t* cur = timeline.first_frame;

  while(cur != NULL){
    if (!frame_is_valid(cur)){
      cur = frame_remove(image_ID, cur, FALSE, onion_skin, opacity);
      continue;
    }
    cur = cur->next_frame;
  }
}