#include <stdio.h>

#include "display.h"

int main(void) {
    int w, h;

    printf(FG_CYAN "HelloWorld!" RESET);
    scanf("%d %d", &w, &h);

    display_init();

    display_print_box(w, h);

    getchar();
    getchar();

    display_cleanup();
    return 0;
}
