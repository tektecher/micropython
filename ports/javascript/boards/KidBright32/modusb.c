#include "py/runtime.h"
#include <emscripten.h>

uint8_t last_value = 0;

STATIC mp_obj_t usb_value(mp_obj_t x_obj) {
    mp_int_t x = mp_obj_get_int(x_obj);

    EM_ASM({
        const value = $0 ? 1 : 0;
        console.log(`Set USB to ${value}`);
        if (typeof simSystem !== "undefined") {
            try {
                simSystem.usb.setValue(value);
            } catch(err) {
                console.log(err);
            }
        }
    }, x);

    last_value = x;

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(usb_value_obj, usb_value);

STATIC mp_obj_t usb_on() {
    usb_value(mp_obj_new_int(1));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_on_obj, usb_on);

STATIC mp_obj_t usb_off() {
    usb_value(mp_obj_new_int(0));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_off_obj, usb_off);

STATIC mp_obj_t usb_toggle() {
    usb_value(mp_obj_new_int(1 - last_value));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(usb_toggle_obj, usb_toggle);

STATIC const mp_rom_map_elem_t usb_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_servo) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&usb_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&usb_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&usb_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_toggle), MP_ROM_PTR(&usb_toggle_obj) },
};
STATIC MP_DEFINE_CONST_DICT(usb_module_globals, usb_module_globals_table);

const mp_obj_module_t usb_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&usb_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_usb, usb_user_cmodule, MODULE_USB_ENABLED);
