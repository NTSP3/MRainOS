#include "screen.h"
#include "console.h"

#include "ports.h"
#include <string.h>

// Video memory addresses
#define VIDEO_MEMORY 0xB8000
#define VIDEO_MEMORY_END (VIDEO_MEMORY + SCREEN_WIDTH * SCREEN_HEIGHT * 2)

// Ports for VGA
#define VGA_SELECT 0x3D4
#define VGA_DATA 0x3D5

uint16_t getScreenData(unsigned int offset)
{
    if (offset >= SCREEN_HEIGHT * SCREEN_WIDTH)
        return 0;
    
    return *(uint16_t*)(VIDEO_MEMORY + 2 * offset);
}

// Sets the char c at pos (x, y) with format fmt
int setChar(unsigned int x, unsigned int y, char c, char fmt)
{
    if (x > SCREEN_WIDTH)
        return -1;

    if (y > SCREEN_HEIGHT)
        return -1;

    setCharOffset(y * SCREEN_WIDTH + x, c, fmt);

    return 0;
}

// Like setChar but with offset instead of position
// !!! offset is supposed to be within screen bounds
void setCharOffset(unsigned int offset, char c, char fmt)
{
    char *px = (char*)(VIDEO_MEMORY + offset * 2);

    *px = c;
    *++px = fmt;
}

// Fills the screen
void fillScreen(char c, char fmt)
{
    // The pixel data
    short px = c | (fmt << 8);

    // Fill each pixel
    for (short *pxPtr = (short*)VIDEO_MEMORY; pxPtr < (short*)VIDEO_MEMORY_END; ++pxPtr)
        *pxPtr = px;
}

int setCaret(unsigned int x, unsigned int y)
{
    if (x > SCREEN_WIDTH)
        return -1;

    if (y > SCREEN_HEIGHT)
        return -1;

    int cursorPos = y * SCREEN_WIDTH + x;

    newCaret(cursorPos);
    setCaretOffset(cursorPos); //Set the cursor itself on the screen

    return 0;
}

void setCaretOffset(unsigned int i)
{
    // High
    outb(0x0E, VGA_SELECT);
    outb((i & 0xFF00) >> 8, VGA_DATA);

    // Low
    outb(0x0F, VGA_SELECT);
    outb(i & 0xFF, VGA_DATA);
}

void screenScroll(size_t pixels)
{
    // TODO : assert(pixels < SCREEN_WIDTH * SCREEN_HEIGHT && pixels != 0);

    // Copy pixels
    memmove((void*)VIDEO_MEMORY, (void*)(VIDEO_MEMORY + 2 * pixels), 2 * SCREEN_WIDTH * SCREEN_HEIGHT * pixels);    

    // Zero memory
    for (char *data = (char*)((SCREEN_WIDTH * SCREEN_HEIGHT - pixels) * 2);
            data < (char*)(2 * SCREEN_WIDTH * SCREEN_HEIGHT); ++data)
        *data = 0;
}
