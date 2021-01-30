#include "ili9340.h"
#include "fontx.h"
#include "bmpfile.h"
#include "decode_image.h"
#include "pngle.h"
#include "dw_font.h"

#include "esp_log.h"

// Include MicroPython API.
#include "py/runtime.h"
#include "py/mphal.h"

#define CONFIG_MOSI_GPIO  19
#define CONFIG_SCLK_GPIO  18
#define CONFIG_CS_GPIO    5
#define CONFIG_DC_GPIO    16
#define CONFIG_RESET_GPIO 23
#define CONFIG_BL_GPIO    4

TFT_t dev;
bool init = false;

extern dw_font_info_t font_supermarket_regular20;
extern dw_font_info_t font_supermarket_regular40;
extern dw_font_info_t font_supermarket_regular60;
extern dw_font_info_t font_supermarket_regular120;

extern dw_font_info_t font_th_sarabun_new_regular20;
extern dw_font_info_t font_th_sarabun_new_regular40;
extern dw_font_info_t font_th_sarabun_new_regular60;

const dw_font_info_t* fonts[] = {
    &font_supermarket_regular20,
    &font_supermarket_regular40,
    &font_supermarket_regular60,
    &font_supermarket_regular120,
    &font_th_sarabun_new_regular20,
    &font_th_sarabun_new_regular40,
    &font_th_sarabun_new_regular60,
};

dw_font_t myfont;

uint16_t textColor = 0xFFFF;

void draw_pixel(int16_t x, int16_t y) {
  lcdDrawPixel(&dev, x, y, textColor);
}

void clear_pixel(int16_t x, int16_t y) {
  // lcdDrawPixel(&dev, x, y, 0x0000);
}

STATIC mp_obj_t display_init() {
    if (!init) {
        spi_master_init(&dev, CONFIG_MOSI_GPIO, CONFIG_SCLK_GPIO, CONFIG_CS_GPIO, CONFIG_DC_GPIO, CONFIG_RESET_GPIO, CONFIG_BL_GPIO);
    }

    lcdInit(&dev, 0x7789, 240, 135, 40, 52);
    lcdFillScreen(&dev, 0x0000);

    dw_font_init(&myfont,
               240,
               135,
               draw_pixel,
               clear_pixel);

    init = true;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(display_init_obj, display_init);

STATIC mp_obj_t display_fill(mp_obj_t color_obj) {
    mp_int_t color = mp_obj_get_int(color_obj);

    lcdFillScreen(&dev, color);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(display_fill_obj, display_fill);

STATIC mp_obj_t display_pixel(mp_obj_t x_obj, mp_obj_t y_obj, mp_obj_t color_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);
    mp_int_t y = mp_obj_get_int(y_obj);
    mp_int_t color = mp_obj_get_int(color_obj);

    lcdDrawPixel(&dev, x, y, color);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_3(display_pixel_obj, display_pixel);

STATIC mp_obj_t display_line(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[0]);
    mp_int_t y1 = mp_obj_get_int(args[1]);
    mp_int_t x2 = mp_obj_get_int(args[2]);
    mp_int_t y2 = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);

    lcdDrawLine(&dev, x1, y1, x2, y2, color);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(display_line_obj, 5, 5, display_line);

STATIC mp_obj_t display_rect(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[0]);
    mp_int_t y1 = mp_obj_get_int(args[1]);
    mp_int_t x2 = mp_obj_get_int(args[2]);
    mp_int_t y2 = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);

    lcdDrawRect(&dev, x1, y1, x2, y2, color);
    
    /*char buff[100];
    sprintf(buff, "display_rect: %d, %d, %d, %d, %04x\r\n", x1, y1, x2, y2, color);
    mp_hal_stdout_tx_str(buff);*/

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(display_rect_obj, 5, 5, display_rect);

STATIC mp_obj_t display_fill_rect(size_t n_args, const mp_obj_t *args) {
    mp_int_t x1 = mp_obj_get_int(args[0]);
    mp_int_t y1 = mp_obj_get_int(args[1]);
    mp_int_t x2 = mp_obj_get_int(args[2]);
    mp_int_t y2 = mp_obj_get_int(args[3]);
    mp_int_t color = mp_obj_get_int(args[4]);

    lcdDrawFillRect(&dev, x1, y1, x2, y2, color);
    
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(display_fill_rect_obj, 5, 5, display_fill_rect);

STATIC mp_obj_t display_circle(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[0]);
    mp_int_t y = mp_obj_get_int(args[1]);
    mp_int_t r = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);

    lcdDrawCircle(&dev, x, y, r, color);
    
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(display_circle_obj, 4, 4, display_circle);

STATIC mp_obj_t display_fill_circle(size_t n_args, const mp_obj_t *args) {
    mp_int_t x = mp_obj_get_int(args[0]);
    mp_int_t y = mp_obj_get_int(args[1]);
    mp_int_t r = mp_obj_get_int(args[2]);
    mp_int_t color = mp_obj_get_int(args[3]);

    lcdDrawFillCircle(&dev, x, y, r, color);
    
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(display_fill_circle_obj, 4, 4, display_fill_circle);

STATIC mp_obj_t display_text(size_t n_args, const mp_obj_t *args) {
    const char* s = mp_obj_str_get_str(args[0]);
    mp_int_t x = mp_obj_get_int(args[1]);
    mp_int_t y = mp_obj_get_int(args[2]);

    if (n_args > 3) {
        textColor = mp_obj_get_int(args[3]);
    } else {
        textColor = 0xFFFF;
    }

    int fontIndex = 0;

    if (n_args > 4) {
        fontIndex = mp_obj_get_int(args[4]);
    }

    dw_font_setfont(&myfont, (dw_font_info_t *)fonts[fontIndex]);
    dw_font_goto(&myfont, x, y);
    dw_font_print(&myfont, (char *)s);
    
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(display_text_obj, 3, 5, display_text);

STATIC const mp_rom_map_elem_t display_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_display) },
    { MP_ROM_QSTR(MP_QSTR_init), MP_ROM_PTR(&display_init_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill), MP_ROM_PTR(&display_fill_obj) },
    { MP_ROM_QSTR(MP_QSTR_pixel), MP_ROM_PTR(&display_pixel_obj) },
    { MP_ROM_QSTR(MP_QSTR_line), MP_ROM_PTR(&display_line_obj) },
    { MP_ROM_QSTR(MP_QSTR_rect), MP_ROM_PTR(&display_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_rect), MP_ROM_PTR(&display_fill_rect_obj) },
    { MP_ROM_QSTR(MP_QSTR_circle), MP_ROM_PTR(&display_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_fill_circle), MP_ROM_PTR(&display_fill_circle_obj) },
    { MP_ROM_QSTR(MP_QSTR_text), MP_ROM_PTR(&display_text_obj) },

    { MP_ROM_QSTR(MP_QSTR_FONT_SUPERMARKET_20), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_FONT_SUPERMARKET_40), MP_ROM_INT(1) },
    { MP_ROM_QSTR(MP_QSTR_FONT_SUPERMARKET_60), MP_ROM_INT(2) },
    { MP_ROM_QSTR(MP_QSTR_FONT_SUPERMARKET_120), MP_ROM_INT(3) },
    { MP_ROM_QSTR(MP_QSTR_FONT_TH_SARABUN_NEW_20), MP_ROM_INT(4) },
    { MP_ROM_QSTR(MP_QSTR_FONT_TH_SARABUN_NEW_40), MP_ROM_INT(5) },
    { MP_ROM_QSTR(MP_QSTR_FONT_TH_SARABUN_NEW_60), MP_ROM_INT(6) },
};
STATIC MP_DEFINE_CONST_DICT(display_module_globals, display_module_globals_table);

const mp_obj_module_t display_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&display_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_display, display_user_cmodule, MODULE_DISPLAY_ENABLED);
