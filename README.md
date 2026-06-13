# raylib4Xbox

OpenGL support (OpenGL ES 1.1)

https://github.com/user-attachments/assets/ed6e8d39-9e37-4955-b455-0198676f1c6b

Software Renderer - [rlsw](https://github.com/Memorix101/raylib4Xbox/tree/rlsw)

https://github.com/user-attachments/assets/cbfb2e8a-83b4-4344-a80f-6f06a1b7c860

**[raylib](https://github.com/raysan5/raylib)** running on the **Original Xbox** via [nxdk](https://github.com/XboxDev/nxdk).

Produces bootable `.iso` images that run in [xemu](https://xemu.app/) and on real hardware.

---

## How it works

| Layer | What it does |
|---|---|
| **raylib** | Handles all drawing, audio and input abstraction |
| **`PLATFORM_XBOX`** (`rcore_xbox.c`) | New platform backend: nxdk-gles11 for display, SDL2 for gamepad input |
| **`GRAPHICS_API_OPENGL_11`** | raylib's OpenGL 1.1 backend (rlgl), running on real GPU hardware |
| **[nxdk-gles11](https://github.com/Memorix101/nxdk-gles11)** | OpenGL ES 1.1 + GL 1.1 immediate mode on the NV2A GPU via `pbkit` pushbuffers |
| **SDL2 (nxdk)** | nxdk's Xbox SDL2 port (controller input only, no video) |
| **nxdk** | Toolchain and runtime: clang cross-compiler, pdclib, XBE packaging, xiso creation |

raylib's rlgl issues standard OpenGL 1.1 calls; nxdk-gles11 translates them into NV2A pushbuffer commands, so all rendering runs on the Xbox GPU. Each frame, `SwapScreenBuffer()` calls `glFlipNV2A()`, which waits for vblank and flips the hardware framebuffer.

---

## Requirements

| Tool | Version | Notes |
|---|---|---|
| [nxdk](https://github.com/XboxDev/nxdk) | latest | Must be cloned and built (`make`) |
| clang | >= 10.x | Ships with nxdk |
| lld-link | any | Ships with nxdk |
| [raylib4Consoles fork](https://github.com/raylib4Consoles/raylib) | branch `raylib4Consoles_6.0` | This repo's patches target that fork |
| xemu | any | For testing ISOs on PC |

nxdk must be bootstrapped first:
```sh
git clone --recurse-submodules https://github.com/XboxDev/nxdk.git
cd nxdk
make
```

---

## Setup

### Clone the raylib4Consoles fork

```sh
git clone --recurse-submodules https://github.com/raylib4Consoles/raylib.git
cd raylib
git checkout raylib4Consoles_6.0
```

### Apply the Xbox patch to raylib core

Two raylib core files need small edits to exclude Win32-only APIs when targeting Xbox:

```sh
git apply /path/to/raylib4Xbox/raylib_xbox.patch
```

What the patch does:
- **`src/rcore.c`**: adds a `PLATFORM_XBOX` branch in 6 places, includes `rcore_xbox.c`, routes to `dirent_xbox.h`, and disables `GetModuleFileNameA`, `timeBeginPeriod`, and the `Sleep` dllimport declaration
- **`src/raudio.c`**: guards the `#include <objbase.h>` (WASAPI/COM) block with `!defined(PLATFORM_XBOX)` because those headers don't exist in nxdk, and sets the device sample rate to 48 000 Hz (the native AC97 rate, which avoids resampling)
- **`src/external/miniaudio.h`**: fixes the XAudio (NXDK) backend's device start; upstream starts the AC97 DMA with empty descriptors, which crashes real hardware, so the patch pre-fills all sub-buffers before `XAudioPlay()`

### Copy Xbox-specific source files into raylib

```sh
cp src/platforms/rcore_xbox.c  /path/to/raylib/src/platforms/
cp src/external/dirent_xbox.h  /path/to/raylib/src/external/
```

### Get nxdk-gles11

The GPU renderer lives in the `lib/nxdk-gles11` submodule:

```sh
git clone --recurse-submodules https://github.com/Memorix101/raylib4Xbox.git
# or, in an existing checkout:
git submodule update --init
```

### Copy the build system

```sh
cp -r projects/Xbox  /path/to/raylib/projects/
```

The Makefiles locate nxdk-gles11 via `GLES11_DIR` (default: `$(HOME)/raylib4Xbox/lib/nxdk-gles11`). Override it if your checkout lives elsewhere:

```sh
make -C /path/to/raylib/projects/Xbox libraylib GLES11_DIR=/path/to/raylib4Xbox/lib/nxdk-gles11
```

### Activate nxdk environment

```sh
# One-time: export env vars (the activate script calls exec and replaces your shell,
# so set variables manually instead of sourcing it)
export NXDK_DIR=/path/to/nxdk
export PATH="$NXDK_DIR/bin:$PATH"
```

### Build libraylib

```sh
make -C /path/to/raylib/projects/Xbox libraylib
# Output: projects/Xbox/bin/libraylib.lib     (raylib)
#         projects/Xbox/bin/libGLESv1_CM.lib  (nxdk-gles11 GPU renderer)
```

---

## Building a sample

```sh
# All samples ship with a Makefile that links against the pre-built libraylib.lib
export NXDK_DIR=/path/to/nxdk
export PATH="$NXDK_DIR/bin:$PATH"

make -C samples/basic_window
# Output: samples/basic_window/window.iso
```

Load the `.iso` in xemu via **File > Load Xbox ISO**, or burn/inject onto real hardware.
To output debug text in terminal run xemu with `./xemu.exe -device lpc47m157 -serial stdio`

---

## Samples

| Sample | ISO | Description |
|---|---|---|
| `basic_window` | `window.iso` | Minimal raylib window, FPS counter, exit with **BACK** |
| `sprite_animation` | `animation.iso` | Sprite sheet animation, **DPAD** changes speed |
| `cubicmap` | `cubicmap.iso` | 3D cubicmap with orbital camera, **A** pauses rotation |
| `audio_music` | `music.iso` | Music stream playback, **A** pause, **B** restart, **DPAD** for volume and pan |

All samples exit and reboot to the Xbox dashboard when **BACK** is pressed.

---

## Controller mapping

The Xbox controller maps to raylib's `GAMEPAD_BUTTON_*` constants:

| Xbox button | raylib constant |
|---|---|
| A | `GAMEPAD_BUTTON_RIGHT_FACE_DOWN` |
| B | `GAMEPAD_BUTTON_RIGHT_FACE_RIGHT` |
| X | `GAMEPAD_BUTTON_RIGHT_FACE_LEFT` |
| Y | `GAMEPAD_BUTTON_RIGHT_FACE_UP` |
| White | `GAMEPAD_BUTTON_LEFT_TRIGGER_1` |
| Black | `GAMEPAD_BUTTON_RIGHT_TRIGGER_1` |
| Left trigger | `GAMEPAD_AXIS_LEFT_TRIGGER` / `GAMEPAD_BUTTON_LEFT_TRIGGER_2` |
| Right trigger | `GAMEPAD_AXIS_RIGHT_TRIGGER` / `GAMEPAD_BUTTON_RIGHT_TRIGGER_2` |
| Left stick click | `GAMEPAD_BUTTON_LEFT_THUMB` |
| Right stick click | `GAMEPAD_BUTTON_RIGHT_THUMB` |
| DPAD | `GAMEPAD_BUTTON_LEFT_FACE_*` |
| Back | `GAMEPAD_BUTTON_MIDDLE_LEFT` |
| Start | `GAMEPAD_BUTTON_MIDDLE_RIGHT` |
| Left stick | `GAMEPAD_AXIS_LEFT_X` / `GAMEPAD_AXIS_LEFT_Y` |
| Right stick | `GAMEPAD_AXIS_RIGHT_X` / `GAMEPAD_AXIS_RIGHT_Y` |

Rumble is supported via `SDL_GameControllerRumble`: call `SetGamepadVibration(gamepad, leftMotor, rightMotor, duration)`.

---

## Supported resolutions

| Resolution | Standard |
|---|---|
| 640 x 480 | NTSC (default) |
| 720 x 480 | NTSC widescreen |

Pass the desired resolution to `InitWindow()`. The backend calls `XVideoSetMode()` accordingly.

---

## Known limitations

- **Audio output is 48 000 Hz stereo**: nxdk's XAudio HAL drives the AC97 codec at s16 stereo 48 000 Hz. raylib mixes in f32 as usual; miniaudio converts to s16 for the hardware. Sources at other sample rates (e.g. 44.1 kHz MP3s) are resampled on the fly.
- **OpenGL 1.1 fixed-function only**: the NV2A is a fixed-function-era GPU, so raylib features that need shaders (`GRAPHICS_API_OPENGL_33`+) are not available. No mipmaps or FBOs yet (see nxdk-gles11 TODO).
- **No keyboard or mouse**: Xbox has no keyboard/mouse support. All input is via gamepad.
- **File paths must use `D:\`**: resources on the ISO are accessible under `D:\resources\filename`. Use backslash paths or pass the full path when loading assets.
- **`fopen()` is binary-only**: nxdk's CRT rejects the text mode flag, so `fopen(path, "rt")` returns `NULL` while `"rb"` works. raylib's `LoadFileText()` uses text mode, which silently broke OBJ/MTL loading, so `PLATFORM_XBOX` registers a binary `LoadFileText` callback in `rcore_xbox.c`. Always open files with `"rb"`/`"wb"`.
- **No `chdir()`**: nxdk's `_chdir()` only accepts `D:\` and aborts (Fatal System Error) on any other path. The OBJ loader's directory switch is disabled for `PLATFORM_XBOX`; load assets with absolute `D:\` paths instead of relying on a working directory.
- **Floating-point string functions**: `atof`, `strtof` and `strtod` are not usable on nxdk - pdclib only declares `atof`, and libxboxrt's `strtod`/`strtof` are `assert(0)` stubs that crash at runtime. `xbox_compat.h` provides a self-contained `xbox_strtod()` and routes `atof`/`strtod`/`strtof` to it, so text parsers that read floats (e.g. cgltf parsing glTF JSON) work. tinyobj is unaffected because it has its own float parser.

---

## Repository structure

```
raylib4Xbox/
├── raylib_xbox.patch           # Minimal patch for raylib's rcore.c and raudio.c
├── lib/
│   └── nxdk-gles11/            # Submodule: OpenGL 1.1 on the NV2A GPU
├── src/
│   ├── platforms/
│   │   └── rcore_xbox.c        # Xbox platform backend (display, input, timing)
│   └── external/
│       └── dirent_xbox.h       # POSIX dirent shim using nxdk's FindFirstFileA
├── projects/
│   └── Xbox/
│       ├── Makefile             # Builds libraylib.lib + libGLESv1_CM.lib via nxdk toolchain
│       └── include/
│           ├── xbox_compat.h   # MSVC CRT shims (fopen_s, atof, stricmp, ...)
│           ├── io.h            # Stub for <io.h> (_access)
│           ├── stat.h          # Redirect for miniaudio's #include <stat.h>
│           └── sys/
│               └── stat.h      # Stub for <sys/stat.h>
└── samples/
    ├── basic_window/           # Hello World
    ├── sprite_animation/       # Spritesheet animation with gamepad speed control
    ├── cubicmap/               # 3D tile map with orbital camera
    └── audio_music/            # Music stream: country.mp3 with volume/pan control
```

---

## Technical notes

### Why a new `PLATFORM_XBOX` instead of `PLATFORM_DESKTOP_WIN32`?

nxdk emulates the Windows ABI (i386-pc-win32 clang target) but provides none of the Win32 windowing APIs (`CreateWindowEx`, `GetDC`, `wglCreateContext`, etc.). `PLATFORM_DESKTOP_WIN32` needs all of these, so a dedicated platform file avoids patching the existing desktop backend.

### How OpenGL runs on the NV2A

The Xbox's NV2A GPU has a proprietary pushbuffer interface exposed via `pbkit`. It is not OpenGL-compatible at the API level. [nxdk-gles11](https://github.com/Memorix101/nxdk-gles11) bridges that gap by implementing OpenGL ES 1.1 plus the desktop GL 1.1 immediate-mode subset that raylib's rlgl backend needs, translating GL state and draw calls into NV2A pushbuffer commands.

Two NV2A quirks the library handles transparently:
- The GPU reads vertex arrays by **physical address**, so client arrays from regular heap memory are staged per draw call into physically contiguous write-combined memory.
- NPOT textures use unnormalized coordinates; the hardware texture matrix is kept in sync with the bound texture's dimensions.

A software-rendered variant of this port (rlsw, no GPU) lives on the [`rlsw` branch](https://github.com/Memorix101/raylib4Xbox/tree/rlsw).

### The `-include xbox_compat.h` scope problem

When nxdk's Makefile compiles all sources with the same `$(CFLAGS)`, a global `-include xbox_compat.h` would also reach pdclib's internal files and trigger a `__STDC_WANT_LIB_EXT1__` redefinition error. The Makefile solves this with a GNU Make target-specific variable:

```makefile
$(RAYLIB_OBJS): CFLAGS += -include $(CURDIR)/include/xbox_compat.h
```

This applies the force-include only to the six raylib translation units, not to nxdk's own library sources.

---

## Credits

- [raylib](https://www.raylib.com/) by Ramon Santamaria (@raysan5)
- [raylib4Consoles](https://github.com/raylib4Consoles) for the multi-console raylib fork
- [nxdk](https://github.com/XboxDev/nxdk) by the XboxDev community
- [xemu](https://xemu.app/) Original Xbox emulator
- [nxdk-gles11](https://github.com/Memorix101/nxdk-gles11) forked from [nxdk-gles11 by Ryzee119](https://github.com/Ryzee119/nxdk-gles11)
