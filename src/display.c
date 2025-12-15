// display.c
#include "display.h"

#include <conio.h>
#include <stdio.h>
#include <windows.h>

static int HEIGHT;
static int WIDTH;

static void update_console_size(void) {
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);

    HEIGHT = info.srWindow.Bottom - info.srWindow.Top + 1;
    WIDTH = info.srWindow.Right - info.srWindow.Left + 1;
}

void display_init(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;

    GetConsoleMode(hOut, &mode);
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, mode);

    SetConsoleOutputCP(CP_UTF8);

    printf(HIDE_CURSOR);
}

void display_cleanup(void) {
    printf(SHOW_CURSOR);
}

// -

static int utf8_strlen(const char str[]) {
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

        int len = utf8_strlen(line->text);
        if (line->type == TEXT) len = len - 2 + line->data.options.maxLen;

        if (len > maxWidth) maxWidth = len;
        lineCount++;
    }

    int footerLen = utf8_strlen(box->footer);
    if (footerLen > maxWidth) maxWidth = footerLen;
    if (footerLen) lineCount++;

    *width = maxWidth;
    *height = lineCount;
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
    for (int row = 0; row < boxHeight - 2 - 1; row++) {
        for (int i = 0; i < sidePadding; i++) putchar(' ');
        printf("│ ");
        printf("%s" RESET, box->content[row]);
        for (int i = 0; i < boxWidth - 4 - utf8_strlen(box->content[row]); i++) putchar(' ');
        printf(" │\n");
    }

    // footer
    int footerPadding = boxWidth - 4 - utf8_strlen(box->footer);
    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("│ ");
    for (int i = 0; i < footerPadding / 2; i++) putchar(' ');
    printf("%s" RESET, box->footer);
    for (int i = 0; i < footerPadding - footerPadding / 2; i++) putchar(' ');
    printf(" │\n");

    for (int i = 0; i < sidePadding; i++) putchar(' ');
    printf("└");
    for (int i = 0; i < boxWidth - 2; i++) printf("─");
    printf("┘\n");
}

PromptInputs display_box_prompt(BoxContent *box) {
    int boxWidth, boxHeight;
    get_box_dimensions(box, &boxWidth, &boxHeight);

    DrawnBox shellBox = {
        .width = boxWidth,
        .height = boxHeight};
    strcpy(shellBox.title, box->title);
    strcpy(shellBox.footer, box->footer);

    int textInputCount = 0;
    int selectableCount = 0;
    for (int i = 0; i < LINE_COUNT; i++) {
        Line *line = &box->content[i];
        if (line->text[0] == '\0') break;
        strcpy(shellBox.content[i], line->text);
        if (line->type == TEXT) textInputCount++;
        if (line->type != DEFAULT) selectableCount++;
    }

    Line **selectableLines = malloc(selectableCount * sizeof(Line *));
    int *lineIndexMap = malloc(selectableCount * sizeof(int *));
    int *textIndexMap = malloc(selectableCount * sizeof(int *));
    int n = 0, t = 0;
    for (int i = 0; i < LINE_COUNT; i++) {
        Line *line = &box->content[i];
        if (line->text[0] == '\0') break;
        if (line->type == TEXT) textIndexMap[n] = t, t++;
        if (line->type != DEFAULT) {
            selectableLines[n] = line;
            lineIndexMap[n] = i;
            n++;
        }
    }

    char **textInputs = malloc(textInputCount * sizeof(char *));
    int dialogueValue;
    int currSelected = 0;
    int ch;

    // temp
    if ((selectableLines[0])->type == TEXT) {
        memmove(shellBox.content[lineIndexMap[0]] + strlen(FLIP), shellBox.content[lineIndexMap[0]], strlen(shellBox.content[lineIndexMap[0]]) + 1);
        memcpy(shellBox.content[lineIndexMap[0]], FLIP, strlen(FLIP));
    } else {
        memmove(shellBox.content[lineIndexMap[0]] + strlen(FLIP), shellBox.content[lineIndexMap[0]], strlen(shellBox.content[lineIndexMap[0]]) + 1);
        memcpy(shellBox.content[lineIndexMap[0]], FLIP, strlen(FLIP));
        dialogueValue = (selectableLines[0])->data.value;
    }

    while (1) {
        ch = _getch();

        update_console_size();
        display_draw_box(&shellBox);

        // escape
        if (ch == K_ESC) break;

        if (ch == 0 || ch == 224) {
            int s = _getch();
            if (s == K_UP || s == K_DOWN) {
                // temp
                memmove(shellBox.content[lineIndexMap[currSelected]], shellBox.content[lineIndexMap[currSelected]] + strlen(FLIP), strlen(shellBox.content[lineIndexMap[currSelected]] + strlen(FLIP)) + 1);

                if (s == K_UP) {
                    currSelected = (currSelected - 1 + selectableCount) % selectableCount;
                } else if (s == K_DOWN) {
                    currSelected = (currSelected + 1 + selectableCount) % selectableCount;
                }

                memmove(shellBox.content[lineIndexMap[currSelected]] + strlen(FLIP), shellBox.content[lineIndexMap[currSelected]], strlen(shellBox.content[lineIndexMap[currSelected]]) + 1);
                memcpy(shellBox.content[lineIndexMap[currSelected]], FLIP, strlen(FLIP));
                printf("currSelected: %d\n", currSelected);

                update_console_size();
                display_draw_box(&shellBox);
            }
        } else if (ch == K_ENTER) {
            printf("Enter key\n");
        } else {
            printf("Key pressed: %c (code %d)\n", ch, ch);
        }
    }

    return (PromptInputs){textInputCount, textInputs, dialogueValue};
}
