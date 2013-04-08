#ifndef _WESTON_QXL_INTERFACE_
#define _WESTON_QXL_INTERFACE_

#include "compositor-spice.h"

#define NUM_MEMSLOTS 1
#define NUM_MEMSLOTS_GROUPS 1
#define MEMSLOT_ID_BITS 1
#define MEMSLOT_GEN_BITS 1
#define NUM_SURFACES 1

#define MEMSLOT_GROUP 1

#define DEFAULT_WIDTH 640
#define DEFAULT_HEIGHT 480

typedef struct spice_compositor spice_compositor_t;

void weston_init_qxl_interface (spice_compositor_t *qxl);

#endif //_WESTON_QXL_INTERFACE_
