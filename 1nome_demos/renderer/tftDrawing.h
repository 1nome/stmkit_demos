#ifndef DRAWING
#define DRAWING

#include <stdio.h>
#include "stm32_kit/tft.h"
#include "renderer_defines.h"

// dimensions
#define canvas_height 60
#define screenWidth 60
#define res_scale 4

// internal optimization

int task_x = 0;
int task_y = 0;
int task_w = 0;
int task_h = 0;
int task_proj_w = 0;
uint8_t task_active = 0;
uint8_t task_proj_active = 0;
uint16_t task_color = 0;

void do_task()
{
    if (!task_active)
    {
        return;
    }
    TFT_draw_rectangle(task_x * res_scale, task_y * res_scale, res_scale * task_w, res_scale * task_h, task_color);
    task_active = 0;
}

void new_task(const int x, const int y)
{
    do_task();
    task_x = x;
    task_y = y;
    task_w = 1;
    task_h = 1;
    task_active = 1;
}

void new_proj_task()
{
    task_proj_w = 1;
    task_proj_active = 1;
}

void task_shift()
{
    do_task();
    task_y += task_h;
    task_w = task_proj_w;
    task_h = 1;
    task_active = 1;
    task_proj_active = 0;
}

void task_merge()
{
    task_h++;
    task_proj_active = 0;
}

// colors

// draws anything remaining
void flush()
{
    if (task_proj_active)
    {
        if (task_proj_w == task_w)
        {
            task_merge();
        }
        else
        {
            task_shift();
        }
    }
    do_task();
}

void set_color(const RGB color)
{
    const uint16_t c = color565(color.red, color.green, color.blue);
    if (c != task_color)
    {
        flush();
    }
    task_color = c;
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
    TFT_draw_rectangle(0, 0, TFT_WIDTH, TFT_HEIGHT, 5124);
}

//---Drawing---

// Draws a single "pixel" using the currently set background color.
// The cursor position is not changed after drawing!
// You need to move the cursor manually before drawing the next pixel,
// otherwise you will just overwrite the previous pixel.
void draw_pixel(const int x, const int y)
{
    if (task_active)
    {
        if (task_proj_active)
        {
            if (y == task_y + task_h && x == task_x + task_proj_w)
            {
                task_proj_w++;
            }
            else if (x == task_x && y == task_y + task_h + 1)
            {
                if (task_proj_w == task_w)
                {
                    task_merge();
                }
                else
                {
                    task_shift();
                }
                new_proj_task();
            }
            else
            {
                if (task_proj_w == task_w)
                {
                    task_merge();
                }
                else
                {
                    task_shift();
                }
                new_task(x, y);
            }
        }
        else if (y == task_y && x == task_x + task_w)
        {
            task_w++;
        }
        else if (x == task_x && y == task_y + task_h)
        {
            new_proj_task(x, y);
        }
        else
        {
            new_task(x, y);
        }
    }
    else
    {
        new_task(x, y);
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
