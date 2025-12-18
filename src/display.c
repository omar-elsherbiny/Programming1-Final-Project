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

Line LINE_DEFAULT(const char text[]) {
    Line res = {
        .type = DEFAULT};
    strcpy(res.text, text);
    return res;
}

Line LINE_DIALOGUE(const char text[], int value) {
    Line res = {
        .type = DIALOGUE,
        .data.value = value};
    strcpy(res.text, text);
    return res;
}

Line LINE_TEXT(const char text[], int maxLen, _Bool hidden, const char validChars[]) {
    Line res = {
        .type = TEXT,
        .data.options = {.maxLen = maxLen, .hidden = hidden}};
    strcpy(res.text, text);
    strcpy(res.data.options.validChars, validChars);
    return res;
}

Line *MULTI_LINE_DEFAULT(const char text[], int width, int *lineCnt) {
    Line *multiLines = malloc(LINE_COUNT * sizeof(Line));
    int curLen = 0;
    int lines = 0;

    char textCpy[LINE_LENGTH];
    strncpy(textCpy, text, LINE_LENGTH - 1);
    textCpy[LINE_LENGTH - 1] = '\0';

    char *word = strtok(textCpy, " ");
    multiLines[lines].text[0] = '\0';
    multiLines[lines].type = DEFAULT;

    while (word) {
        int wlen = strlen(word);
        if (curLen + (curLen > 0) + wlen > width) {
            lines++;
            curLen = 0;
            multiLines[lines].text[0] = '\0';
            multiLines[lines].type = DEFAULT;
        }

        if (curLen > 0) {
            strcat(multiLines[lines].text, " ");
            curLen++;
        }

        strcat(multiLines[lines].text, word);
        curLen += wlen;

        word = strtok(NULL, " ");
    }

    if (curLen == 0 && lines > 0) lines--;

    *lineCnt = lines + 1;
    return multiLines;
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

static int utf8_strcnt(const char str[], int char_cnt, _Bool extend) {
    int bytes = 0;
    const unsigned char *p = (const unsigned char *)str;

    while (*p && char_cnt > 0) {
        // skip ANSI
        if (*p == 0x1B && p[1] == '[') {
            p += 2;
            bytes += 2;
            while (*p && (*p < '@' || *p > '~')) {
                p++;
                bytes++;
            }
            if (*p) {
                p++;
                bytes++;
            }
            continue;
        }

        int len;

        if ((*p & 0x80) == 0x00)
            len = 1;  // ASCII
        else if ((*p & 0xE0) == 0xC0)
            len = 2;
        else if ((*p & 0xF0) == 0xE0)
            len = 3;
        else if ((*p & 0xF8) == 0xF0)
            len = 4;
        else
            break;  // invalid UTF-8

        p += len;
        bytes += len;
        char_cnt--;
    }

    if (extend && *p) {
        if (*p == 0x1B && p[1] == '[') {
            p += 2;
            bytes += 2;
            while (*p && (*p < '@' || *p > '~')) {
                p++;
                bytes++;
            }
            if (*p) {
                p++;
                bytes++;
            }
        }
    }

    return bytes;
}

static int calc_max_offset(const char template[], const char str[], int width) {
    int contentWidthC = width - (utf8_strlen(template) - 2);
    int strLenC = utf8_strlen(str);
    return strLenC - contentWidthC;
}

void format_string(const char template[], const char str[], int width, int offset, char dest[]) {
    const char *placeholder = strstr(template, "%s");
    if (!placeholder) return;

    int prefixLenB = placeholder - template;
    int suffixLenB = strlen(template) - prefixLenB - 2;

    int contentWidthB = 0;
    int contentWidthC = width - (utf8_strlen(template) - 2);
    if (contentWidthC < 0) return;

    char result[LINE_LENGTH];

    // put prefix
    memcpy(result, template, prefixLenB);

    // put formatted content
    char *content = result + prefixLenB;
    int strLenC = utf8_strlen(str);
    int strLenB = strlen(str);
    if (strLenC <= contentWidthC) {
        // pad with spaces
        memcpy(content, str, strLenB);
        memset(content + strLenB, ' ', contentWidthC - strLenC);
        contentWidthB += strLenB + contentWidthC - strLenC;
    } else {
        // truncate and offset
        if (offset > strLenC - contentWidthC) offset = strLenC - contentWidthC;
        int offsetB = utf8_strcnt(str, offset, 0);
        int leading = 0;
        contentWidthB = utf8_strcnt(str + offsetB, contentWidthC, 1);
        memcpy(content, str + offsetB, contentWidthB);
        // leading elipses
        if (offset > 0) {
            leading = 1;
            contentWidthB = utf8_strcnt(str + offsetB + 1, contentWidthC - 1, 1);
            memcpy(content, "…", 3);
            memcpy(content + 3, str + offsetB + 1, contentWidthB);
            contentWidthB += 3;
        }
        // trailing elipses
        if (offset < strLenC - contentWidthC) {
            contentWidthB -= (utf8_strcnt(str + offsetB + leading, contentWidthC - leading, 1) -
                              utf8_strcnt(str + offsetB + leading, contentWidthC - leading - 1, 1));
            // subtract last char bytes
            memcpy(content + contentWidthB, "…", 3);
            contentWidthB += 3;
        }
    }

    // put suffix
    memcpy(result + prefixLenB + contentWidthB, placeholder + 2, suffixLenB);

    result[prefixLenB + contentWidthB + suffixLenB] = '\0';
    strcpy(dest, result);
}

void replace_wrap_string(const char str[], const char first[], const char second[], char dest[]) {
    int count = 0;
    const char *p = str;
    while ((p = strstr(p, "%s")) != NULL) count++, p += 2;

    if (count == 2)
        sprintf(dest, str, first, second);
    else
        sprintf(dest, "%s%s%s", first, str, second);
}

// -

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
        if (line->type == DIALOGUE) {
            int count = 0;
            char *p = line->text;
            while ((p = strstr(p, "%s")) != NULL) count++, p += 2;
            len = len - 2 * count;
        }

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
    setvbuf(stdout, NULL, _IOFBF, 0);
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

    fflush(stdout);
}

PromptInputs display_box_prompt(BoxContent *box) {
    int boxWidth, boxHeight;
    get_box_dimensions(box, &boxWidth, &boxHeight);

    DrawnBox resultBox = {
        .width = boxWidth,
        .height = boxHeight};
    strcpy(resultBox.title, box->title);
    strcpy(resultBox.footer, box->footer);

    // count
    int textInputCount = 0;
    int selectableCount = 0;
    for (int i = 0; i < LINE_COUNT; i++) {
        Line *line = &box->content[i];
        if (line->text[0] == '\0') break;
        strcpy(resultBox.content[i], line->text);
        if (line->type == TEXT) {
            format_string(line->text, "", boxWidth, 0, resultBox.content[i]);
            textInputCount++;
        }
        if (line->type == DIALOGUE) replace_wrap_string(line->text, "", "", resultBox.content[i]);
        if (line->type != DEFAULT) selectableCount++;
    }

    // handle no selectables
    int ch;
    if (selectableCount == 0) {
        update_console_size();  // recenters
        display_draw_box(&resultBox);
        while (1) {
            ch = _getch();
            if (ch == K_ESC)
                exit(1);
            else if (ch == K_ENTER)
                return (PromptInputs){0};
        }
    }

    // initialize arrays
    Line **selectableLines = malloc(selectableCount * sizeof(Line *));
    int *resultIndexMap = malloc(selectableCount * sizeof(int));
    int *textIndexMap = malloc(selectableCount * sizeof(int));
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

    int *textOffsets = calloc(textInputCount, sizeof(int));
    char **textInputs = malloc(textInputCount * sizeof(char *));
    for (int i = 0; i < textInputCount; i++) {
        textInputs[i] = calloc(LINE_LENGTH + 1, sizeof(char));
    }

    int curr = 0;
    int prev = -1;

    int dialogueValue;
    char *textInput;
    char tempTextInput[LINE_LENGTH];

    Line *currLine = selectableLines[curr];
    char *currResLine = resultBox.content[resultIndexMap[curr]];

    if (currLine->type == TEXT) {
        format_string(currLine->text, BLINK "█" BLINK_RESET, boxWidth, 0, currResLine);
        textInput = textInputs[textIndexMap[curr]];
    }  // add initial FLIP TEXT
    else {
        replace_wrap_string(currLine->text, FLIP, FLIP_RESET, currResLine);
        dialogueValue = currLine->data.value;
    }  // add initial FLIP DIALOGUE

    while (1) {
        update_console_size();  // recenters
        display_draw_box(&resultBox);

        ch = _getch();

        if (ch == K_ESC) {
            exit(1);  // TODO: remove before prod
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
                    if (selectableLines[prev]->data.options.hidden) {
                        int len = utf8_strlen(textInputs[textIndexMap[prev]]);
                        memset(tempTextInput, '*', len);
                        tempTextInput[len] = '\0';
                    } else {
                        strcpy(tempTextInput, textInputs[textIndexMap[prev]]);
                    }  // format string buffer for text field
                    format_string(
                        selectableLines[prev]->text,
                        tempTextInput,
                        boxWidth,
                        0,
                        resultBox.content[resultIndexMap[prev]]);
                }  // remove FLIP TEXT
                else {
                    replace_wrap_string(selectableLines[prev]->text, "", "", resultBox.content[resultIndexMap[prev]]);
                }  // remove FLIP DIALOGUE

                if (currLine->type == TEXT) {
                    textInput = textInputs[textIndexMap[curr]];
                    textOffsets[textIndexMap[curr]] = LINE_LENGTH;
                    if (currLine->data.options.hidden) {
                        int len = utf8_strlen(textInput);
                        memset(tempTextInput, '*', len);
                        tempTextInput[len] = '\0';
                        strcat(tempTextInput, BLINK "█" BLINK_RESET);
                    } else {
                        sprintf(tempTextInput, "%s" BLINK "█" BLINK_RESET, textInput);
                    }  // format string buffer for text field
                    format_string(currLine->text, tempTextInput, boxWidth, LINE_LENGTH, currResLine);
                }  // add FLIP TEXT
                else {
                    replace_wrap_string(currLine->text, FLIP, FLIP_RESET, currResLine);
                    dialogueValue = currLine->data.value;
                }  // add FLIP DIALOGUE
            } else if ((s == K_LEFT || s == K_RIGHT) && currLine->type == TEXT) {
                if (currLine->data.options.hidden) {
                    int len = utf8_strlen(textInput);
                    memset(tempTextInput, '*', len);
                    tempTextInput[len] = '\0';
                    strcat(tempTextInput, BLINK "█" BLINK_RESET);
                } else {
                    sprintf(tempTextInput, "%s" BLINK "█" BLINK_RESET, textInput);
                }  // format string buffer for text field
                int textOffset = textOffsets[textIndexMap[curr]];
                int maxTextOffset = calc_max_offset(currLine->text, tempTextInput, boxWidth);

                if (textOffset < 0) textOffset = 0;
                if (textOffset > maxTextOffset) textOffset = maxTextOffset;

                if (textOffset > 0 && s == K_LEFT) textOffset--;
                if (textOffset < maxTextOffset && s == K_RIGHT) textOffset++;

                textOffsets[textIndexMap[curr]] = textOffset;
                format_string(currLine->text, tempTextInput, boxWidth, textOffset, currResLine);
            }  // scroll TEXT
        } else if (ch == K_ENTER && currLine->type == DIALOGUE) {
            break;
        } else if (currLine->type == TEXT &&
                   (currLine->data.options.validChars[0] == '\0' ||
                    strchr(currLine->data.options.validChars, (char)ch))) {
            // write TEXT
            int len = utf8_strlen(textInput);
            int maxLen = currLine->data.options.maxLen;

            if (ch == K_BACKSPACE && len > 0) {
                textInput[--len] = '\0';
            } else if (ch >= 32 && ch <= 126 && len < maxLen) {
                textInput[len] = (char)ch;
                textInput[++len] = '\0';
            }

            textOffsets[textIndexMap[curr]] = LINE_LENGTH;
            if (currLine->data.options.hidden) {
                memset(tempTextInput, '*', len);
                tempTextInput[len] = '\0';
                strcat(tempTextInput, BLINK "█" BLINK_RESET);
            } else {
                sprintf(tempTextInput, "%s" BLINK "█" BLINK_RESET, textInput);
            }  // format string buffer for text field
            format_string(currLine->text, tempTextInput, boxWidth, LINE_LENGTH, currResLine);
        }
    }

    free(selectableLines);
    free(resultIndexMap);
    free(textIndexMap);
    free(textOffsets);

    return (PromptInputs){textInputCount, textInputs, dialogueValue};
}
