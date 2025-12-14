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
#define BLINK "\033[5m"
#define BOLD "\033[1m"
#define RESET "\033[0m"

typedef enum {
    DEFAULT,
    TEXT,
    DIALOGUE,
    INLINE
} LineType;

typedef struct {
    int maxLen;
    _Bool hidden;
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

#define LINE_TEXT(str, mxln, h)                                     \
    {                                                               \
        .text = str, .type = TEXT, .data.options = {.maxLen = mxln, \
                                                    .hidden = h }   \
    }

#define LINE_DIALOGUE(str, val) \
    {.text = str, .type = DIALOGUE, .data.value = val}

// For DEFAULT lines
#define LINE_DEFAULT(str) \
    {.text = str, .type = DEFAULT}

void display_init(void);
void display_draw_box(DrawnBox *box);
void display_box_prompt(BoxContent *box);
void display_cleanup(void);

#endif