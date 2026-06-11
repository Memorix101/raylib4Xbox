#**************************************************************************************************
#
#   raylib - Xbox (Original) via nxdk
#
#   Builds raylib as a static library for the Original Xbox.
#   Uses PLATFORM_XBOX (rcore_xbox.c) + GRAPHICS_API_OPENGL_SOFTWARE (rlsw).
#
#   Setup:
#       source /path/to/nxdk/bin/activate    # sets NXDK_DIR and PATH
#
#   Build the library:
#       make libraylib
#
#   Build an example XBE (set SRCS to your app's .c files):
#       make SRCS=my_game.c
#
#   Output:
#       bin/libraylib.lib    - static library to link against
#       bin/default.xbe      - executable (if SRCS is set)
#
#**************************************************************************************************

# --- Paths -------------------------------------------------------------------

# NXDK_DIR is set by 'source nxdk/bin/activate'; override if needed.
NXDK_DIR   ?= $(HOME)/nxdk/nxdk
RAYLIB_DIR ?= $(HOME)/raylib
OUTPUT_DIR ?= $(CURDIR)/bin

RAYLIB_SRC  = $(RAYLIB_DIR)/src

# --- nxdk Makefile integration -----------------------------------------------
# Must be set BEFORE including $(NXDK_DIR)/Makefile so nxdk picks them up.

XBE_TITLE = raylib-xbox
NXDK_SDL  = y          # include SDL2 for Xbox (display + controller)

# --- Compiler flags (appended to nxdk's NXDK_CFLAGS via CFLAGS) --------------

CFLAGS += \
    -DPLATFORM_XBOX \
    -DGRAPHICS_API_OPENGL_SOFTWARE \
    -DSUPPORT_GESTURES_SYSTEM \
    -DXBOX \
    -DSTBIR_NO_SIMD \
    -I$(CURDIR)/include \
    -I$(RAYLIB_SRC) \
    -I$(RAYLIB_SRC)/external \
    -std=gnu11 \
    -O2 \
    -Wno-unused-parameter \
    -Wno-sign-compare

# nxdk-cc already adds -I$(NXDK_DIR)/lib and -I$(NXDK_DIR)/lib/winapi,
# so <hal/video.h>, <windows.h>, <SDL.h> etc. resolve without extra -I flags.

# --- raylib source files -----------------------------------------------------

RAYLIB_SRCS = \
    $(RAYLIB_SRC)/rcore.c \
    $(RAYLIB_SRC)/rshapes.c \
    $(RAYLIB_SRC)/rtextures.c \
    $(RAYLIB_SRC)/rtext.c \
    $(RAYLIB_SRC)/rmodels.c \
    $(RAYLIB_SRC)/raudio.c

RAYLIB_OBJS = $(addsuffix .obj, $(basename $(RAYLIB_SRCS)))

# --- Pull in nxdk build system -----------------------------------------------
# This defines CC, LIB, NXDK_CFLAGS, the %.obj:%.c rule, etc.

include $(NXDK_DIR)/Makefile

# Apply xbox_compat.h only to raylib translation units, not to nxdk/pdclib sources.
# Target-specific variable: evaluated when make builds each of these .obj files.
$(RAYLIB_OBJS): CFLAGS += -include $(CURDIR)/include/xbox_compat.h

# --- Library target ----------------------------------------------------------

$(OUTPUT_DIR)/libraylib.lib: $(RAYLIB_OBJS) | $(OUTPUT_DIR)
	$(LIB) -out:'$@' $^

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# --- Convenience targets -----------------------------------------------------

.PHONY: libraylib clean-raylib

libraylib: $(OUTPUT_DIR)/libraylib.lib
	@echo "raylib for Xbox built: $<"

clean-raylib:
	rm -f $(RAYLIB_OBJS) $(OUTPUT_DIR)/libraylib.lib
