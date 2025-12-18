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
#define K_TAB 9
#define K_BACKSPACE 8
#define K_UP 72
#define K_DOWN 80
#define K_RIGHT 77
#define K_LEFT 75

typedef enum {  //defines each line type
    DEFAULT,
    TEXT,
    DIALOGUE,
} LineType;

typedef struct {    //defines the properties of an input text field
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

typedef struct {    //the box to be processed
    char title[LINE_LENGTH];
    Line content[LINE_COUNT];   //array of lines
    char footer[LINE_LENGTH];
} BoxContent;

typedef struct {    //the processed box to be printed 
    int width, height;
    char title[LINE_LENGTH];
    char content[LINE_COUNT][LINE_LENGTH];
    char footer[LINE_LENGTH];
} DrawnBox;

typedef struct {
    int textInputCount; //contains the number of input fields
    char **textInputs;  //array of strings, contains the text the user entered
    int dialogueValue;  //contains the choice of the user
} PromptInputs;

Line LINE_DEFAULT(const char text[]);   //constructs a line of DEFAULT type 
Line LINE_DIALOGUE(const char text[], int value);   //constructs a line of DIALOGUE type
Line LINE_TEXT(const char str[], int maxLen, _Bool hidden, const char validChars[]);    //constructs a line of TEXT type
Line *MULTI_LINE_DEFAULT(const char text[], const char linePrefix[], int width, int *lineCnt);   //constructs multiple DEFAULT lines given some large text

void display_init(void);  //initialize the terminal to use ANSI codes and UTF-8
void display_draw_box(DrawnBox *box);   //renders a processed box on screen
PromptInputs display_box_prompt(BoxContent *box);   //displays an interactive box that takes prompt from the user and returns it to the program
void display_cleanup(void);   //resets the terminal to normal state

void format_string(const char template[], const char str[], int width, int offset, char dest[]);    //takes a template with some %s ,and replaces %s with str width that describes target width of the output string, the offset that describes where to begin printing the str from, and puts the formatted output in dest 

void replace_wrap_string(const char str[], const char first[], const char second[], char dest[]);   //puts first as a prefix and second as a suffix if no %s is present or only one is present, if more than two %s are present, put first instead of the first one , and second instead of the second one 


#endif