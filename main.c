#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libevdev/libevdev.h>
#include <fcntl.h>
#include <ctype.h>
#include <xkbcommon/xkbcommon.h>

int fd, rc;
struct libevdev *dev = NULL;
struct input_event ev;


void print_buf(char *buf, unsigned int head){
    for(int i = 0; i < 64; i++){
        char c = buf[(head + i) & 63];
        if(c != '\0'){
            printf("%c", c);
        }
    }
    printf("\n");
}

void lock_screen(){
    system("loginctl lock-session");
}

xkb_keysym_t get_next_key(struct xkb_state *state){
    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_BLOCKING, &ev);
    xkb_keysym_t sym = 0;

    if(rc == 0 && ev.type == EV_KEY && (ev.value == 1 || ev.value == 2)){
        xkb_state_update_key(state, ev.code + 8, XKB_KEY_DOWN);
        sym = xkb_state_key_get_one_sym(state, ev.code + 8);
    }
    return sym;
}

int main(int argc, char *argv[]){
    const char *device = "/dev/input/event0";

    fd = open(device, O_RDONLY|O_NONBLOCK);
    if(fd < 0) return 1;

    if(libevdev_new_from_fd(fd, &dev) < 0) return 1;

    struct xkb_context *ctx = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
    struct xkb_rule_names names = {
        .rules = NULL,
        .model = "pc105",
        .layout = "fr",
        .variant = NULL,
        .options = NULL
    };
    struct xkb_keymap *keymap = xkb_keymap_new_from_names(ctx, &names, XKB_KEYMAP_COMPILE_NO_FLAGS);
    struct xkb_state *state = xkb_state_new(keymap);

    char buf[64] = {0};
    unsigned int head = 0;
    unsigned int cursor = 0;

    while(1){

        xkb_keysym_t sym = get_next_key(state);
        
        switch (sym){
        case 0:
            break;

        case XKB_KEY_BackSpace:
            buf[--cursor & 63] = '\0';
            break;

        case XKB_KEY_Left:
            cursor--;
            break;

        case XKB_KEY_Right:
            if(cursor<head)cursor++;
            break;
        
        default:
            char key_str[7];
            char chr;

            xkb_keysym_to_utf8(sym, key_str, sizeof(key_str));
            chr = tolower(key_str[0]);

            buf[cursor & 63] = chr;
            head++;
            cursor++;

            unsigned int p = head - 1;
            for(int i = 0; i < 10; i++){
                unsigned int s = p - 4 + i;
                if(buf[(s) & 63] == 'c' &&
                    buf[(s+1) & 63] == 'h' &&
                    buf[(s+2) & 63] == 'o' &&
                    buf[(s+3) & 63] == 'c' &&
                    buf[(s+4) & 63] == 'o'){
                    lock_screen();
                    head = 0;
                    memset(buf, 0, 64);
                    break;
                }
            }
            break;
        }

    }
    return 0;
}