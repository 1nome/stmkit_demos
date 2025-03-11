// taken from VSB-TUO UPR

#ifndef DRAWING
#define DRAWING
#include <stdio.h>
#include "stm32_kit/uart.h"
#include "renderer_defines.h"

// ANSI escape sequences reference:
// - https://www.lihaoyi.com/post/BuildyourownCommandLinewithANSIescapecodes.html#cursor-navigation
// - https://tldp.org/HOWTO/Bash-Prompt-HOWTO/x361.html

// Height of the drawing canvas
// Increase this if you need more height for drawing.
// However, you will the need to scroll further up to see the
// beginning of the drawing :)
#define canvas_height 90
#define screenWidth (int)(canvas_height * 2.1)

// Use the functions declared here (`set_red_color`, `move_right`, `draw_pixel`, `animate`, etc.)
// to draw things.

void flush()
{
    fflush(stdout);
}

//---Cursor management---

// Hide the blinking terminal cursor.
void hide_cursor()
{
    USART3_write("\x1b[?25l", 6);
    // flush();
}

// Show the terminal cursor.
void show_cursor()
{
    USART3_write("\x1b[?25h", 6);
    // flush();
}

//---Color management---

// Reset the drawing and background color to the default values.
void reset_color()
{
    USART3_write("\x1b[0m", 4);
    // flush();
}

// Set the background color to red.
void set_red_color()
{
    USART3_write("\x1b[41;1m", 7);
    // flush();
}

// Set the background color to green.
void set_green_color()
{
    USART3_write("\x1b[42;1m", 7);
    // flush();
}

// Set the background color to blue.
void set_blue_color()
{
    USART3_write("\x1b[44;1m", 7);
    // flush();
}

// Set the background color to white.
void set_white_color()
{
    USART3_write("\x1b[47;1m", 7);
    // flush();
}

// Set the background color to yellow.
void set_yellow_color()
{
    USART3_write("\x1b[43;1m", 7);
    // flush();
}

// Set the background color to black.
void set_black_color()
{
    USART3_write("\x1b[40;1m", 7);
    // flush();
}

void set_color(RGB color)
{
    char str[20];
    USART3_write(str, sprintf(str, "\x1b[48;2;%d;%d;%dm", color.red, color.green, color.blue));
}

void set_grayscale(unsigned char scale)
{
    char str[20];
    USART3_write(str, sprintf(str, "\x1b[48;2;%d;%d;%dm", scale, scale, scale));
}

//---Cursor movement---

// Move cursor right.
void move_right()
{
    USART3_write("\x1b[1C", 4);
    // flush();
}

// Move cursor left.
void move_left()
{
    USART3_write("\x1b[1D", 4);
    // flush();
}

// Move cursor up.
void move_up()
{
    USART3_write("\x1b[1A", 4);
    // flush();
}

// Move cursor down.
void move_down()
{
    USART3_write("\x1b[1B", 4);
    // flush();
}

// Move cursor to the specified `line` and `column` position.
// Positions are numbered from 1, the origin is at (1, 1).
void move_to(int line, int column)
{
    char str[11];
    USART3_write(str, snprintf(str, 11, "\x1b[%d;%dH", line, column));
    // flush();
}

//---Screen management---

// Reset the drawing environment.
// Clears the screen, moves the cursor to the origin in the top-left corner and
// prepares the drawing canvas.
void clear_screen()
{
    reset_color();
    hide_cursor();

    // Clear screen
    USART3_write("\x1b[2J", 4);
    // Move cursor to origin
    move_to(1, 1);
    // Create canvas
    for (int i = 0; i < canvas_height; i++)
    {
        USART3_putc('\n');
    }
    move_to(1, 1);
    // flush();
}

//---Drawing---

// Draws a single "pixel" using the currently set background color.
// The cursor position is not changed after drawing!
// You need to move the cursor manually before drawing the next pixel,
// otherwise you will just overwrite the previous pixel.
void draw_pixel()
{
    USART3_putc(' ');
    // move_left();
}

// Clears the drawing session, resets colors and cursor.
// Call this before the end of the program, otherwise your terminal will get messed up.
void end_drawing()
{
    reset_color();
    move_to(1, 1);
    for (int i = 0; i < canvas_height; i++)
    {
        move_down();
    }
    USART3_putc('\n');
    show_cursor();
}

void animate_ms(int ms)
{
    // flush();
    //  Sleep for the given amount of ms
    delay_ms(ms);
}

// Pause the program for a while, so that you can see what has been drawn so far.
// If you repeatedly call animate() after drawing something, you can crate interactive animations.
// You can also combine this with clear_screen() to draw something new on every "animation frame".
void animate()
{
    animate_ms(500);
}

#endif
