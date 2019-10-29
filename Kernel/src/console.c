// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <pixelMap.h>
#include <strings.h>
#include <stdarg.h>
#include <videoDriver.h>

#include <console.h>

#define MAX_BUFFER 100

Color background = {29,29,29};
Color foreground = {255,255,255};
Color errorForeground = {255,0,0};
Color okForeground = {0, 255, 0};

static Point ZeroVector = {0,0};
static Point cursor = {0,0};

int get_max_line();

int get_max_cursorY() {
    return get_res_y() / CHAR_HEIGHT;
}

int get_max_cursorX() {
    return get_res_x() / CHAR_WIDTH;
}

/* Initializes the console */
void init_console() {
    clear_console();
    cursor.y = get_max_line()-1;
}

/* Deletes a character */
void backspace() {
  if (cursor.y == 0 && cursor.x == 0) {
    return;
  } else if (cursor.x == 0){
    cursor.x = get_max_cursorX() - 1;
    cursor.y--;
  } else {
    cursor.x--;
  }

  draw_char_with_background(cursor, ' ', background, background);
}

void new_line(){
    cursor.y++;
    cursor.x=0;
}

/* Prints a 0 ending string with the chosen color for background and foreground */
void print_with_colors(Color chosenForeground, Color chosenBackground, char * str, va_list list){

    int i = 0;
    while (str[i] != 0) {
        if (str[i] == '%' && str[i-1] != '\\') {
            char buffer[MAX_BUFFER] = {0};
            switch (str[i+1]) {
                case 'd':
                    int_to_string((int) va_arg(list,int), buffer, 10);
                    print(buffer);
                    i++;
                    break;
                case 's':
                    print(va_arg(list,char*));
                    i++;
                    break;
            }
        } else {
            switch(str[i]) {
                case '\n':
                    new_line(); break;
                case '\t':
                    cursor.x+=4; break;
                default:
                    draw_char_with_background(cursor, str[i], chosenForeground, chosenBackground);
                    cursor.x++; break;
            }
        }


        if(cursor.x >= get_res_x() / CHAR_WIDTH){ 
            cursor.x = 0;
            cursor.y++;
        }
        if(cursor.y >= get_res_y() / CHAR_HEIGHT) 
            move_all_up();
            cursor.y--;
        }
        i++;
    }
}

/* Print only one character on stdout */
void print_char(char c){
    switch(c) {
        case '\n':
            new_line(); break;
        case '\t':
            cursor.x+=4; break;
        case '\b':
            backspace(); break;
        default:
            draw_char_with_background(cursor, c, foreground, background);
            cursor.x++;
            break;
    }

    /* Start a new line if cursor overflows */
    if(cursor.x >= get_res_x() / CHAR_WIDTH) {
        cursor.x = 0;
        cursor.y++;
    }

    /* Moves all up if needed */
    if(cursor.y >= get_res_y() / CHAR_HEIGHT) {
        move_all_up();
        cursor.y--;
    }
}

/* Print only one character on stderr */
void printError_char(char c){ 
    switch(c) {
        case '\n':
            new_line(); break;
        case '\t':
            cursor.x += 4; break;
        case '\b':
            backspace(); break;
        default:
            draw_char_with_background(cursor, c, errorForeground, background);
            cursor.x++;
            break;
    }

    /* Start a new line if cursor overflows */
    if(cursor.x >= get_res_x() / CHAR_WIDTH) {
        cursor.x = 0;
        cursor.y++;
    }

    /* Moves all up if needed */
    if(cursor.y >= get_res_y() / CHAR_HEIGHT) {
        move_all_up();
        cursor.y--;
    }
}

/* Prints a string with count length on stdout */
void print_N(const char * str, uint64_t length){
    for(uint64_t i = 0; i < length; i++){
        print_char(str[i]);
    }
}

/* Prints a string with count length on stderr */
void printError_N(const char * str, uint64_t length){
    for(uint64_t i = 0; i < length; i++){
        printError_char(str[i]);
    }
}

/* Prints on stdout a formatted string */
void print(char * str, ...) {
    va_list list;
    va_start(list, str);
    print_with_colors(foreground, background, str, list);
    va_end(list);
}

/* Prints an integer */
void printInteger(uint64_t dec){
    print("%d", dec);
}

/* Prints on stderr a string like printf */
void printError(char * str, ...) {
    va_list list;
    va_start(list, str);
    print_with_colors(errorForeground, background, str, list);
    va_end(list);
}

/* Moves all lines one up */
void move_all_up() {
  move_all_lines_up();
    clear_line(get_max_line() - 1);
}

/* Deletes a line */
void clear_line(uint64_t line){
    Point posDraw = {0,line*CHAR_HEIGHT};
    for(uint64_t j = 0; j < CHAR_HEIGHT; j++) {
        for (uint64_t i = 0; i < get_res_x(); ++i) {
            draw_pixel(posDraw, background);
            posDraw.x++;
        }
        posDraw.x=0;
        posDraw.y++;
    }
}

/* Clears screen */
void clear_console() {
    Point size = {get_res_x(), get_res_y()};
    draw_rect(ZeroVector, size, background);
    cursor.x = 0;
    cursor.y = get_max_line()-1;
}

/* Gets max characters per line */
int get_max_line() {
    return get_res_y() / CHAR_HEIGHT;
}
