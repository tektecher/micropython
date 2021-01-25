SDKCONFIG += boards/sdkconfig.base

FROZEN_MANIFEST ?= $(BOARD_DIR)/manifest.py


TFT_eSPI_DIR := $(BOARD_DIR)/TFT_eSPI

# Add our source files to the respective variables.
# SRC_USERMOD += $(TFT_eSPI_DIR)/examplemodule.c
# SRC_MOD_CXX += $(TFT_eSPI_DIR)/TFT_eSPI.cpp

# Add our module directory to the include path.
CFLAGS_MOD += -I$(TFT_eSPI_DIR) -DESP32
CXXFLAGS_MOD += -I$(TFT_eSPI_DIR)

# We use C++ features so have to link against the standard library.
LDFLAGS_USERMOD += -lstdc++
