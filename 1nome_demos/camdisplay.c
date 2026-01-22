/**
 * @file	camdisplay.c
 * @author	1nome
 *
 * @brief	A program that displays the camera output on the tft display.
 *          The data from the camera is copied to a buffer by the DMA (no other way afaik)
 *          The MCU then manually sends it to the display.
 *          My display doesn't handle the high frequency SPI (interference between the wires)
 *          but seems fine at 16Mhz.
 *          I wrapped my SCL and SDA with aluminium foil, that fixed the problem.
 *          I didn't bother with rotation so physically rotate the camera or display as needed.
 *          I put the stack to the CCM in my ldscript; lower BUFFER_SIZE if this doesn't compile.
 *          There is a diagram on how to connect up the camera in ./images
 *
 * @attention needs I2C_ALT set in config.h
 */

#include <string.h>

#include "stm32_kit.h"
#include "stm32_kit/overclock.h"
#include "stm32_kit/tft.h"
#include "stm32_kit/cam.h"

BOARD_SETUP void setup(void)
{
    clock168Mhz(); // comment this line if your display stops working (but then the spi is slower than the camera)
    SysTick_Config(SystemCoreClock / 10000);
    CAM_setup();
    TFT_setup();
}

#define BUFFER_SIZE 28800
uint32_t buff[BUFFER_SIZE]; // ensures word-alignment
#define WIDTH 240
#define HEIGHT 240

int main(void)
{
    uint16_t* img1 = buff;
    uint16_t* img2 = buff;
    img2 += BUFFER_SIZE;

    CAM_mcu_crop_en(40, 0, WIDTH, HEIGHT);
    CAM_DMA_setup(img1, img2, BUFFER_SIZE);
    // CAM_reg_write(REG_SCALING_YSC, TEST_PATTTERN_1); // uncomment this to get 8 (6 visible by default) color bars

    TFT_draw_rectangle(0, 0, WIDTH, HEIGHT, 0);
    TFT_start_colors();

    CAM_continuous_start();
    while (1)
    {
        while (!CAM_DMA_get_ct()){}
        TFT_push_colors(img1, BUFFER_SIZE);
        while (CAM_DMA_get_ct()){}
        TFT_push_colors(img2, BUFFER_SIZE);
    }
}
