CROSS = 0

QSTR_DEFS = qstrdefsport.h

# Select the board to build for: if not given on the command line,
# then default to GENERIC.
# BOARD ?= GENERIC
BOARD ?= KidBright32
# BOARD ?= IPST-WiFi

# If the build directory is not given, make it reflect the board name.
BUILD ?= build-$(BOARD)

BOARD_DIR ?= boards/$(BOARD)
ifeq ($(wildcard $(BOARD_DIR)/.),)
$(error Invalid BOARD specified: $(BOARD_DIR))
endif

include ../../py/mkenv.mk

# Expected to set SDKCONFIG
include $(BOARD_DIR)/mpconfigboard.mk

# qstr definitions (must come before including py.mk)
QSTR_DEFS = qstrdefsport.h
QSTR_GLOBAL_DEPENDENCIES = $(BOARD_DIR)/mpconfigboard.h
# QSTR_GLOBAL_REQUIREMENTS = $(SDKCONFIG_H)

# MicroPython feature configurations
# MICROPY_ROM_TEXT_COMPRESSION ?= 1
# MICROPY_PY_USSL = 0
# MICROPY_SSL_AXTLS = 0
# MICROPY_PY_BTREE = 1
# MICROPY_VFS_FAT = 1
# MICROPY_VFS_LFS2 = 1

# use boards/manifest.py if board not set
FROZEN_MANIFEST ?= boards/manifest.py

include $(TOP)/py/py.mk

CC = emcc -g4
LD = emcc -g4

INC += -I.
INC += -I$(TOP)
INC += -I$(BUILD)

CPP = clang -E

ifdef EMSDK
	EMSCRIPTEN = $(EMSDK)/upstream/emscripten
endif

ifdef EMSCRIPTEN
	CPP += -isystem $(EMSCRIPTEN)/system/include/libc
	CPP += -isystem $(EMSCRIPTEN)/system/lib/libc/musl/arch/emscripten
	CPP += -cxx-isystem $(EMSCRIPTEN)/system/include/libcxx
endif

INC += -I$(EMSCRIPTEN)/system/include/emscripten
INC += -I$(EMSCRIPTEN)/system/include

# CFLAGS = -m32 -Wall -Werror -Wdouble-promotion -Wfloat-conversion $(INC) -std=c99 $(COPT)
CFLAGS = -m32 -Wall -Werror -Wdouble-promotion -Wfloat-conversion $(INC) $(COPT)
# LDFLAGS = -m32 -Wl,-Map=$@.map,--cref -Wl,--gc-sections
LDFLAGS = 

CFLAGS += -O0 -DNDEBUG
CFLAGS += -fdata-sections -ffunction-sections

ifneq ($(FROZEN_MPY_DIR),)
# To use frozen bytecode, put your .py files in a subdirectory (eg frozen/) and
# then invoke make with FROZEN_MPY_DIR=frozen (be sure to build from scratch).
CFLAGS += -DMICROPY_QSTR_EXTRA_POOL=mp_qstr_frozen_const_pool
CFLAGS += -DMICROPY_MODULE_FROZEN_MPY
endif

CFLAGS += -I$(BOARD_DIR)

SRC_LIB = $(addprefix lib/,\
	utils/interrupt_char.c \
	utils/stdout_helpers.c \
	utils/pyexec.c \
	mp-readline/readline.c \
	)

SRC_C = \
	main.c \
	mphalport.c \
	modutime.c \
	moduos.c \
	$(wildcard $(BOARD_DIR)/*.c) \
	$(SRC_MOD)

SRC_QSTR += $(SRC_C)

OBJ = 
OBJ = $(PY_O) 
OBJ += $(addprefix $(BUILD)/, $(SRC_LIB:.c=.o))
OBJ += $(addprefix $(BUILD)/, $(SRC_C:.c=.o))

# List of sources for qstr extraction
# SRC_QSTR += $(SRC_C) $(SRC_CXX) $(EXTMOD_SRC_C) $(LIB_SRC_C) $(DRIVERS_SRC_C)
# Append any auto-generated sources that are needed by sources listed in SRC_QSTR
# SRC_QSTR_AUTO_DEPS +=

# JSFLAGS = -O0 -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_keyboard_interrupt']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" -s "BINARYEN_TRAP_MODE='clamp'" --memory-init-file 0 --js-library library.js
JSFLAGS = -O0 -s EXPORTED_FUNCTIONS="['_mp_js_init', '_mp_js_init_repl', '_mp_js_do_str', '_mp_js_process_char', '_mp_hal_get_interrupt_char', '_mp_keyboard_interrupt', '_mp_js_soft_rtc_tick', '_mp_switch_value_change_handle']" -s EXTRA_EXPORTED_RUNTIME_METHODS="['ccall', 'cwrap']" --memory-init-file 0 --js-library library.js

all: $(BUILD)/micropython.js

$(BUILD)/micropython.js: $(OBJ) library.js wrapper.js
	$(ECHO) "LINK $(BUILD)/firmware.js"
	$(Q)emcc $(LDFLAGS) -o $(BUILD)/firmware.js $(OBJ) $(JSFLAGS)
	cat wrapper.js $(BUILD)/firmware.js > $@

min: $(BUILD)/micropython.js
	uglifyjs $< -c -o $(BUILD)/micropython.min.js

test: $(BUILD)/micropython.js $(TOP)/tests/run-tests
	$(eval DIRNAME=ports/$(notdir $(CURDIR)))
	cd $(TOP)/tests && MICROPY_MICROPYTHON=../ports/javascript/node_run.sh ./run-tests

include $(TOP)/py/mkrules.mk
