#include <conio.h>
#include <stdio.h>
#include <windows.h>

// --- Terminal Control ---
#define CLEAR "\033[H\033[2J\033[3J"
#define HIDE_CURSOR "\033[?25l"
#define SHOW_CURSOR "\033[?25h"

// --- Foreground Colors ---
#define FG_BLACK "\033[30m"
#define FG_RED "\033[31m"
#define FG_GREEN "\033[32m"
#define FG_YELLOW "\033[33m"
#define FG_BLUE "\033[34m"
#define FG_MAGENTA "\033[35m"
#define FG_CYAN "\033[36m"
#define FG_WHITE "\033[37m"
#define FG_RESET "\033[39m"

// --- Background Colors ---
#define BG_BLACK "\033[40m"
#define BG_WHITE "\033[47m"
#define BG_RESET "\033[49m"

// --- Style/Attributes ---
#define BOLD "\033[1m"
#define RESET "\033[0m"  // Reset all attributes (color, bold, etc.)

int HEIGHT, WIDTH;

/*
typedef struct {

} BoxContent;*/

void printBox(int w, int h) {  // takes in BoxContent
    printf(CLEAR);

    int boxWidth = w, boxHeight = h;

    if (boxHeight > HEIGHT) boxHeight = HEIGHT - 1;
    if (boxWidth > WIDTH) boxWidth = WIDTH - 1;
    int topPadding = (HEIGHT - boxHeight) / 2;
    int sidePadding = (WIDTH - boxWidth + 1) / 2;

    // top padding
    for (int i = 0; i < topPadding; i++) putchar('\n');

    // top border
    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("┌");
    for (int i = 0; i < boxWidth - 2; i++) printf("─");
    printf("┐");
    putchar('\n');

    // middle
    for (int row = 0; row < boxHeight - 2; row++) {
        for (int i = 0; i < sidePadding; i++) putchar(' ');
        printf("│");

        printf(BG_WHITE);
        for (int col = 0; col < boxWidth - 2; col++) putchar(' ');
        printf(RESET);

        printf("│");
        putchar('\n');
    }

    // bottom border
    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("└");
    for (int i = 0; i < boxWidth - 2; i++) printf("─");
    printf("┘");
    putchar('\n');

    for (int i = 0; i < HEIGHT - topPadding - boxHeight - 1; i++) putchar('\n');
}

int main() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    SetConsoleOutputCP(CP_UTF8);

    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    HEIGHT = info.srWindow.Bottom - info.srWindow.Top + 1;
    WIDTH = info.srWindow.Right - info.srWindow.Left + 1;

    // printf("\033[5m \033[41m Hello \033[34mWorld! \033[7m Im \033[33mSelected \033[0m Normal text");

    printf(HIDE_CURSOR);

    int w, h;
    scanf("%d%d", &w, &h);
    printBox(w, h);
    /*
        while (getc(stdin) != 'n') {
            printf("\033[H\033[J");

            int row, col, n;

            for (row = 0; row < 11; row++) {
                for (col = 0; col < 10; col++) {
                    n = 10 * row + col;
                    if (n > 109)
                        break;
                    printf("\033[%dm %3d\033[m", n, n);
                }
                printf("\n");
            }
        }
        */

    getc(stdin);
    getc(stdin);

    return 0;
}