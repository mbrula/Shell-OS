#ifndef _VIDEO_DRIVER_H_
#define _VIDEO_DRIVER_H_

#include <stdint.h>

/* RGB color structure */
typedef struct color {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} Color;

/* 2D point structure */
typedef struct point {
    uint16_t x;
    uint16_t y;
} Point;

/* Init screen information */
void init_video_driver(void);

/* Dibujar un pixel a pantalla */
void draw_pixel(Point pos, Color color);

/* Gets pixel color on certain position */
void get_pixel(Point pos, Color* out);

/* Move all lines one up */
void move_all_lines_up();

/* Draws a rectangule */
void draw_rect(Point pos, Point size, Color color);

/* Draws character on screen, uses pixel_map wich contains each letter pixel per pixel */
void draw_char_with_background(Point pos, char c, Color foreground, Color background);

/* Draws string on screen */
void draw_string_with_background(Point pos, char * str, Color foreground, Color background);

/* Gets x resolution of the screen*/
uint16_t get_res_x(void);

/* Gets y resolution of the screen*/
uint16_t get_res_y(void);

#endif /* _VIDEO_DRIVER_H_ */
