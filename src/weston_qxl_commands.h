#ifndef WESTON_QXL_COMMANDS_H
#define WESTON_QXL_COMMANDS_H

#include "compositor-spice.h"
#include "weston_spice_interfaces.h"

#define COLOR_RGB(r,g,b) \
    (((r)<<16) | ((g)<<8) | ((b)<<0) | 0xff000000)

#define COLOR_ARGB(a,r,g,b) \
    (((a)<<24) | ((r)<<16) | ((g)<<8) | ((b)<<0))

#define COLOR_RGB_F(r,g,b) \
    ( ( ((int) ((r)*255)) << 16) | ( ((int) ((g)*255)) <<  8) | \
      ( ((int) ((b)*255)) <<  0) | 0xff000000 )

#define COLOR_ARGB_F(a,r,g,b) \
    ( ( ((int) ((a)*255)) << 24) | ( ((int) ((r)*255)) << 16) | \
      ( ((int) ((g)*255)) <<  8) | ( ((int) ((b)*255)) <<  0) )

typedef uint32_t color_t;

uint32_t
spice_create_primary_surface (struct spice_compositor *c,
        int width, int height, uint8_t *data);

uint32_t
spice_create_image (struct spice_compositor *c);

int
spice_print_image (struct spice_compositor *c, uint32_t image_id,
        int x, int y, int width, int wight,
        intptr_t data, int32_t stride, 
        pixman_region32_t *region);
#endif //WESTON_QXL_COMMANDS_H
