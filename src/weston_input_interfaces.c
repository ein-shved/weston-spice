#include "compositor-spice.h"
#include "weston_spice_interfaces.h"

#define button_equal(old, new, mask) \
    ( ( (old) & (mask) ) == ( (new) & (mask) ) )

static void
weston_mouse_button_notify (spice_compositor_t *c, uint32_t buttons_state)
{
    static uint32_t buttons = 0;
    enum wl_pointer_button_state state;
    
    if ( !button_equal (buttons, buttons_state, SPICE_MOUSE_BUTTON_MASK_LEFT ) ) {
        state = buttons_state & SPICE_MOUSE_BUTTON_MASK_LEFT ? 
                WL_POINTER_BUTTON_STATE_PRESSED :
                WL_POINTER_BUTTON_STATE_RELEASED;
        dprint(3, "button %x", buttons_state );        
        notify_button (&c->core_seat, weston_compositor_get_time(),
                    1, state );
    }
    buttons = buttons_state;

}

static void
weston_mouse_motion (SpiceMouseInstance *sin, int dx, int dy, int dz,
        uint32_t buttons_state)
{
    spice_compositor_t *c = container_of(sin, spice_compositor_t, mouse_sin);

    dprint (3, "called. delta: (%d,%d,%d), buttons: %x c: %x",
            dx,dy,dz,buttons_state, c);
    if (!c->core_seat.has_pointer) {
        return;
    }
    notify_motion(&c->core_seat, weston_compositor_get_time(), 50*dx, 50*dy );

    weston_mouse_button_notify (c, buttons_state);
}
static void 
weston_mouse_buttons (SpiceMouseInstance *sin, uint32_t buttons_state )
{
    spice_compositor_t *c = container_of(sin, spice_compositor_t, mouse_sin);

    dprint (3, "called. Buttons: %x", buttons_state);
    weston_mouse_button_notify (c, buttons_state);
}

static struct SpiceMouseInterface weston_mouse_interface = {
    .base.type          = SPICE_INTERFACE_MOUSE,
    .base.description   = "weston mouse",
    .base.major_version = SPICE_INTERFACE_MOUSE_MAJOR,
    .base.minor_version = SPICE_INTERFACE_MOUSE_MINOR,

    .motion     = weston_mouse_motion,
    .buttons    = weston_mouse_buttons,
};

void 
weston_init_mouse_interface (spice_compositor_t *c)
{
    c->mouse_sin.base.sif   = &weston_mouse_interface.base;
    c->mouse_sin.st         = (SpiceMouseState*) c;
    weston_seat_init_pointer (&c->core_seat);
}

static void 
weston_kbd_push_scan_frag (SpiceKbdInstance *sin, uint8_t frag) 
{
    spice_compositor_t *c = container_of(sin, spice_compositor_t, kbd_sin);

    dprint (2, "called: %x", frag);

    notify_key (&c->core_seat, weston_compositor_get_time(), frag,
                    WL_KEYBOARD_KEY_STATE_PRESSED, STATE_UPDATE_AUTOMATIC );

}
static uint8_t 
weston_kbd_get_leds (SpiceKbdInstance *sin) 
{
    spice_compositor_t *c = container_of(sin, spice_compositor_t, kbd_sin);

    return 0;
}
static struct SpiceKbdInterface weston_kbd_interface = {
    .base.type          = SPICE_INTERFACE_KEYBOARD,
    .base.description   = "weston keyboard",
    .base.major_version = SPICE_INTERFACE_KEYBOARD_MAJOR,
    .base.minor_version = SPICE_INTERFACE_KEYBOARD_MINOR,

    .push_scan_freg     = weston_kbd_push_scan_frag,
    .get_leds           = weston_kbd_get_leds,
};
int
weston_init_kbd_interface (spice_compositor_t *c)
{
    c->kbd_sin.base.sif = &weston_kbd_interface.base;
    c->kbd_sin.st       = (SpiceKbdState*) c;
    if ( weston_seat_init_keyboard (&c->core_seat, NULL) < 0 ) {
        dprint (1, "failed to init seat keyboard");
        return -1;
    }

    return 0;
}
