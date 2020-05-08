#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include "timeline.h"

#define PLUG_IN_PROC "plug-in-animation"

static gboolean animation_dialog();
static void query (void);
static void run   (const gchar      *name,
                   gint              nparams,
                   const GimpParam  *param,
                   gint             *nreturn_vals,
                   GimpParam       **return_vals);

gint32 image_ID = 0;
gint32 drawable_id = 0;

gint image_width = 0;
gint image_height = 0;

GtkWidget* timelinecols = NULL;
GtkEntryBuffer* framerateBuffer = NULL;
GtkEntryBuffer* spreads_h_axis = NULL;
gboolean isAnimationPlaying = FALSE;
gboolean onion_skin_active = FALSE;
gboolean copy_last_frame = FALSE;
gint onion_opacity = 35;
  
GimpPlugInInfo PLUG_IN_INFO =
{
  NULL,
  NULL,
  query,
  run
};

MAIN()

static void
query (void)
{
  static const GimpParamDef args[] =
  {
    {
      GIMP_PDB_INT32,
      "run-mode",
      "Run mode"
    },
    {
      GIMP_PDB_IMAGE,
      "image",
      "Input image"
    },
    {
      GIMP_PDB_DRAWABLE,
      "drawable",
      "Input drawable"
    }
  };

  gimp_install_procedure (
    PLUG_IN_PROC,
    "Animation Timeline...",
    "A tool to help you make animation in GIMP",
    "Periklis Stinis",
    "Copyright Periklis Stinis",
    "2020",
    "_Timeline...",
    "RGB*, GRAY*",
    GIMP_PLUGIN,
    G_N_ELEMENTS (args), 0,
    args, NULL);

  gimp_plugin_menu_register (PLUG_IN_PROC,
                             "<Image>/Filters/Animation");
}

static void
run (const gchar      *name,
     gint              nparams,
     const GimpParam  *param,
     gint             *nreturn_vals,
     GimpParam       **return_vals)
{
  static GimpParam  values[2];
  GimpDrawable *drawable;
  GimpPDBStatusType status = GIMP_PDB_SUCCESS;
  GimpRunMode       run_mode;
  
  /* Setting mandatory output values */
  *nreturn_vals = 1;
  *return_vals  = values;

  run_mode = param[0].data.d_int32;

  values[0].type = GIMP_PDB_STATUS;
  values[0].data.d_status = status;

  image_ID = param[1].data.d_image;
  drawable_id = param[2].data.d_drawable;

  image_width = gimp_image_width(image_ID);
  image_height = gimp_image_height(image_ID);

  if (run_mode != GIMP_RUN_NONINTERACTIVE){
    if (!animation_dialog())
        return;
  }
}

gboolean entry_buffer_to_int(GtkEntryBuffer* buffer, int* number)
{
  const char* buf = gtk_entry_buffer_get_text(buffer);
  char* cpy = (char*)buf;
  gboolean isNumber = FALSE;
  while(*cpy != '\0')
  {
    isNumber = isdigit(*cpy);
    cpy++;
  }
  *number = atoi(buf);
  return isNumber;
}

void play_animation_cb()
{
  if (frame_get_number_of_frames() > 1 && !isAnimationPlaying){
    int fr = 0;
    if (entry_buffer_to_int(framerateBuffer, &fr)){
      check_image_is_valid_and_update_size();
      frame_play_preview(image_width, image_height, fr);
    }
  }
}

void onion_skin_toggle_cb (GtkToggleButton *toggle_button, gpointer user_data)
{
  check_image_is_valid_and_update_size();
  onion_skin_active = gtk_toggle_button_get_active(toggle_button);
  if (frame_get_active() != NULL)
    frame_set_active(image_ID, frame_get_active(), onion_skin_active, onion_opacity);
}

void copy_last_toggle_cb (GtkToggleButton *toggle_button, gpointer user_data)
{
  copy_last_frame = gtk_toggle_button_get_active(toggle_button);
}

void onion_slider_change_cb(GtkRange* slider, gpointer user_data)
{
  onion_opacity = gtk_range_get_value(slider);
  if (onion_skin_active)
    frame_set_active(image_ID, frame_get_active(), onion_skin_active, onion_opacity);
}

gboolean update_active_frame_thumb_cb()
{
  check_image_is_valid_and_update_size();
  return frame_update_thumb(frame_get_active(), image_width, image_height);
}

void create_frame_cb()
{
  check_image_is_valid_and_update_size();
  frame_t* new_frame = frame_create_new(image_ID, image_width, image_height, copy_last_frame);
  frame_set_active(image_ID, new_frame, onion_skin_active, onion_opacity);
  frame_add_to_timeline(new_frame, timelinecols, set_active_frame_cb);
}

static gboolean animation_dialog()
{

  gimp_ui_init("Animation Timeline", FALSE);

  GtkWidget* dialog = gimp_dialog_new("Animation Timeline", "animation-timeline",
                          NULL, 0, NULL, PLUG_IN_PROC, NULL);
  gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);
  gimp_window_set_transient(GTK_WINDOW(dialog));
  gtk_widget_set_size_request(dialog, 800, 180);
  gtk_widget_show(dialog);

  GtkWidget* topbuttons = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
  gtk_container_set_border_width(GTK_CONTAINER(topbuttons), 2);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), topbuttons, FALSE, FALSE, 0);
  gtk_widget_show(topbuttons);

  GtkWidget* timelinebox = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_set_border_width(GTK_CONTAINER(timelinebox), 2);
  gtk_box_pack_start(GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), timelinebox, TRUE, TRUE, 2);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(timelinebox), GTK_POLICY_AUTOMATIC, GTK_POLICY_NEVER);
  gtk_widget_show(timelinebox);
  timelinecols = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
  gtk_box_pack_start(GTK_BOX(timelinebox), timelinecols, FALSE, FALSE, 0);
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(timelinebox), timelinecols);
  gtk_widget_show(timelinecols);

  GtkWidget* addKeyframe = gtk_button_new_with_label("Add Keyframe");
  gtk_box_pack_start(GTK_BOX(topbuttons), addKeyframe, FALSE, FALSE, 0);
  gtk_widget_show(addKeyframe);
  g_signal_connect(addKeyframe, "pressed", G_CALLBACK(create_frame_cb), NULL);

  GtkWidget* vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 1);
  gtk_container_set_border_width(GTK_CONTAINER(topbuttons), 0);
  gtk_box_pack_start(GTK_BOX(topbuttons), vertbox, FALSE, FALSE, 0);
  gtk_widget_show(vertbox);


  GtkWidget* onion_btn = gtk_check_button_new_with_label("Onion Skin");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(onion_btn), onion_skin_active);
  gtk_box_pack_start(GTK_BOX(vertbox), onion_btn, FALSE, FALSE, 0);
  gtk_widget_show(onion_btn);
  g_signal_connect(onion_btn, "toggled", G_CALLBACK(onion_skin_toggle_cb), NULL);

  GtkWidget* onion_slider = gtk_hscale_new(NULL);
  gtk_scale_set_draw_value(GTK_SCALE(onion_slider), FALSE);
  gtk_range_set_range(GTK_RANGE(onion_slider), 0, 100);
  gtk_range_set_value(GTK_RANGE(onion_slider), 35);
  gtk_box_pack_start(GTK_BOX(vertbox), onion_slider, FALSE, FALSE, 0);
  gtk_widget_show(onion_slider);
  g_signal_connect(onion_slider, "value-changed", G_CALLBACK(onion_slider_change_cb), NULL);

  GtkWidget* removeKeyframe = gtk_button_new_with_label("Remove Keyframe");
  gtk_box_pack_start(GTK_BOX(topbuttons), removeKeyframe, FALSE, FALSE, 0);
  gtk_widget_show(removeKeyframe);
  g_signal_connect(removeKeyframe, "pressed", G_CALLBACK(remove_active_frame_cb), NULL);

  GtkWidget* playButton = gtk_button_new_with_label("Play");
  gtk_box_pack_start(GTK_BOX(topbuttons), playButton, FALSE, FALSE, 0);
  gtk_widget_show(playButton);
  g_signal_connect(playButton, "pressed", G_CALLBACK(play_animation_cb), NULL);

  GtkWidget* framerateLabel = gtk_label_new("Framerate: ");
  gtk_box_pack_start(GTK_BOX(topbuttons), framerateLabel, FALSE, FALSE, 0);
  gtk_widget_show(framerateLabel);

  framerateBuffer = gtk_entry_buffer_new("30", 2);

  GtkWidget* framerateEntry = gtk_entry_new_with_buffer(framerateBuffer);
  gtk_box_pack_start(GTK_BOX(topbuttons), framerateEntry, FALSE, FALSE, 0);
  gtk_widget_show(framerateEntry);
  gtk_widget_set_size_request(framerateEntry, 30, 30);
  gtk_entry_set_max_length(GTK_ENTRY(framerateEntry), 3);

  GtkWidget* ssButton = gtk_button_new_with_label("Create Sprite Sheet...");
  gtk_box_pack_start(GTK_BOX(topbuttons), ssButton, FALSE, FALSE, 0);
  gtk_widget_show(ssButton);
  g_signal_connect(ssButton, "pressed", G_CALLBACK(create_spritesheet), NULL);

  spreads_h_axis = gtk_entry_buffer_new("6", 2);

  GtkWidget* spread_s_entry = gtk_entry_new_with_buffer(spreads_h_axis);
  gtk_box_pack_start(GTK_BOX(topbuttons), spread_s_entry, FALSE, FALSE, 0);
  gtk_widget_show(spread_s_entry);
  gtk_widget_set_size_request(spread_s_entry, 30, 30);
  gtk_entry_set_max_length(GTK_ENTRY(spread_s_entry), 3);

  GtkWidget* copy_btn = gtk_check_button_new_with_label("Copy Last");
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(copy_btn), copy_last_frame);
  gtk_box_pack_start(GTK_BOX(topbuttons), copy_btn, FALSE, FALSE, 0);
  gtk_widget_show(copy_btn);
  g_signal_connect(copy_btn, "toggled", G_CALLBACK(copy_last_toggle_cb), NULL);

  initialize_frames();
  g_timeout_add(100, update_active_frame_thumb_cb, NULL);

  while(TRUE){
    GtkResponseType run = gimp_dialog_run(GIMP_DIALOG(dialog));
    if (run != GTK_RESPONSE_OK){
      gtk_widget_destroy(dialog);
      return FALSE;
    }
  }

  return FALSE;

}

void remove_active_frame_cb(){
  check_image_is_valid_and_update_size();
  frame_remove(image_ID, frame_get_active(), TRUE, onion_skin_active, onion_opacity);
}

void check_image_is_valid_and_update_size()
{
  if (!gimp_image_is_valid(image_ID)) {
    gimp_quit();
  }
  image_width = gimp_image_width(image_ID);
  image_height = gimp_image_height(image_ID);
}

void create_spritesheet()
{
  int h_axis = 0;
  gboolean is_number = entry_buffer_to_int(spreads_h_axis, &h_axis);
  if (is_number){
    gint new_rows = (gint)ceil(frame_get_number_of_frames() / (double)h_axis);

    gint new_width = h_axis * image_width;
    if (frame_get_number_of_frames() < h_axis)
      new_width = frame_get_number_of_frames() * image_width;
    
    gint new_height = new_rows * image_height;
    gint32 new_image_id = gimp_image_new(new_width, new_height, GIMP_RGB);

    frame_t* cur = frame_get_first_frame();
    int i = 0;
    while(cur != NULL){
      GdkPixbuf* pixbuf = gimp_drawable_get_thumbnail(cur->layer_group_id, 
        image_width, image_height, GIMP_PIXBUF_KEEP_ALPHA);
      gint32 nid = gimp_layer_new_from_pixbuf(new_image_id, "frame", pixbuf, 100, GIMP_LAYER_MODE_NORMAL, 0, 100);
      gimp_image_insert_layer(new_image_id, nid, 0, cur->index);
      gimp_item_transform_translate(nid, image_width * (i % (int)h_axis), floor(i / h_axis) * image_height);
      i++;
      cur = cur->next_frame;
    }
    gimp_display_new(new_image_id);
    gimp_displays_flush();
  }
}

void set_active_frame_cb(GtkWidget* widget, GdkEvent* event, gpointer data)
{
  frame_t* cur = (frame_t*)data;
  frame_set_active(image_ID, cur, onion_skin_active, onion_opacity);
}

void initialize_frames()
{
  for (int i = 0; i < MAX_FRAMES; i++){
    char buf[40];
    sprintf(buf, "frame %d", i);
    gint32 layer_id = gimp_image_get_layer_by_name(image_ID, buf);
    if (layer_id == -1) {return;}
    check_image_is_valid_and_update_size();
    frame_t* frame_created = frame_create_new_from_layer(layer_id, image_width, image_height);
    frame_set_active(image_ID, frame_created, onion_skin_active, onion_opacity);
    frame_add_to_timeline(frame_created, timelinecols, set_active_frame_cb);
  }
}
