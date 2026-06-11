#**************************************************************************************************
#
#   raylib - Xbox sample: basic_window
#
#   Builds a bootable .xbe and .iso for the original Xbox (via nxdk).
#   Run in xemu with: File → Load Xbox ISO
#
#   Prerequisites:
#       1. source $(NXDK_DIR)/bin/activate
#       2. make -C ../../ libraylib          (builds bin/libraylib.lib)
#       3. make                              (builds this sample)
#
#**************************************************************************************************

XBE_TITLE   = raylib basic window
GEN_XISO    = $(XBE_TITLE).iso

NXDK_DIR   ?= $(HOME)/nxdk/nxdk
RAYLIB_DIR ?= $(HOME)/raylib
RAYLIB_SRC  = $(RAYLIB_DIR)/src

RAYLIB_LIB  = $(RAYLIB_DIR)/projects/Xbox/bin/libraylib.lib
COMPAT_DIR  = $(RAYLIB_DIR)/projects/Xbox/include

NXDK_SDL    = y

# Source files for this sample only (raylib is pre-built as a static lib)
SRCS = $(CURDIR)/main.c

# Compiler flags for the sample source.
# No -include xbox_compat.h here: main.c only calls the raylib public API
# and does not use MSVC CRT extensions. Keeping it out avoids polluting
# nxdk's own library sources (pdclib etc.) with the compat header.
CFLAGS += \
    -DPLATFORM_XBOX \
    -DGRAPHICS_API_OPENGL_SOFTWARE \
    -DSUPPORT_GESTURES_SYSTEM \
    -DXBOX \
    -I$(COMPAT_DIR) \
    -I$(RAYLIB_SRC) \
    -std=gnu11 \
    -O2

# Tell the linker to include our raylib static library.
LDFLAGS += $(RAYLIB_LIB)

# Pull in the full nxdk build system (provides CC, LD, rules for .xbe and .iso)
include $(NXDK_DIR)/Makefile

# Add libraylib.lib as a dependency of the final executable
main.exe: $(RAYLIB_LIB)

.PHONY: rebuild-raylib
rebuild-raylib:
	$(MAKE) -C $(RAYLIB_DIR)/projects/Xbox libraylib
