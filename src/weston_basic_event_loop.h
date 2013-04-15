#ifndef __BASIC_EVENT_LOOP_H__
#define __BASIC_EVENT_LOOP_H__

#include <spice.h>
#include <weston/compositor.h>

SpiceCoreInterface *basic_event_loop_init(struct wl_display *display);

#endif // __BASIC_EVENT_LOOP_H__
