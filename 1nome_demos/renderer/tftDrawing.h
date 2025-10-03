// taken from VSB-TUO UPR

#ifndef DRAWING
#define DRAWING
#include <stdio.h>
#include "stm32_kit/tft.h"
#include "renderer_defines.h"

// ANSI escape sequences reference:
// - https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html#cursor-navigation
// - https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html

// Height of the drawing canvas
// Increase this if you need more height for drawing.
// However, you will the need to scroll further up to see the
// beginning of the drawing :)
#define canvas_height 120
#define screenWidth 120
#define res_scale 2

// Use the functions declared here (`set_red_color`, `move_right`, `draw_pixel`, `animate`, etc.)
// to draw things.

//---Color management---

uint16_t curr_color;

void set_color(const RGB color)
{
    curr_color = color565(color.red, color.green, color.blue);
}

void set_grayscale(const uint8_t scale)
{
    curr_color = color565(scale, scale, scale);
}

// Move cursor to the specified `line` and `column` position.
// Positions are numbered from 1, the origin is at (1, 1).
uint8_t display_x = 0, display_y = 0;
void move_to(const int line, const int column)
{
    display_x = column;
    display_y = line;
}

//---Screen management---

// Reset the drawing environment.
// Clears the screen, moves the cursor to the origin in the top-left corner and
// prepares the drawing canvas.
void clear_screen()
{
    TFT_draw_rectangle(0, 0, TFT_WIDTH, TFT_HEIGHT, 5124);
    move_to(0, 0);
}

//---Drawing---

// Draws a single "pixel" using the currently set background color.
// The cursor position is not changed after drawing!
// You need to move the cursor manually before drawing the next pixel,
// otherwise you will just overwrite the previous pixel.
void draw_pixel()
{
    TFT_draw_rectangle(display_x * res_scale, display_y * res_scale, res_scale, res_scale, curr_color);
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
