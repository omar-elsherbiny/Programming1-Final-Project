// display.h
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
#define BG_RED "\033[41m"
#define BG_GREEN "\033[42m"
#define BG_YELLOW "\033[43m"
#define BG_BLUE "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN "\033[46m"
#define BG_WHITE "\033[47m"
#define BG_RESET "\033[49m"

#define FLIP "\033[7m"
#define FLIP_RESET "\033[27m"
#define BLINK "\033[5m"
#define BLINK_RESET "\033[25m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

#define K_ESC 27
#define K_ENTER 13
#define K_BACKSPACE 8
#define K_UP 72
#define K_DOWN 80
#define K_RIGHT 77
#define K_LEFT 75

typedef enum {
    DEFAULT,
    TEXT,
    DIALOGUE,
} LineType;

typedef struct {
    int maxLen;
    _Bool hidden;
    char validChars[256];
} TextOptions;

#define LINE_LENGTH 300
#define LINE_COUNT 100

typedef struct {
    char text[LINE_LENGTH];
    LineType type;
    union {
        TextOptions options;
        int value;
    } data;
} Line;

typedef struct {
    char title[LINE_LENGTH];
    Line content[LINE_COUNT];
    char footer[LINE_LENGTH];
} BoxContent;

typedef struct {
    int width, height;
    char title[LINE_LENGTH];
    char content[LINE_COUNT][LINE_LENGTH];
    char footer[LINE_LENGTH];
} DrawnBox;

typedef struct {
    int textInputCount;
    char **textInputs;
    int dialogueValue;
} PromptInputs;

Line LINE_DEFAULT(const char text[]);
Line LINE_DIALOGUE(const char text[], int value);
Line LINE_TEXT(const char str[], int maxLen, _Bool hidden, const char validChars[]);

void display_init(void);
void display_draw_box(DrawnBox *box);
PromptInputs display_box_prompt(BoxContent *box);
void display_cleanup(void);

void format_string(const char template[], const char str[], int width, int offset, char dest[]);
void replace_wrap_string(const char str[], const char first[], const char second[], char dest[]);

#endif