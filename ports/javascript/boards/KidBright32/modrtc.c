#include "py/runtime.h"
#include <emscripten.h>
#include "library.h"

static uint8_t year = 0;
static uint8_t month = 0;
static uint8_t day = 0;
static uint8_t hour = 0;
static uint8_t minute = 0;
static uint8_t second = 0;

void mp_js_soft_rtc_tick() {
    second++;
    if (second >= 60) {
        second = 0;
        minute++;
    }
    if (minute >= 60) {
        minute = 0;
        hour++;
    }
    if (hour >= 24) {
        hour = 0;
        day++;
    }
    if (day >= 30) {
        day = 0;
        month++;
    }
    if (month >= 12) {
        month = 0;
        year++;
    }
    if (year >= 100) {
        year = 0;
    }
}

STATIC mp_obj_t rtc_datetime(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        mp_obj_t level[] = {
            mp_obj_new_int(year + 2000), // year
            mp_obj_new_int(month), // month
            mp_obj_new_int(day), // day
            mp_obj_new_int(hour), // hour
            mp_obj_new_int(minute), // minute
            mp_obj_new_int(second), // second
            mp_obj_new_int(0), // microsecond
            mp_obj_new_int(0), // tzinfo
        };
        return mp_obj_new_list(8, level);
    } else {
        mp_obj_t *datetime = NULL;
        size_t datetime_len = 0;
        mp_obj_get_array(args[0], &datetime_len, &datetime);

        if (datetime_len >= 1) year = mp_obj_get_int(datetime[0]) % 100;
        if (datetime_len >= 2) month = mp_obj_get_int(datetime[1]) % 12;
        if (datetime_len >= 3) day = mp_obj_get_int(datetime[2]) % 32;
        if (datetime_len >= 4) hour = mp_obj_get_int(datetime[3]) % 24;
        if (datetime_len >= 5) minute = mp_obj_get_int(datetime[4]) % 60;
        if (datetime_len >= 6) second = mp_obj_get_int(datetime[5]) % 60;
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(rtc_datetime_obj, 0, 1, rtc_datetime);

STATIC const mp_rom_map_elem_t rtc_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_rtc) },
    { MP_ROM_QSTR(MP_QSTR_datetime), MP_ROM_PTR(&rtc_datetime_obj) },
};
STATIC MP_DEFINE_CONST_DICT(rtc_module_globals, rtc_module_globals_table);

const mp_obj_module_t rtc_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&rtc_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_rtc, rtc_user_cmodule, MODULE_RTC_ENABLED);
