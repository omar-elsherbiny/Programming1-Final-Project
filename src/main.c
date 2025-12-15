// main.c
#include <stdio.h>

#include "display.h"
#include "functions.h"
#include "menus.h"

int main(void) {
    printf(FG_CYAN BG_WHITE "HelloWorld!\n" RESET);

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
            LINE_DEFAULT("Account Number: "),
            LINE_DEFAULT("┌────────────────────────────┐"),
            LINE_TEXT("│ %s │ ", 20, 0),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DIALOGUE("Find", 0),
            LINE_DIALOGUE("Discard", 1)}};

    DrawnBox boxC = {
        .width = 30,
        .height = 9,
        .title = FG_BLUE "Login",
        .content = {
            "┌ Username ──────────────────┐",
            "│ username                   │",
            "└────────────────────────────┘",
            "┌ Password ──────────────────┐",
            "│ ***█                       │",
            "└────────────────────────────┘",
            " ",
            FG_BLUE "Enter",
            FG_RED "Quit",
        }};

    // display_draw_box(boxC);
    display_box_prompt(&boxB);

    display_cleanup();
    return 0;
}
