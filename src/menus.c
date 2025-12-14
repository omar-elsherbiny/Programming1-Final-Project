// menus.c
#include "menus.h"

typedef enum {
    RETURN = -1,
    ENTRY = 0,
} MenuIndex;

static MenuIndex (*menuFunctions[100])();

// MenuIndex entry_func(params)
// menuFunctions[ENTRY] = entry_func

void mainloop() {
    MenuIndex currentIndex = ENTRY;
    while (currentIndex != RETURN) {
        currentIndex = menuFunctions[currentIndex]();
    }
}