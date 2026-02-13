#include <fcntl.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <unistd.h>

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

    ioctl(fd, UI_DEV_SETUP, &(struct uinput_setup){
        .name = "MCMouse",
        .id = {
            .bustype = BUS_USB,
            .vendor = 0x1234,
            .product = 0x5678,
            .version = 1
        }
    });
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
        struct input_event syn = { .type = EV_SYN, .code = SYN_REPORT, .value = 0 };
        write(ufd, &syn, sizeof(syn));
    }

    ioctl(fd, EVIOCGRAB, 0);
    ioctl(ufd, UI_DEV_DESTROY);
    close(fd);
    close(ufd);
    return 0;
}