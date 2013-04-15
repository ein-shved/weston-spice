#ifndef COMPOSITOR_SPICE_H
#define COMPOSITOR_SPICE_H

#include <spice.h>

#include <weston/compositor.h>
#include <assert.h>

#include "compositor-spice-conf.h"

#define NUM_MEMSLOTS        1
#define NUM_MEMSLOTS_GROUPS 1
#define NUM_SURFACES        2
#define MEMSLOT_ID_BITS     1
#define MEMSLOT_GEN_BITS    1

#define MEMSLOT_GROUP 0

#define MAX_WIDTH 1920
#define MAX_HEIGHT 1080

#define dprint(lvl, fmt, ...) \
    if ((lvl) <= DEBUG) weston_log("%s: " fmt "\n", __func__, ## __VA_ARGS__)
#define weston_log_error(err_str)\
    weston_log("%s: error detected. %s returned.\n", __func__, (err_str));
struct spice_compositor {
    struct weston_compositor base;

    SpiceServer *spice_server;

    QXLInstance display_sin;
    SpiceMouseInstance mouse_sin;
    SpiceKbdInstance kbd_sin;

    SpiceCoreInterface *core;
    SpiceTimer *wakeup_timer;
    QXLWorker *worker;
  
    struct spice_output *primary_output;
    struct weston_seat core_seat;

    void (*produce_command) (struct spice_compositor*);
    int (*push_command) (struct spice_compositor*, QXLCommandExt *);
    void (*release_resource) (struct spice_compositor*, QXLCommandExt *);

    void (*pixman_renderer_repaint_output) (struct weston_output *output_base,
            pixman_region32_t *damage);

};

uint32_t
spice_get_primary_surface_id (struct spice_compositor *c);

#endif //COMPOSITOR_SPICE_H
