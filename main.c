#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/time.h>

#define MOUSE_DEVICE "/dev/input/by-id/usb-Razer_Razer_DeathAdder_Elite-event-mouse"

int uinput_fd;

void emit(int fd, int type, int code, int val) {
    struct input_event ev;
    gettimeofday(&ev.time, NULL);
    ev.type = type;
    ev.code = code;
    ev.value = val;
    write(fd, &ev, sizeof(ev));
}

int setup_uinput_device() {
    int fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("Opening /dev/uinput");
        exit(4);
    }

    // Enable mouse and keyboard event types
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

    struct uinput_setup usetup;
    memset(&usetup, 0, sizeof(usetup));
    snprintf(usetup.name, UINPUT_MAX_NAME_SIZE, "VirtualMouseKeyboard");
    usetup.id.bustype = BUS_USB;
    usetup.id.vendor = 0x1234;
    usetup.id.product = 0x5678;
    usetup.id.version = 1;

    ioctl(fd, UI_DEV_SETUP, &usetup);
    ioctl(fd, UI_DEV_CREATE);
    sleep(1); // Give system time to register

    return fd;
}

void send_key(int fd, int keycode) {
    emit(fd, EV_KEY, keycode, 1);
    emit(fd, EV_SYN, SYN_REPORT, 0);
    emit(fd, EV_KEY, keycode, 0);
    emit(fd, EV_SYN, SYN_REPORT, 0);
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

    uinput_fd = setup_uinput_device();

    struct input_event ev;
    while (read(fd, &ev, sizeof(ev)) > 0) {
        if (ev.type == EV_KEY) {
            if (ev.code == BTN_EXTRA) {
                if (ev.value == 1) send_key(uinput_fd, KEY_8);
            } else if (ev.code == BTN_SIDE) {
                if (ev.value == 1) send_key(uinput_fd, KEY_9);
            } else {
                emit(uinput_fd, EV_KEY, ev.code, ev.value);
                emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
            }
        } else if (ev.type == EV_REL) {
            emit(uinput_fd, EV_REL, ev.code, ev.value);
            emit(uinput_fd, EV_SYN, SYN_REPORT, 0);
        }
    }

    // Cleanup
    ioctl(fd, EVIOCGRAB, 0);
    ioctl(uinput_fd, UI_DEV_DESTROY);
    close(fd);
    close(uinput_fd);
    return 0;
}
