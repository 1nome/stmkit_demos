/**
* @file	rendrMain.c
 * @author	1nome
 *
 * @brief	A renderer demo that renders a scene to the tft display (or terminal).
 *          The MCU manually sends the frame buffer to the display.
 *          Be sure to shield high frequency wires to minimise interference.
 *          I put the stack to the CCM in my ldscript; lower screen dimensions if this doesn't compile.
 */

#include "stm32_kit.h"
#include "stm32_kit/overclock.h"
#define DRAW_SYSTEM 3
#include "renderer/renderer.h"
#include "renderer/suzanne.h"
#include "renderer/m16.h"
#include "stm32_kit/tft.h"
#include "stm32_kit/uart.h"

BOARD_SETUP void setup(void)
{
    clock168Mhz();
    SysTick_Config(SystemCoreClock / 10000);

    TFT_setup();
}

int main()
{
    clear_screen();

    const int nObjects = 1;
    object3d *objects = calloc(nObjects, sizeof(object3d));
    // objects[0] = suzanne;
    // objects[0].scaleX = 1.7f;
    // objects[0].scaleY = 1.7f;
    // objects[0].scaleZ = 1.7f;
    objects[0] = m16;
    objects[0].rotY = piHalfs;

    const int n = 40;

    const int maxVs = 3200;
    vertex3d *worldVs = calloc(maxVs, sizeof(vertex3d));
    int totalVs = 0;

    // camera cam1 = {3, 0, 0, 1, 0, 0, 0, 1, 0, 1};
    camera cam1 = {1, 0.5f, -2.5f, 1, 0.4f, 0, 0, 1, 0, 1};
    vertex3d skyDir = {0, 1, -1};
    vectorNormalize(&skyDir);
    // while (1)
    for (int t = 0; t < n; t++)
    {
        // update objects
        // objects[0].rotY += 0.09465747f;
        cam1.posZ += 0.1f;

        //   copy objects to world, rotate, scale, move them in that order
        if (!makeWorld(objects, nObjects, worldVs, &totalVs, maxVs))
        {
            break;
        }

        // translate to camera space, ~~add perspective~~
        toCamera(worldVs, totalVs, cam1);

        clear_screen();
        rasterize2(objects, nObjects, worldVs, screenWidth, canvas_height, skyDir);
    }

    free(worldVs);
    free(objects);

    end_drawing();

    return 0;
}
