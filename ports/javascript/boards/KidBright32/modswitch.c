#include "py/runtime.h"
#include <emscripten.h>

#include "mphalport.h"
#include "mpconfigport.h"

uint8_t SW_Value[2] = { 1, 1 };

mp_obj_t onPressCallback[2] = { MP_OBJ_NULL, MP_OBJ_NULL };
mp_obj_t onReleaseCallback[2] = { MP_OBJ_NULL, MP_OBJ_NULL };
mp_obj_t onPressedCallback[3] = { MP_OBJ_NULL, MP_OBJ_NULL, MP_OBJ_NULL };
bool pressedFlag[2] = { false, false };
uint64_t pressStart[2] = { 0, 0 };

void mp_switch_value_change_handle(int pin, int value) ;

void mp_js_switch_poll() {
    int s1_value = EM_ASM_INT({
        return simSystem.switch[0].value;
    });

    int s2_value = EM_ASM_INT({
        return simSystem.switch[1].value;
    });

    // EM_ASM_INT({ console.log("SW Poll", $0, $1) }, s1_value, s2_value);

    if (SW_Value[0] != s1_value) {
        mp_switch_value_change_handle(1, s1_value);
    }
    if (SW_Value[1] != s2_value) {
        mp_switch_value_change_handle(2, s2_value);
    }
}

void mp_switch_value_change_handle(int pin, int value) {
    mp_obj_t callback = MP_OBJ_NULL;

    int index = pin - 1;

    if (value == 0) {
        callback = onPressCallback[index];
        pressStart[index] = mp_hal_ticks_ms();
    }
    if (value == 1) {
        callback = onReleaseCallback[index];
        if (pressStart[index] != 0) {
            uint32_t diff = mp_hal_ticks_ms() - pressStart[index];
            pressedFlag[index] = diff >= 40 && diff < 1000;
            pressStart[index] = 0;
        }
    }

    SW_Value[index] = value;

    if (callback != MP_OBJ_NULL) {
        // mp_sched_schedule(callback, mp_const_none);
        mp_call_function_0(callback);
    }

    if (SW_Value[0] == 1 && SW_Value[1] == 1) {
        callback = MP_OBJ_NULL;

        if (pressedFlag[0] && pressedFlag[1]) {
            callback = onPressedCallback[2];
        } else if (pressedFlag[0]) {
            callback = onPressedCallback[0];
        } else if (pressedFlag[1]) {
            callback = onPressedCallback[1];
        }
        pressedFlag[0] = false;
        pressedFlag[1] = false;

        if (callback != MP_OBJ_NULL) {
            // mp_sched_schedule(callback, mp_const_none);
            mp_call_function_0(callback);
        }
    }

    MICROPY_EVENT_POLL_HOOK
}

STATIC mp_obj_t switch_value(mp_obj_t pin_obj) {
    mp_int_t pin = mp_obj_get_int(pin_obj);

    if (pin < 1 || pin > 2) {
        return mp_obj_new_int(0);
    }

    return mp_obj_new_int(1 - SW_Value[pin - 1]);
}
MP_DEFINE_CONST_FUN_OBJ_1(switch_value_obj, switch_value);

STATIC mp_obj_t switch_press(mp_obj_t pin_obj, mp_obj_t callback_obj) {
    mp_int_t pin = mp_obj_get_int(pin_obj);

    if (pin >= 1 && pin <= 2) {
        onPressCallback[pin - 1] = callback_obj != mp_const_none ? callback_obj : MP_OBJ_NULL;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(switch_press_obj, switch_press);

STATIC mp_obj_t switch_release(mp_obj_t pin_obj, mp_obj_t callback_obj) {
    mp_int_t pin = mp_obj_get_int(pin_obj);

    if (pin >= 1 && pin <= 2) {
        onReleaseCallback[pin - 1] = callback_obj != mp_const_none ? callback_obj : MP_OBJ_NULL;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(switch_release_obj, switch_release);

STATIC mp_obj_t switch_pressed(mp_obj_t pin_obj, mp_obj_t callback_obj) {
    mp_int_t pin = mp_obj_get_int(pin_obj);

    if (pin >= 1 && pin <= 3) {
        onPressedCallback[pin - 1] = callback_obj != mp_const_none ? callback_obj : MP_OBJ_NULL;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(switch_pressed_obj, switch_pressed);

STATIC const mp_rom_map_elem_t switch_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sensor) },
    { MP_ROM_QSTR(MP_QSTR_value), MP_ROM_PTR(&switch_value_obj) },
    { MP_ROM_QSTR(MP_QSTR_press), MP_ROM_PTR(&switch_press_obj) },
    { MP_ROM_QSTR(MP_QSTR_release), MP_ROM_PTR(&switch_release_obj) },
    { MP_ROM_QSTR(MP_QSTR_pressed), MP_ROM_PTR(&switch_pressed_obj) },

    { MP_ROM_QSTR(MP_QSTR_S1), MP_ROM_INT(0b01) },
    { MP_ROM_QSTR(MP_QSTR_S2), MP_ROM_INT(0b10) },
    { MP_ROM_QSTR(MP_QSTR_S12), MP_ROM_INT(0b11) },
};
STATIC MP_DEFINE_CONST_DICT(switch_module_globals, switch_module_globals_table);

const mp_obj_module_t switch_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&switch_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_switch, switch_user_cmodule, MODULE_SWITCH_ENABLED);
