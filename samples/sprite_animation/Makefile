#**************************************************************************************************
#
#   raylib - Xbox sample: sprite animation
#
#   Assets at resources/ are copied into bin/resources/ so extract-xiso
#   packages them into the ISO alongside default.xbe.
#   On Xbox the files are accessible at D:\resources\<name>.
#
#**************************************************************************************************

XBE_TITLE   = raylib sprite animation
GEN_XISO    = $(XBE_TITLE).iso

NXDK_DIR   ?= $(HOME)/nxdk/nxdk
RAYLIB_DIR ?= $(HOME)/raylib
RAYLIB_SRC  = $(RAYLIB_DIR)/src

RAYLIB_LIB  = $(RAYLIB_DIR)/projects/Xbox/bin/libraylib.lib
COMPAT_DIR  = $(RAYLIB_DIR)/projects/Xbox/include

NXDK_SDL    = y

SRCS = $(CURDIR)/main.c

CFLAGS += \
    -DPLATFORM_XBOX \
    -DGRAPHICS_API_OPENGL_11 \
    -DSUPPORT_GESTURES_SYSTEM \
    -DXBOX \
    -I$(COMPAT_DIR) \
    -I$(RAYLIB_SRC) \
    -std=gnu11 \
    -O2

LDFLAGS += $(RAYLIB_LIB)
GLES11_DIR ?= $(HOME)/raylib4Xbox/lib/nxdk-gles11
GLES11_LIB  = $(RAYLIB_DIR)/projects/Xbox/bin/libGLESv1_CM.lib
LDFLAGS    += $(GLES11_LIB)

include $(NXDK_DIR)/Makefile

main.exe: $(RAYLIB_LIB)

# Copy resources into bin/ before ISO is packaged.
# Uses an order-only prerequisite so extract-xiso always sees them.
$(GEN_XISO): | $(OUTPUT_DIR)/resources

$(OUTPUT_DIR)/resources: | $(OUTPUT_DIR)
	mkdir -p $@
	cp -r $(CURDIR)/resources/. $@/

.PHONY: rebuild-raylib
rebuild-raylib:
	$(MAKE) -C $(RAYLIB_DIR)/projects/Xbox libraylib
