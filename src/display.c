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
        if (line->type == TEXT) len = len - 2;  // + line->data.options.maxLen;

        if (len > maxWidth) maxWidth = len;
        lineCount++;
    }

    int footerLen = utf8_strlen(box->footer);
    if (footerLen > maxWidth) maxWidth = footerLen;
    if (footerLen) lineCount++;

    *width = maxWidth;
    *height = lineCount;
}

void format_string(char template[], char str[], int width, int offset, char dest[]) {
    const char *placeholder = strstr(template, "%s");
    if (!placeholder) return;

    int prefixLenB = placeholder - template;
    int suffixLenB = strlen(template) - prefixLenB - 2;

    int contentWidth = width - (utf8_strlen(template) - 2);
    if (contentWidth < 0) return;

    char result[LINE_LENGTH];

    // put prefix
    memcpy(result, template, prefixLenB);

    // put formatted content
    char *content = result + prefixLenB;
    int contentLenB = 0;
    int strLen = strlen(str);
    if (strLen <= contentWidth) {
        // pad with spaces
        memcpy(content, str, strLen);
        memset(content + strLen, ' ', contentWidth - strLen);
        contentLenB += contentWidth;
    } else {
        // truncate and offset
        if (offset > strLen - contentWidth) offset = strLen - contentWidth;
        memcpy(content, str + offset, strLen - offset);
        // leading elipses
        if (offset > 0) {
            memcpy(content, "…", 3);
            memcpy(content + 3, str + offset, contentWidth - 1);
            contentLenB += 2;
        }
        contentLenB += contentWidth;
        // trailing elipses
        if (offset < strLen - contentWidth) {
            memcpy(content + contentLenB - 1, "…", 3);
            contentLenB += 2;
        }
    }

    // put suffix
    memcpy(result + prefixLenB + contentLenB, placeholder + 2, suffixLenB);

    *(result + prefixLenB + contentLenB + suffixLenB + 1) = '\0';
    strcpy(dest, result);
}

void replace_wrap_string(char str[], char first[], char second[], char dest[]) {
    int count = 0;
    char *p = str;
    while ((p = strstr(p, "%s")) != NULL) count++, p += 2;

    if (count == 2)
        sprintf(dest, str, first, second);
    else
        sprintf(dest, "%s%s%s", first, str, second);
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

PromptInputs display_box_prompt(BoxContent *box) {
    int boxWidth, boxHeight;
    get_box_dimensions(box, &boxWidth, &boxHeight);

    DrawnBox resultBox = {
        .width = boxWidth,
        .height = boxHeight};
    strcpy(resultBox.title, box->title);
    strcpy(resultBox.footer, box->footer);

    int textInputCount = 0;
    int selectableCount = 0;
    for (int i = 0; i < LINE_COUNT; i++) {
        Line *line = &box->content[i];
        if (line->text[0] == '\0') break;
        strcpy(resultBox.content[i], line->text);
        if (line->type == TEXT) textInputCount++;
        if (line->type != DEFAULT) selectableCount++;
    }

    Line **selectableLines = malloc(selectableCount * sizeof(Line *));
    int *resultIndexMap = malloc(selectableCount * sizeof(int *));
    int *textIndexMap = malloc(selectableCount * sizeof(int *));
    int n = 0, t = 0;
    for (int i = 0; i < LINE_COUNT; i++) {
        Line *line = &box->content[i];
        if (line->text[0] == '\0') break;
        if (line->type == TEXT) textIndexMap[n] = t, t++;
        if (line->type != DEFAULT) {
            selectableLines[n] = line;
            resultIndexMap[n] = i;
            n++;
        }
    }

    int curr = 0;
    int prev = -1;
    int ch;
    char **textInputs = malloc(textInputCount * sizeof(char *));
    int dialogueValue;
    Line *currLine = selectableLines[curr];
    char *currResLine = resultBox.content[resultIndexMap[curr]];

    // temp
    /*if ((selectableLines[0])->type == TEXT) {
        memmove(+strlen(FLIP), shellBox.content[lineIndexMap[0]], strlen(shellBox.content[lineIndexMap[0]]) + 1);
        memcpy(shellBox.content[lineIndexMap[0]], FLIP, strlen(FLIP));
    } else {
        memmove(shellBox.content[lineIndexMap[0]] + strlen(FLIP), shellBox.content[lineIndexMap[0]], strlen(shellBox.content[lineIndexMap[0]]) + 1);
        memcpy(shellBox.content[lineIndexMap[0]], FLIP, strlen(FLIP));
        dialogueValue = (selectableLines[0])->data.value;
    }*/

    while (1) {
        update_console_size();  // recenters
        display_draw_box(&resultBox);

        ch = _getch();

        if (ch == K_ESC) {
            exit(1);
        } else if (ch == 0 || ch == 224) {
            int s = _getch();
            if (s == K_UP || s == K_DOWN) {
                prev = curr;
                if (s == K_UP) {
                    curr = (curr - 1 + selectableCount) % selectableCount;
                } else if (s == K_DOWN) {
                    curr = (curr + 1 + selectableCount) % selectableCount;
                }
                currLine = selectableLines[curr];
                currResLine = resultBox.content[resultIndexMap[curr]];

                if (selectableLines[prev]->type == TEXT) {
                }  // remove FLIP TEXT
                else {
                }  // remove FLIP DIALOGUE

                if (currLine->type == TEXT) {
                    format_string(currLine->text, "Hello", boxWidth, 0, currResLine);
                }  // add FLIP TEXT
                else {
                    replace_wrap_string(currLine->text, FLIP, FLIP_RESET, currResLine);
                    // dialogueValue = selectableLines[currSelected]->data.value;
                }  // add FLIP DIALOGUE
            } else if (s == K_LEFT || s == K_RIGHT) {
                // scroll TEXT
            }
        } else if (ch == K_ENTER && currLine->type == DIALOGUE) {
            break;
        } else if (currLine->type == TEXT) {
            // write TEXT
        }
    }

    // TODO: free stuff

    return (PromptInputs){textInputCount, textInputs, dialogueValue};
}
