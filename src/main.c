// main.c
#include "display.h"
#include "menus.h"

int main() {
    display_init();
    mainloop();
    display_cleanup();

    return 0;
}
