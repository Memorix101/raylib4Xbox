#**************************************************************************************************
#
#   raylib - Xbox (Original) via nxdk
#
#   Builds raylib as a static library for the Original Xbox.
#   Uses PLATFORM_XBOX (rcore_xbox.c) + GRAPHICS_API_OPENGL_11 (nxdk-gles11 NV2A hardware).
#
#   Setup:
#       source /path/to/nxdk/bin/activate    # sets NXDK_DIR and PATH
#
#   Build the library:
#       make libraylib
#
#   Output:
#       bin/libraylib.lib    - raylib static library
#       bin/libGLESv1_CM.lib - nxdk-gles11 OpenGL 1.1 on NV2A
#
#**************************************************************************************************

# --- Paths -------------------------------------------------------------------

NXDK_DIR    ?= $(HOME)/nxdk/nxdk
RAYLIB_DIR  ?= $(HOME)/raylib
GLES11_DIR  ?= $(HOME)/raylib4Xbox/lib/nxdk-gles11
OUTPUT_DIR  ?= $(CURDIR)/bin

RAYLIB_SRC   = $(RAYLIB_DIR)/src

# --- nxdk Makefile integration -----------------------------------------------

.DEFAULT_GOAL := libraylib

XBE_TITLE = raylib-xbox
NXDK_SDL  = y          # SDL2 for controller input

# --- Compiler flags ----------------------------------------------------------

CFLAGS += \
    -DPLATFORM_XBOX \
    -DGRAPHICS_API_OPENGL_11 \
    -DSUPPORT_GESTURES_SYSTEM \
    -DXBOX \
    -DSTBIR_NO_SIMD \
    -I$(CURDIR)/include \
    -I$(RAYLIB_SRC) \
    -I$(RAYLIB_SRC)/external \
    -I$(GLES11_DIR)/include \
    -I$(GLES11_DIR)/lib/xgu \
    -I$(GLES11_DIR)/lib/cglm/include \
    -I$(GLES11_DIR)/lib/stb \
    -I$(GLES11_DIR)/lib/swizzle \
    -std=gnu11 \
    -O2 \
    -Wno-unused-parameter \
    -Wno-sign-compare

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

include $(NXDK_DIR)/Makefile

# Apply xbox_compat.h only to raylib translation units, not to nxdk/pdclib sources.
$(RAYLIB_OBJS): CFLAGS += -include $(CURDIR)/include/xbox_compat.h

# rcore.c #includes the Xbox platform backend, so rebuild rcore.obj whenever
# the platform source or its dirent shim changes (Make can't see #include deps).
$(RAYLIB_SRC)/rcore.obj: $(RAYLIB_SRC)/platforms/rcore_xbox.c $(RAYLIB_SRC)/external/dirent_xbox.h

# --- Pull in nxdk-gles11 build -----------------------------------------------

include $(GLES11_DIR)/config_gl.make

# gles11 objects also need the compat header excluded — apply only to raylib objs
$(GLES11_OBJS): CFLAGS += $(GLES11_CFLAGS)

# --- Library targets ---------------------------------------------------------

$(OUTPUT_DIR)/libGLESv1_CM.lib: $(GLES11_OBJS) | $(OUTPUT_DIR)
	$(LIB) -out:'$@' $^

$(OUTPUT_DIR)/libraylib.lib: $(RAYLIB_OBJS) | $(OUTPUT_DIR)
	$(LIB) -out:'$@' $^

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# --- Convenience targets -----------------------------------------------------

.PHONY: libraylib clean-raylib

libraylib: $(OUTPUT_DIR)/libGLESv1_CM.lib $(OUTPUT_DIR)/libraylib.lib
	@echo "raylib for Xbox built (NV2A hardware renderer)"

clean-raylib:
	rm -f $(RAYLIB_OBJS) $(GLES11_OBJS) $(OUTPUT_DIR)/libraylib.lib $(OUTPUT_DIR)/libGLESv1_CM.lib
