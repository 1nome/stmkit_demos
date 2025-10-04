#ifndef DRAWING
#define DRAWING

#include <stdio.h>
#include "stm32_kit/tft.h"
#include "renderer_defines.h"

// dimensions
#define canvas_height 120
#define screenWidth 120
#define res_scale 2

uint16_t frame_buffer[canvas_height * screenWidth];
const uint16_t framebufsiz = canvas_height * screenWidth * 2;
uint16_t curr_color = 0;

void set_color(const RGB color)
{
    const uint16_t c = color565(color.red, color.green, color.blue);
    curr_color = c;
}

void set_grayscale(const uint8_t scale)
{
    const RGB c = {scale, scale, scale};
    set_color(c);
}

//---Screen management---

// Reset the drawing environment.
// Clears the screen, moves the cursor to the origin in the top-left corner and
// prepares the drawing canvas.
void clear_screen()
{
    memset(frame_buffer, 20, framebufsiz);
}

//---Drawing---

// Draws a single "pixel" using the currently set background color.
// The cursor position is not changed after drawing!
// You need to move the cursor manually before drawing the next pixel,
// otherwise you will just overwrite the previous pixel.
void draw_pixel(const int x, const int y)
{
    frame_buffer[y * screenWidth + x] = curr_color;
}

void flush()
{
    TFT_write_command(ST7789_RAMWR);
    SPI2_wait_for_idle();
    io_set(TFT_DC, 1);
    for (int i = 0; i < canvas_height; i++)
    {
        for (int k = 0; k < res_scale; k++)
        {
            for (int j = 0; j < screenWidth; j++)
            {
                const uint8_t hi = frame_buffer[i * screenWidth + j] >> 8;
                const uint8_t lo = frame_buffer[i * screenWidth + j];
                for (int l = 0; l < res_scale; l++)
                {
                    SPI2_transmit(hi);
                    SPI2_transmit(lo);
                }
            }
        }
    }
}

// Clears the drawing session, resets colors and cursor.
// Call this before the end of the program, otherwise your terminal will get messed up.
void end_drawing()
{

}

void animate_ms(const int ms)
{
    delay_ms(ms);
}

#endif
