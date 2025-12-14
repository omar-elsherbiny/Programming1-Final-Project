#include <stdio.h>

#include "display.h"

int main(void) {
    printf(FG_CYAN BG_WHITE "HelloWorld!" RESET);

    display_init();

    BoxContent boxA = {
        .title = "Confirm Add",
        .content = {
            LINE_DEFAULT(" Are you sure you want to add"),
            LINE_DEFAULT(" this account"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE(" Yes", 0),
            LINE_DIALOGUE(" No", 1)}};

    BoxContent boxB = {
        .title = "Search Account",
        .content = {
            LINE_DEFAULT(" Account Number: "),
            LINE_DEFAULT(" ┌────────────────────────────┐ "),
            LINE_TEXT(" │ -- │ ", 20, 0),
            LINE_DEFAULT(" └────────────────────────────┘ "),
            LINE_DIALOGUE(" Find", 0),
            LINE_DIALOGUE(" Discard", 1)}};

    display_print_box(&boxB);

    getchar();

    display_cleanup();
    return 0;
}
