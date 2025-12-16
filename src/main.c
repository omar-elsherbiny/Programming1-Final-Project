// main.c
#include <stdio.h>

#include "display.h"
#include "functions.h"
#include "menus.h"

int main(void) {
    printf(FG_CYAN BG_WHITE "HelloWorld!\n" RESET);

    display_init();

    BoxContent searchPage = {
        .title = FG_RED "Delete Account",
        .content = {
            LINE_DEFAULT("┌ "FG_RED "Delete Option" FG_RESET " ─────────────┐"),
            LINE_DIALOGUE("│ " FG_RED "%sOne with an Account number%s" FG_RESET " │", 10),
            LINE_DIALOGUE("│ " FG_RED "%sMultiple with a criteria%s" FG_RESET"   │", 11),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Discard", 0)}};

    display_box_prompt(&searchPage);

    display_cleanup();
    return 0;

}
