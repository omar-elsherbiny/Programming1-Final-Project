// display.c
#include "display.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>

static int HEIGHT;
static int WIDTH;

static int utf8_strlen(char str[]) {
    int len = 0;
    while (*str != '\0') {
        unsigned char c = (unsigned char)*str;
        // skip ANSI
        if (c == 0x1B && str[1] == '[') {
            str += 2;  // skip ESC[
            while (*str && (*str < '@' || *str > '~')) {
                str++;
            }
            if (*str) str++;  // final command character
            continue;
        }
        // count multi-chars once
        if ((c & 0xC0) != 0x80) len++;
        str++;
    }
    return len;
}

static void get_box_dimensions(BoxContent *box, int *width, int *height) {
    int maxWidth = 0;
    int lineCount = 0;

    int titleLen = utf8_strlen(box->title);
    if (titleLen > maxWidth) maxWidth = titleLen;

    for (int i = 0; i < LINE_COUNT; i++) {
        Line *line = &box->content[i];
        if (line->text[0] == '\0') break;

        int len;
        // TODO: check length per LineType
        len = utf8_strlen(line->text);

        if (len > maxWidth) maxWidth = len;
        lineCount++;
    }

    int footerLen = utf8_strlen(box->footer);
    if (footerLen > maxWidth) maxWidth = footerLen;
    if (footerLen) lineCount++;

    *width = maxWidth;
    *height = lineCount;
}

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

    printf(HIDE_CURSOR);
}

void display_draw_box(DrawnBox *box) {
    printf(CLEAR);

    int boxWidth = (box->width) + 4, boxHeight = (box->height) + 2;
    if (boxWidth > WIDTH) boxWidth = WIDTH;
    if (boxHeight > HEIGHT) boxHeight = HEIGHT;

    int topPadding = (HEIGHT - boxHeight) / 2;
    int sidePadding = (WIDTH - boxWidth + 1) / 2;

    // top padding
    for (int i = 0; i < topPadding; i++) putchar('\n');

    // top border + title
    int titlePadding = boxWidth - 2 - utf8_strlen(box->title);
    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("┌");
    for (int i = 0; i < titlePadding / 2 - 1; i++) printf("─");
    printf(" %s " RESET, box->title);
    for (int i = 0; i < titlePadding - titlePadding / 2 - 1; i++) printf("─");
    printf("┐\n");

    // middle - footer
    _Bool isFooter = (box->footer)[0] != '\0';
    for (int row = 0; row < boxHeight - 2 - isFooter; row++) {
        for (int i = 0; i < sidePadding; i++) putchar(' ');
        printf("│ ");
        printf("%s" RESET, box->content[row]);
        for (int i = 0; i < boxWidth - 4 - utf8_strlen(box->content[row]); i++) putchar(' ');
        printf(" │\n");
    }

    // footer
    if (isFooter) {
        int footerPadding = boxWidth - 4 - utf8_strlen(box->footer);
        for (int i = 0; i < sidePadding; i++) putchar(' ');
        printf("│ ");
        for (int i = 0; i < footerPadding / 2; i++) putchar(' ');
        printf("%s" RESET, box->footer);
        for (int i = 0; i < footerPadding - footerPadding / 2; i++) putchar(' ');
        printf(" │\n");
    }

    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("└");
    for (int i = 0; i < boxWidth - 2; i++) printf("─");
    printf("┘\n");
}

void display_cleanup(void) {
    printf(SHOW_CURSOR);
}