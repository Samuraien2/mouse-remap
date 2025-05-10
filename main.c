#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

#define MOUSE_DEVICE "/dev/input/event7"

void send_key(Display *dpy, int keycode) {
    XTestFakeKeyEvent(dpy, keycode, 1, CurrentTime);  // key down
    XTestFakeKeyEvent(dpy, keycode, 0, CurrentTime); // key up
    XFlush(dpy);
}

int main() {
    // Open input device
    int fd = open(MOUSE_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Opening input device");
        return 1;
    }

    // Connect to X server
    Display *dpy = XOpenDisplay(NULL);
    if (!dpy) {
        fprintf(stderr, "Cannot open X display\n");
        return 1;
    }

    // Get X keycodes for '8' and '9'
    KeyCode key8 = XKeysymToKeycode(dpy, XStringToKeysym("8"));
    KeyCode key9 = XKeysymToKeycode(dpy, XStringToKeysym("9"));

    struct input_event ev;
    while (1) {
        if (read(fd, &ev, sizeof(ev)) != sizeof(ev)) continue;

        if (ev.type == EV_KEY && ev.value == 1) { // press only
            if (ev.code == BTN_SIDE) {
                send_key(dpy, key9);
            } else if (ev.code == BTN_EXTRA) {
                send_key(dpy, key8);
            }
        }
    }

    close(fd);
    XCloseDisplay(dpy);
    return 0;
}
