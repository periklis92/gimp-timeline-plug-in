#ifndef PREVIEW_H
#define PREVIEW_H 1

#include <libgimp/gimp.h>
#include <libgimp/gimpui.h>
#include "frame_types.h"

void preview_open(gint32 width, gint32 height, timeline_t* timeline, gint32 framerate);

#endif