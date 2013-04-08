/*
 * Copyright © 2013 Yury Shvedov <shved@lvk.cs.msu.su>
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  The copyright holders make
 * no representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <stdlib.h>
#include <string.h>

#include <weston/compositor.h>
#include <spice.h>
#include <spice/qxl_dev.h>
#include <spice/macros.h>
#include <pixman.h>

#include "compositor-spice.h"
#include "weston_basic_event_loop.h"

struct spice_server_ops {
    const char* addr;
    int flags;
    int port;
    int no_auth;
};
struct spice_output {
    struct weston_output base;

    struct weston_mode mode;

    uint32_t spice_surface_id;
    uint8_t *surface;
};

struct surface_create_cmd {
    QXLCommandExt ext; //first
    QXLSurfaceCmd cmd;
};


static void
spice_destroy(struct weston_compositor *ec)
{
}
static void
spice_restore(struct weston_compositor *ec)
{
}

static void set_cmd(QXLCommandExt *ext, uint32_t type, QXLPHYSICAL data)
{
    ext->cmd.type = type;
    ext->cmd.data = data;
    ext->cmd.padding = 0;
    ext->group_id = MEMSLOT_GROUP;
    ext->flags = 0;
}
static void set_release_info(QXLReleaseInfo *info, intptr_t ptr)
{
    info->id = ptr;
    //info->group_id = MEMSLOT_GROUP;
}

static struct surface_create_cmd *
spice_compositor_create_surface_cmd (int width, int height, 
                    uint32_t id, uint8_t *data )
{
    struct surface_create_cmd *surface_cmd;
    QXLSurfaceCmd *qxl_cmd;

    surface_cmd = malloc (sizeof *surface_cmd);
    if (surface_cmd == NULL) {
        goto err_surface_cmd_malloc;
    }
    qxl_cmd = &surface_cmd->cmd;

    set_cmd (&surface_cmd->ext, QXL_CMD_SURFACE, (intptr_t) qxl_cmd);
    set_release_info (&qxl_cmd->release_info, (intptr_t) surface_cmd);
    qxl_cmd->type   = QXL_SURFACE_CMD_CREATE;
    qxl_cmd->flags  = 0;
    qxl_cmd->surface_id = id;
    qxl_cmd->u.surface_create.format    = SPICE_SURFACE_FMT_32_ARGB;
    qxl_cmd->u.surface_create.width     = width;
    qxl_cmd->u.surface_create.height    = height;
    qxl_cmd->u.surface_create.stride    = -width * 4;
    qxl_cmd->u.surface_create.data      = (intptr_t) data;

    return surface_cmd;

err_surface_cmd_malloc:
    return NULL;
    weston_log_error("NULL");
}


static uint8_t *
spice_compositor_create_surface_empty ( struct spice_compositor *c,
                    int width, int height, uint32_t *id )
{
    static int surfaces_count = 0;
    uint8_t *surface;
    struct surface_create_cmd *surface_cmd;
    
    if (surfaces_count >= NUM_SURFACES ) {
        weston_log ("WARNING: surface number owerflow\n");
        goto err_surfaces_num;
    }
    if ( c->push_command == NULL ) {
        weston_log ("ERROR: no push command in spice compositor\n");
        goto err_push_command;
    }
    if ( width > MAX_WIDTH || height > MAX_HEIGHT) {
        goto err_max_params;
    }
    surface = malloc (width * height * 4 );
    if (surface == NULL) {
        goto err_surface_malloc;
    }
    memset (surface, 0, width * height * 4 );
    *id = surfaces_count ++;

    surface_cmd = spice_compositor_create_surface_cmd ( width, 
                height, *id, surface);
    if (surface_cmd == NULL) {
        goto err_surface_cmd;
    }
    c->push_command (c, &surface_cmd->ext);
   
    return surface;

err_surface_cmd:
    free (surface);
err_surface_malloc:
err_surfaces_num:
err_push_command:
err_max_params:
    weston_log_error("NULL");
    return NULL;
}

static void 
spice_output_repaint ( struct weston_output *output_base,
                pixman_region32_t *damage)
{
}
static void
spice_output_destroy ( struct weston_output *output_base)
{
}

static struct spice_output *
spice_create_output ( struct spice_compositor *c,
                      int x, int y,
                      int width, int height,
                      uint32_t transform )
{
    struct spice_output *output;

    output = malloc (sizeof *output);
    if (output == NULL) {
        goto err_output_malloc;
    }
    memset (output, 0, sizeof *output);
    
    output->surface =
        spice_compositor_create_surface_empty (c, width, height,
            &output->spice_surface_id);
    if (output->surface == NULL) {
        goto err_surface_create;
    }

	output->mode.flags =
		WL_OUTPUT_MODE_CURRENT | WL_OUTPUT_MODE_PREFERRED;
	output->mode.width = width;
	output->mode.height = height;
	output->mode.refresh = 60000;
	wl_list_init(&output->base.mode_list);
	wl_list_insert(&output->base.mode_list, &output->mode.link);

    output->base.repaint        = spice_output_repaint;
    output->base.destroy        = spice_output_destroy;
    output->base.assign_planes  = NULL;
	output->base.set_backlight  = NULL;
	output->base.set_dpms       = NULL;
	output->base.switch_mode    = NULL;
    
    output->base.current    = &output->mode;
    output->base.make       = "none";
	output->base.model      = "none";

    weston_output_init ( &output->base, &c->base,
                x, y, width, height, transform );

    wl_list_insert(c->base.output_list.prev, &output->base.link);

    weston_log ("Spice output created on (%d,%d), width: %d, height: %d\n",
                x,y,width,height);

    return output;

    
    free (output->surface);
err_surface_create:
    free (output);
err_output_malloc:
    weston_log_error("NULL");
    return NULL;
}

static void weston_server_new ( struct spice_compositor *c, 
                                const struct spice_server_ops *ops )
{
    //Init spice server
    c->spice_server = spice_server_new();
    
    spice_server_set_addr ( c->spice_server,
                            ops->addr, ops->flags );
    spice_server_set_port ( c->spice_server,
                            ops->port );
    if (ops->no_auth ) {
        spice_server_set_noauth ( c->spice_server );
    }

    //TODO set another spice server options here   

    spice_server_init (c->spice_server, c->core);
}

struct spice_compositor *
spice_compositor_create ( struct wl_display *display, 
                          const struct spice_server_ops *ops,
                          int *argc, char *argv[], const char *config_file)
{
    struct spice_compositor *c;

    c = malloc(sizeof *c);
    if ( c == NULL ) {
        goto err_malloc_compositor;
    }
    memset (c, 0, sizeof *c);

    if (weston_compositor_init (&c->base, display, argc, argv, 
        config_file) < 0) 
    {
        goto err_weston_init;
    }
   
    c->core = basic_event_loop_init();
    weston_server_new (c, ops);

    weston_log ("Spice server is up on %s:%d\n", ops->addr, ops->port);

    c->base.wl_display = display;
    if ( pixman_renderer_init (&c->base) < 0) {
        goto err_init_pixman;
    }
    weston_log ("Using %s renderer\n", "pixman");

    c->base.destroy = spice_destroy;
    c->base.restore = spice_restore;

    weston_init_qxl_interface (c);

    c->primary_output = spice_create_output ( c,
                0, 0, //(x,y)
                640, 420, //WIDTH x HEIGTH
                0 ); //transform ?
    if (c->primary_output == NULL ) {
        goto err_output;
    }

    return c;

err_output:
err_init_pixman:
err_weston_init:
    free (c);
err_malloc_compositor:
    return NULL;
}

WL_EXPORT struct weston_compositor *
backend_init( struct wl_display *display, int *argc, char *argv[],
	          const char *config_file)
{
    struct spice_compositor *c;
    struct spice_server_ops ops = {
        .addr = "localhost",
        .port = 5912,
        .flags = 0,
        .no_auth = TRUE,
    };

    const struct weston_option spice_options[] = {
		{ WESTON_OPTION_STRING,  "host", 0, &ops.addr },
		{ WESTON_OPTION_INTEGER, "port", 0, &ops.port },
        //TODO parse auth options here
	};

    parse_options (spice_options, ARRAY_LENGTH (spice_options), argc, argv);
    weston_log ("Initialising spice compositor\n");
    c = spice_compositor_create ( display, &ops,
            argc, argv, config_file);
    if (c == NULL ) {
        return NULL;
    }
    return &c->base;
}

