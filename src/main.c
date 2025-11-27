#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <poll.h>

#include <libevdev/libevdev.h>
#include <xkbcommon/xkbcommon.h>
#include "list.h"

#define DEVICE "/dev/input/event0"
#define KEYBOARD_LAYOUT "fr"
#define KEYBOARD_MODEL "pc105"

static int fd;
static struct libevdev *dev = NULL;
static size_t keyword_len = 0;
static const char *keyword = NULL;

static inline void lock_screen() {
    system("loginctl list-sessions --no-legend | awk '{print $1}' | xargs -r -n1 loginctl lock-session");
}

static void check_pattern() {
    if (head == NULL || list_size < (int)keyword_len) return;

    Node *start_node = head;

    while (start_node != NULL) {
        Node *curr = start_node;
        size_t i = 0;

        while (curr != NULL && i < keyword_len) {
            if (curr->data != keyword[i]) {
                break;
            }
            curr = curr->next;
            i++;
        }

        if (i == keyword_len) {
            lock_screen();
            clear_list();
            return;
        }

        start_node = start_node->next;
    }
}

static void process_event(struct input_event *ev, struct xkb_state *state) {
    if (ev->type == EV_KEY && (ev->value == 1 || ev->value == 2)) {
        
        xkb_state_update_key(state, ev->code + 8, XKB_KEY_DOWN);
        xkb_keysym_t sym = xkb_state_key_get_one_sym(state, ev->code + 8);

        switch (sym) {
            case XKB_KEY_BackSpace:
                delete_char();
                check_pattern();
                break;
            
            case XKB_KEY_Delete:
                delete_char_forward();
                check_pattern();
                break;

            case XKB_KEY_Left:
                if (cursor != NULL) {
                    cursor = cursor->prev;
                }
                break;

            case XKB_KEY_Right:
                if (cursor == NULL && head != NULL) {
                    cursor = head;
                } else if (cursor != NULL && cursor->next != NULL) {
                    cursor = cursor->next;
                }
                break;

            case XKB_KEY_Shift_L:
            case XKB_KEY_Shift_R:
            case XKB_KEY_Caps_Lock:
            case XKB_KEY_Control_L:
            case XKB_KEY_Control_R:
            case XKB_KEY_Alt_L:
            case XKB_KEY_Alt_R:
            case 0:
                break;

            default:;
                char key_str[8]; 
                xkb_keysym_to_utf8(sym, key_str, sizeof(key_str));

                if (key_str[0] == '\0') break;

                char chr = tolower(key_str[0]);

                if (isprint(chr)) {
                    insert_char(chr);
                    check_pattern();
                }
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    const char *device_path = DEVICE;
    keyword = (argc > 1) ? argv[1] : "choco";
    keyword_len = strlen(keyword);

    if (keyword_len == 0) return 1;

    fd = open(device_path, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("ERR_OPEN");
        return 1;
    }

    if (libevdev_new_from_fd(fd, &dev) < 0) {
        close(fd);
        return 1;
    }

    struct xkb_context *ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    static const struct xkb_rule_names names = {
        .rules = NULL, .model = KEYBOARD_MODEL, .layout = KEYBOARD_LAYOUT, .variant = NULL, .options = NULL
    };
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(ctx, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
    struct xkb_state *state = xkb_state_new(keymap);

    if (!state) return 1;

    struct pollfd fds[1];
    fds[0].fd = fd;
    fds[0].events = POLLIN;

    struct input_event ev;

    while (1) {
        if (poll(fds, 1, -1) > 0) {
            while (libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev) == LIBEVDEV_READ_STATUS_SUCCESS) {
                process_event(&ev, state);
            }
        }
    }

    libevdev_free(dev);
    close(fd);
    return 0;
}