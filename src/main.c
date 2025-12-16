// main.c
#include <stdio.h>

#include "display.h"
#include "functions.h"
#include "menus.h"

int main(void) {
    display_init();

    BoxContent addAccountPage = {
        .title = "Add Account" ,
        .content = {
            LINE_DEFAULT("┌ Account Number ────────────┐"),
            LINE_TEXT("│ %s │", 10,    0),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Name ──────────────────────┐"),
            LINE_TEXT("│ %s │", 50,    0),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ E-mail ────────────────────┐"),
            LINE_TEXT("│ %s │", 25,    0),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Balance ───────────────────┐"),
            LINE_TEXT("│ %s ($) │", 15,    0),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT("┌ Mobile ────────────────────┐"),
            LINE_TEXT("│ + %s │", 15,    0),
            LINE_DEFAULT("└────────────────────────────┘"),

            LINE_DIALOGUE("Add", 0),
            LINE_DIALOGUE("Back", 1)}};

    BoxContent searchPage = {
        .title = FG_RED "Delete Account",
        .content = {
            LINE_DEFAULT("┌ "FG_RED "Delete Option" FG_RESET " ─────────────┐"),
            LINE_DIALOGUE("│ " FG_RED "%sOne with an Account number%s" FG_RESET " │", 10),
            LINE_DIALOGUE("│ " FG_RED "%sMultiple with a criteria%s" FG_RESET"   │", 11),
            LINE_DEFAULT("└────────────────────────────┘"),
            LINE_DEFAULT(" "),
            LINE_DIALOGUE("Discard", 0)}};

    //display_box_prompt(&searchPage);
    display_box_prompt(&addAccountPage);

    display_cleanup();
    return 0;
}
