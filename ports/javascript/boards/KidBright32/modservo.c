#include "py/runtime.h"
#include <emscripten.h>

STATIC mp_obj_t servo_angle(mp_obj_t pin_obj, mp_obj_t angle_obj) {
    mp_int_t pin = mp_obj_get_int(pin_obj);
    mp_int_t angle = mp_obj_get_int(angle_obj);

    EM_ASM({
        const pin = $0;
        const angle = $1;
        console.log(`Set servo ${pin} to ${angle}`);
        if (typeof simSystem !== "undefined") {
            try {
                simSystem.servo.setAngle(pin, angle);
            } catch(err) {
                console.log(err);
            }
        }
    }, pin, angle);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(servo_angle_obj, servo_angle);

STATIC const mp_rom_map_elem_t servo_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_servo) },
    { MP_ROM_QSTR(MP_QSTR_angle), MP_ROM_PTR(&servo_angle_obj) },

    { MP_ROM_QSTR(MP_QSTR_SV1), MP_ROM_INT(0) },
    { MP_ROM_QSTR(MP_QSTR_SV2), MP_ROM_INT(1) },
};
STATIC MP_DEFINE_CONST_DICT(servo_module_globals, servo_module_globals_table);

const mp_obj_module_t servo_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&servo_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_servo, servo_user_cmodule, MODULE_SERVO_ENABLED);
