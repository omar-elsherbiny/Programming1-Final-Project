#ifndef DISPLAY_H
#define DISPLAY_H

// Terminal
#define CLEAR "\033[H\033[2J\033[3J"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

// Foreground
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"
#define FG_RESET "\033[39m"

// Background
#define BG_BLACK "\033[40m"
#define BG_WHITE "\033[47m"
#define BG_RESET "\033[49m"

#define BOLD "\033[1m"
#define RESET "\033[0m"

void display_init(void);
void display_print_box(int width, int height);
void display_cleanup(void);

#endif