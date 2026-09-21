#include <stdio.h>

#include "stm32h743xx.h"
#include "stm32h7xx_hal.h"
#include "lvgl.h"
#include "hx8347.hpp"
#include "touch.hpp"

#define SCREEN_WIDTH  240
#define SCREEN_HEIGHT 320

// 1. Allocate a draw buffer (can be a fraction of the screen size to save RAM)
#define BUFFER_SIZE (SCREEN_WIDTH * 40) // 40 lines of text/graphics
static lv_color16_t screenBuf[BUFFER_SIZE]; 

// 2. Declaration of the flush callback
static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    // 1. Get the boundaries of the updated region
    int32_t x1 = area->x1;
    int32_t y1 = area->y1;
    int32_t x2 = area->x2;
    int32_t y2 = area->y2;

    // 2. Calculate the width and height of the bitmap chunk
    int32_t width = (x2 - x1) + 1;
    int32_t height = (y2 - y1) + 1;
    
    // Total number of pixels in this specific update
    uint32_t num_pixels = width * height; 

    // Cast px_map to uint16_t since RGB565 uses 2 bytes per pixel
    uint16_t * rgb565_bitmap = (uint16_t *)px_map;

    // Set the area and send the data.
    lcd_set_address_window(x1, y1, x2, y2);
    lcd_transmit_rgb565_data(rgb565_bitmap, num_pixels);

    // 4. CRITICAL: Inform LVGL that you are done copying/sending the bitmap
    lv_display_flush_ready(disp);
}

void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data)
{
    /* Check if the screen is currently being touched */
    uint16_t x, y;
    if (touch_get_coordinates(&x, &y)) {
        data->state = LV_INDEV_STATE_PRESSED;
        /* Get the physical X and Y coordinates */
        data->point.x = x;
        data->point.y = y;
    }
    else
    {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

extern "C" void InitializeLvgl()
{
    // General hardware and library initialisation
    lcd_init();
    touch_init();
    lv_init();
    lv_tick_set_cb(HAL_GetTick);

    // Display device initialisation, we use a flush callback to send the bitmap data to the LCD
    lv_display_t * disp = lv_display_create(SCREEN_WIDTH, SCREEN_HEIGHT);
    lv_display_set_buffers(disp, screenBuf, NULL, sizeof(screenBuf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(disp, my_flush_cb);

    // Touch input device initialisation, we use a read callback to get the touch coordinates
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);
}

extern "C" void LoadLvglScreen()
{
#if 1
    // /* Create a simple label */
    // lv_obj_t *label = lv_label_create(lv_scr_act());
    // lv_label_set_text(label, "Hello, ResMed!");
    // lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    void lv_example_flex_flow(void);
    lv_example_flex_flow();
#else
    lv_obj_t * screen = lv_screen_active();
    lv_obj_set_flex_flow(screen, LV_FLEX_FLOW_ROW);
    lv_obj_set_style_flex_main_place(screen, LV_FLEX_ALIGN_SPACE_EVENLY, 0);
    lv_obj_set_style_flex_track_place(screen, LV_FLEX_ALIGN_CENTER, 0);

    /* Long arc, medium speed */
    lv_obj_t * spinner_1 = lv_spinner_create(screen);
    lv_obj_set_size(spinner_1, 80, 80);
    lv_spinner_set_anim_duration(spinner_1, 1500);
    lv_spinner_set_arc_sweep(spinner_1, 270);

    /* Short arc, faster */
    lv_obj_t * spinner_2 = lv_spinner_create(screen);
    lv_obj_set_size(spinner_2, 80, 80);
    lv_spinner_set_anim_duration(spinner_2, 600);
    lv_spinner_set_arc_sweep(spinner_2, 180);
#endif
}

extern "C" void UpdateLvglScreen()
{
    /* Handle LVGL tasks */
    lv_timer_handler();
    HAL_Delay(2);
}
