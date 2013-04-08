#ifndef COMPOSITOR_SPICE_H
#define COMPOSITOR_SPICE_H

#include <weston/compositor.h>
#include <assert.h>

#include "compositor-spice-conf.h"

#define MEMSLOT_GROUP 1

#define NUM_SURFACES 1
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
    SpiceCoreInterface *core;
    SpiceTimer *wakeup_timer;
    QXLWorker *worker;
    
    struct spice_output *primary_output;

    void (*produce_command) (struct spice_compositor*);
    int (*push_command) (struct spice_compositor*, QXLCommandExt *);
    void (*release_resource) (struct spice_compositor*, QXLCommandExt *);
};

#endif //COMPOSITOR_SPICE_H
