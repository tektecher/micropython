#include <stdio.h>
#include <string.h>

#include "py/runtime.h"
#include <emscripten.h>
#include "mphalport.h"

EM_JS(void, js_audio, (int freq, int duty), {
    if (typeof AudioContext === "undefined") {
        console.log("Debug Buzzer: Freq", freq, "Duty", duty);
        return;
    }

    if (typeof simPlayNoteContext === "undefined") {
        simPlayNoteContext = new AudioContext();
    }

    if (typeof simPlayNoteOscillator === "undefined") {
        simPlayNoteOscillator = null;
    }

    if (simPlayNoteOscillator) {
        simPlayNoteOscillator.stop();
        simPlayNoteOscillator = null;
    }

    if (typeof simSystem !== "undefined") {
        simSystem.buzzer.setStatus(duty !== 0);
    }

    if (duty === 0) {
        return;
    }

    simPlayNoteOscillator = simPlayNoteContext.createOscillator();
    let playNoteGain = simPlayNoteContext.createGain();
    playNoteGain.gain.value = duty / 512;
    simPlayNoteOscillator.type = "square";
    simPlayNoteOscillator.frequency.value = freq;
    simPlayNoteOscillator.connect(playNoteGain);
    playNoteGain.connect(simPlayNoteContext.destination);
    simPlayNoteOscillator.start();
});

uint8_t volume = 50;
uint16_t bpm = 120;

STATIC mp_obj_t buzzer_tone(mp_obj_t freq_obj, mp_obj_t duration_obj) {
    mp_int_t freq = mp_obj_get_int(freq_obj);
    mp_float_t duration = mp_obj_get_float(duration_obj);

    js_audio(freq, (int)(volume / 100.0 * 512.0));
    mp_hal_delay_ms((mp_uint_t)(duration * 1000.0));
    js_audio(freq, 0);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(buzzer_tone_obj, buzzer_tone);

STATIC mp_obj_t buzzer_on(mp_obj_t freq_obj) {
    mp_int_t freq = mp_obj_get_int(freq_obj);

    js_audio(freq, (int)(volume / 100.0 * 512.0));

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_1(buzzer_on_obj, buzzer_on);

STATIC mp_obj_t buzzer_off() {
    js_audio(0, 0);

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_0(buzzer_off_obj, buzzer_off);

const char *note_map_note[] = {
    "C4",
	"C#4",
	"D4",
	"Eb4",
	"E4",
	"F4",
	"F#4",
	"G4",
	"G#4",
	"A4",
	"Bb4",
	"B4",
	"C5",
	"C#5",
	"D5",
	"Eb5",
	"E5",
	"F5",
	"F#5",
	"G5",
	"G#5",
	"A5",
	"Bb5",
	"B5",
	"C6",
	"C#6",
	"D6",
	"Eb6",
	"E6", 
	"F6",
	"F#6",
	"G6",
	"G#6",
	"A6",
	"Bb6",
	"B6",
	"C7",
    "SIL"
};

const int note_map_freq[] = {
    261,
	277,
	293,
	311,
	329,
	349,
	369,
	391,
	415,
	440,
	466,
	493,
	523,
	554,
	587,
	622,
	659,
	698,
	740,
	784,
	831,
	880,
	932,
	988,
	1046,
	1109,
	1175,
	1244,
	1318, 
	1396,
	1480,
	1568,
	1661,
	1760,
	1865,
	1976,
	2093,
    0
};

STATIC mp_obj_t buzzer_note(mp_obj_t notes_obj, mp_obj_t duration_obj) {
    const char* notes = mp_obj_str_get_str(notes_obj);
    mp_float_t duration = mp_obj_get_float(duration_obj);

    double quarter_delay = (60.0 * 1000.0) / bpm;
    double delay = quarter_delay * duration;
    delay = delay / 1000; // mS -> S

    char note[10];
    int noteIndex = 0;
    int notesLen = strlen(notes);
    for (int strIndex=0;strIndex<notesLen;strIndex++) {
        char c = notes[strIndex];
        if ((c == ' ') || (strIndex == (notesLen - 1))) {
            if (strIndex == (notesLen - 1)) {
                note[noteIndex] = c;
                noteIndex++;
            }
            note[noteIndex] = 0; // null terminated 

            // Find notes index
            int noteFreqIndex = 999;
            for (uint8_t i=0;i<(sizeof(note_map_freq) / sizeof(int));i++) {
                if (strcmp(note, note_map_note[i]) == 0) {
                    noteFreqIndex = i;
                    break;
                }
            }
            if (noteFreqIndex != 999) {
                buzzer_tone(mp_obj_new_int(note_map_freq[noteFreqIndex]), mp_obj_new_float(delay));
            }

            noteIndex = 0;
        } else if (noteIndex < 10) {
            note[noteIndex] = c;
            noteIndex++;
        }
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(buzzer_note_obj, buzzer_note);

STATIC mp_obj_t buzzer_volume(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int((mp_int_t)volume);
    } else {
        volume = mp_obj_get_int(args[0]);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(buzzer_volume_obj, 0, 1, buzzer_volume);

STATIC mp_obj_t buzzer_bpm(size_t n_args, const mp_obj_t *args) {
    if (n_args == 0) {
        return mp_obj_new_int((mp_int_t)bpm);
    } else {
        bpm = mp_obj_get_int(args[0]);
    }

    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(buzzer_bpm_obj, 0, 1, buzzer_bpm);

STATIC const mp_rom_map_elem_t buzzer_module_globals_table[] = {
    { MP_ROM_QSTR(MP_QSTR___name__), MP_ROM_QSTR(MP_QSTR_buzzer) },
    { MP_ROM_QSTR(MP_QSTR_tone), MP_ROM_PTR(&buzzer_tone_obj) },
    { MP_ROM_QSTR(MP_QSTR_on), MP_ROM_PTR(&buzzer_on_obj) },
    { MP_ROM_QSTR(MP_QSTR_off), MP_ROM_PTR(&buzzer_off_obj) },
    { MP_ROM_QSTR(MP_QSTR_note), MP_ROM_PTR(&buzzer_note_obj) },

    { MP_ROM_QSTR(MP_QSTR_volume), MP_ROM_PTR(&buzzer_volume_obj) },
    { MP_ROM_QSTR(MP_QSTR_bpm), MP_ROM_PTR(&buzzer_bpm_obj) },
};
STATIC MP_DEFINE_CONST_DICT(buzzer_module_globals, buzzer_module_globals_table);

const mp_obj_module_t buzzer_user_cmodule = {
    .base = { &mp_type_module },
    .globals = (mp_obj_dict_t *)&buzzer_module_globals,
};

MP_REGISTER_MODULE(MP_QSTR_buzzer, buzzer_user_cmodule, MODULE_BUZZER_ENABLED);
