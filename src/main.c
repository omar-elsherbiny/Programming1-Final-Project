#include <stdio.h>
#include <conio.h>
#include <windows.h>

// --- Terminal Control ---
#define CLEAR "\033[H\033[J"
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
#define BG_RESET "\033[49m" // Reset to default background color

// --- Style/Attributes ---
#define BOLD "\033[1m"
#define RESET "\033[0m" // Reset all attributes (color, bold, etc.)

int HEIGHT, WIDTH;
/*
typedef struct {
    
} BoxContent;*/


void printBox() {// takes in BoxContent
    printf(CLEAR);
    
    int boxWidth=20, boxHeight=20; // = <max width, max height>(box content)
    
    for (int i=0;i<(HEIGHT-boxHeight)/2-1;i++) putchar('\n');
    
    for (int i=0;i<(WIDTH-boxWidth)/2-1;i++) putchar(' ');
    putchar('+');
    for (int i=0;i<boxWidth-2;i++) putchar('-');
    putchar('+');
    for (int i=0;i<(WIDTH-boxWidth)/2-1;i++) putchar(' ');
    putchar('\n');
    
    for (int i=0;i<boxHeight;i++) {
         for (int i=0;i<(WIDTH-boxWidth)/2-1;i++) putchar(' ');
        putchar('|');
        printf(BG_WHITE);
        for (int i=0;i<boxWidth-2;i++) putchar(' ');
        printf(RESET);
        putchar('|');
        for (int i=0;i<(WIDTH-boxWidth)/2-1;i++) putchar(' ');
        putchar('\n');
    }
    
    for (int i=0;i<(WIDTH-boxWidth)/2-1;i++) putchar(' ');
    putchar('+');
    for (int i=0;i<boxWidth-2;i++) putchar('-');
    putchar('+');
    for (int i=0;i<(WIDTH-boxWidth)/2-1;i++) putchar(' ');
    
    for (int i=0;i<(HEIGHT-boxHeight)/2-1;i++) putchar('\n');
}


int main() {
    
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
    HEIGHT = info.srWindow.Bottom - info.srWindow.Top + 1;
    WIDTH = info.srWindow.Right - info.srWindow.Left + 1;
    
    //printBox();

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
    

    return 0;
}