#ifndef _WESTON_SPICE_INTERFACES_
#define _WESTON_SPICE_INTERFACES_

#include "compositor-spice.h"

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

struct spice_release_info {
    void (*destructor) (struct spice_release_info *);
};

typedef struct spice_compositor spice_compositor_t;

void weston_init_qxl_interface (spice_compositor_t *qxl);
void weston_init_mouse_interface (spice_compositor_t *c);
int weston_init_kbd_interface (spice_compositor_t *c);
void release_simple (struct spice_release_info *);

#endif //_WESTON_QXL_INTERFACE_
