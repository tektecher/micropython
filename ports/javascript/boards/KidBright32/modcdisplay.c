#include "py/runtime.h"
#include <emscripten.h>

STATIC mp_obj_t cdisplay_raw(mp_obj_t data_obj) {
    // const char* data = mp_obj_str_get_str(data_obj);
    mp_buffer_info_t bufinfo;
    mp_get_buffer_raise(data_obj, &bufinfo, MP_BUFFER_READ);

    EM_ASM({
        const buf = $0;
        const len = $1;
        let data = [ ];
        for (let i=0;i<len;i++) {
            data.push(HEAPU8[buf + i]);
        }
        for (let i=0;i<len-16;i++) {
            data.push(0);
        }

        if (typeof simSystem !== "undefined") {
            try {
                simSystem.display.setData(data);
            } catch(err) {
                console.log(err);
            }
        } else {
            console.log("Display data", data);
        }
    }, bufinfo.buf, bufinfo.len);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(cdisplay_raw_obj, cdisplay_raw);

STATIC const mp_rom_map_elem_t cdisplay_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_cdisplay) },
    { MP_ROM_QSTR(MP_QSTR_raw), MP_ROM_PTR(&cdisplay_raw_obj) },
};
STATIC MP_DEFINE_CONST_DICT(cdisplay_module_globals, cdisplay_module_globals_table);

const mp_obj_module_t cdisplay_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&cdisplay_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_cdisplay, cdisplay_user_cmodule, MODULE_CDISPLAY_ENABLED);
