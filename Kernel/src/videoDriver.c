// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdint.h>
#include <lib.h>
#include <pixelMap.h>

#include <videoDriver.h>

/* Struct taken from https://wiki.osdev.org/Getting_VBE_Mode_Info */
typedef struct __attribute__((packed)) {
    uint16_t attributes;
    uint8_t win_A,win_B;
    uint16_t granularity;
    uint16_t winsize;
    uint16_t segment_A, segment_B;
    uint32_t real_fct_ptr;
    uint16_t pitch;

    uint16_t x_res, y_res; // Screen X and Y resolution
    uint8_t w_char, y_char, planes, bpp, banks; // BPP - Bits Per Pixel
    uint8_t memory_model, bank_size, image_pages;
    uint8_t reserved0;

    uint8_t red_mask, red_position;
    uint8_t green_mask, green_position;
    uint8_t blue_mask, blue_position;
    uint8_t rsv_mask, rsv_position;
    uint8_t directcolor_attributes;

    uint32_t physbase;  // your LFB (Linear Framebuffer) address ;)
    uint32_t reserved1;
    uint16_t reserved2;
} ModeInfoBlock;

static ModeInfoBlock * infoBlock;

/* Init screen information */
void init_video_driver() {
    infoBlock = (ModeInfoBlock*)0x0000000000005C00;
}

/* Checks if pos is inside the screen */
static int64_t out_of_range_pixel(Point pos) {
    return !((pos.x >= 0) && (pos.x <= infoBlock->x_res) && (pos.y >= 0) && (pos.y <= infoBlock->y_res));
}

/* Dibujar un pixel a pantalla */
void draw_pixel(Point pos, Color color){
    if (out_of_range_pixel(pos))
        return;
    
    uint8_t * pixel_address;
    uint64_t bpp = infoBlock->bpp / 8;
    pixel_address = (uint8_t *) ((uint64_t)(infoBlock->physbase + pos.x * bpp  + pos.y * (infoBlock->x_res) * bpp));
    pixel_address[0] = color.b;
    pixel_address[1] = color.g;
    pixel_address[2] = color.r;
}

/* Gets pixel color on certain position */
void get_pixel(Point pos, Color* out){
    if (out_of_range_pixel(pos))
        return;

    uint8_t * pixel_address;
    uint64_t bpp = infoBlock->bpp / 8;
    pixel_address = (uint8_t *) ((uint64_t)(infoBlock->physbase + pos.x * bpp + pos.y * (infoBlock->x_res) * bpp));
    out->b = pixel_address[0];
    out->g = pixel_address[1];
    out->r = pixel_address[2];
}

/* Move all lines one up */
void move_all_lines_up() {
  void * source = (void *)((uint64_t)(infoBlock->physbase + (infoBlock->bpp/8) * infoBlock->x_res * CHAR_HEIGHT));
  void * dest = (void *)((uint64_t)infoBlock->physbase);
  uint64_t size = (((infoBlock->bpp/8) * (infoBlock->x_res-1) *  infoBlock->y_res)-(infoBlock->bpp/8) * infoBlock->x_res * CHAR_HEIGHT);
  infoBlock->physbase = ((uint64_t)(memcpy(dest, source, size)));
}

/* Draws a rectangule */
void draw_rect(Point pos, Point size, Color color) {
    Point auxPos = {0,0};
    for (auxPos.y = pos.y; auxPos.y < pos.y + size.x; auxPos.y++) {
        for (auxPos.x = pos.x; auxPos.x < pos.x + size.x; auxPos.x++) {
            draw_pixel(auxPos, color);
        }
    }
}

/* Draws character on screen, uses pixel_map wich contains each letter pixel per pixel */
void draw_char_with_background(Point pos, char c, Color foreground, Color background){
    uint8_t * cMap = pixel_map(c);
    for (uint64_t j = 0; j < CHAR_HEIGHT; ++j) {
        for (uint64_t i = 0; i < CHAR_WIDTH; ++i) {
            Point aux = {(pos.x * (CHAR_WIDTH)) + i, (pos.y * CHAR_HEIGHT) + j};
            if((cMap[j] >> (CHAR_WIDTH - i - 1)) & 1) // NO se porque el -1, pero funciona...
                draw_pixel(aux,foreground);
            else
                draw_pixel(aux, background);
        }
    }
}

/* Draws string on screen */
void draw_string_with_background(Point pos, char * str, Color foreground, Color background){
    Point auxPos = {pos.x,pos.y};

    while((*str) != 0) {
        draw_char_with_background(auxPos, (*str), foreground, background);
        auxPos.x++;
        str++;
    }
}

/* Gets x resolution of the screen*/
uint16_t get_res_x(){
    return infoBlock->x_res;
}

/* Gets y resolution of the screen */
uint16_t get_res_y(){
    return infoBlock->y_res;
}
