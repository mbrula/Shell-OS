#ifndef GRAPHICS_LIBRARY_H_
#define GRAPHICS_LIBRARY_H_

#include <clib.h>
#include <types.h>
#include <pixelMap.h>

void drawRect(Point start, Point size, Color color);

void drawString(Point pos, char * str, Color foreground, Color background);

#endif /* GRAPHICS_LIBRARY_H_ */
