#include "py/runtime.h"
#include <emscripten.h>

STATIC mp_obj_t sensor_light() {
    return mp_obj_new_int(EM_ASM_INT({
        if (typeof simSystem !== "undefined") {
            try {
                return simSystem.ldr.getValue();
            } catch(err) {
                console.log(err);
            }
        }

        return 0;
    }));
}
MP_DEFINE_CONST_FUN_OBJ_0(sensor_light_obj, sensor_light);

STATIC mp_obj_t sensor_temperature() {
    return mp_obj_new_float(EM_ASM_INT({
        if (typeof simSystem !== "undefined") {
            try {
                return simSystem.lm75.getValue() * 100;
            } catch(err) {
                console.log(err);
            }
        }

        return 0;
    }) / 100.0);
}
MP_DEFINE_CONST_FUN_OBJ_0(sensor_temperature_obj, sensor_temperature);

STATIC const mp_rom_map_elem_t sensor_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_sensor) },
    { MP_ROM_QSTR(MP_QSTR_light), MP_ROM_PTR(&sensor_light_obj) },
    { MP_ROM_QSTR(MP_QSTR_temperature), MP_ROM_PTR(&sensor_temperature_obj) },
};
STATIC MP_DEFINE_CONST_DICT(sensor_module_globals, sensor_module_globals_table);

const mp_obj_module_t sensor_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&sensor_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_sensor, sensor_user_cmodule, MODULE_SENSOR_ENABLED);
