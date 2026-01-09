#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MOUSE_DEVICE "/dev/input/by-id/usb-Razer_Razer_DeathAdder_Elite-event-mouse"

void setup_uinput(int fd) {
    ioctl(fd, UI_SET_EVBIT, EV_KEY);
    ioctl(fd, UI_SET_KEYBIT, KEY_8);
    ioctl(fd, UI_SET_KEYBIT, KEY_9);
    ioctl(fd, UI_SET_KEYBIT, BTN_LEFT);
    ioctl(fd, UI_SET_KEYBIT, BTN_RIGHT);
    ioctl(fd, UI_SET_KEYBIT, BTN_MIDDLE);

    ioctl(fd, UI_SET_EVBIT, EV_REL);
    ioctl(fd, UI_SET_RELBIT, REL_X);
    ioctl(fd, UI_SET_RELBIT, REL_Y);
    ioctl(fd, UI_SET_RELBIT, REL_WHEEL);
    ioctl(fd, UI_SET_RELBIT, REL_HWHEEL);

    ioctl(fd, UI_SET_EVBIT, EV_SYN);

    struct uinput_setup usetup = {0};
    strcpy(usetup.name, "MCMouse");
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    usetup.id.version = 1;

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
}

int main() {
    int fd = open(MOUSE_DEVICE, O_RDONLY);
    if (fd < 0) {
        perror("Opening mouse device");
        return 1;
    }

    if (ioctl(fd, EVIOCGRAB, 1) == -1) {
        perror("Couldn't grab device");
        return 2;
    }

    int ufd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (ufd < 0) {
        perror("Opening /dev/uinput");
        return 3;
    }

    setup_uinput(ufd);

    struct input_event e;
    while (read(fd, &e, sizeof(e)) > 0) {
        if (e.type == EV_KEY) {
            if (e.code == BTN_EXTRA) e.code = KEY_8;
            else if (e.code == BTN_SIDE) e.code = KEY_9;
        }
        write(ufd, &e, sizeof(e));
    }

    ioctl(fd, EVIOCGRAB, 0);
    ioctl(ufd, UI_DEV_DESTROY);
    close(fd);
    close(ufd);
    return 0;
}