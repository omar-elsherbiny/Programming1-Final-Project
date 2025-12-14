#include "display.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>

static int HEIGHT;
static int WIDTH;

void display_init(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);

    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    HEIGHT = info.srWindow.Bottom - info.srWindow.Top + 1;
    WIDTH = info.srWindow.Right - info.srWindow.Left + 1;

    printf("\n%d %d\n", WIDTH, HEIGHT);

    printf(HIDE_CURSOR);
}

void display_print_box(int w, int h) {
    printf(CLEAR);

    int boxWidth = (w > WIDTH) ? WIDTH - 1 : w;
    int boxHeight = (h > HEIGHT) ? HEIGHT - 1 : h;

    int topPadding = (HEIGHT - boxHeight) / 2;
    int sidePadding = (WIDTH - boxWidth + 1) / 2;

    for (int i = 0; i < topPadding; i++) putchar('\n');

    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("┌");
    for (int i = 0; i < boxWidth - 2; i++) printf("─");
    printf("┐\n");

    for (int row = 0; row < boxHeight - 2; row++) {
        for (int i = 0; i < sidePadding; i++) putchar(' ');
        printf("│");
        printf(BG_WHITE);
        for (int col = 0; col < boxWidth - 2; col++) putchar(' ');
        printf(RESET);
        printf("│\n");
    }

    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("└");
    for (int i = 0; i < boxWidth - 2; i++) printf("─");
    printf("┘\n");
}

void display_cleanup(void) {
    printf(SHOW_CURSOR);
}