# raylib4Xbox

**[raylib](https://github.com/raysan5/raylib)** running on the **Original Xbox** via [nxdk](https://github.com/XboxDev/nxdk)

Produces bootable `.iso` images that run in [xemu](https://xemu.app/) and on real hardware.

---

## How it works

| Layer | What it does |
|---|---|
| **raylib** | Handles all drawing, audio, input abstraction |
| **`PLATFORM_XBOX`** (`rcore_xbox.c`) | New platform backend: SDL2 for display/events, gamepad mapping |
| **`GRAPHICS_API_OPENGL_SOFTWARE`** | raylib's built-in software renderer (`rlsw`) — no GPU required |
| **SDL2 (nxdk)** | nxdk's Xbox SDL2 port; uses `pbkit` (pushbuffer DMA) under the hood to blit pixels |
| **nxdk** | Toolchain + runtime: clang cross-compiler, pdclib, XBE packaging, xiso creation |

The software renderer draws into a pixel buffer. Each frame, `SwapScreenBuffer()` uploads that buffer to an SDL2 streaming texture and blits it to the Xbox framebuffer.

---

## Requirements

| Tool | Version | Notes |
|---|---|---|
| [nxdk](https://github.com/XboxDev/nxdk) | latest | Must be cloned and built (`make`) |
| clang | ≥ 10.x | Ships with nxdk |
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
- **`src/rcore.c`**: adds `PLATFORM_XBOX` branch in 6 places — includes `rcore_xbox.c`, routes to `dirent_xbox.h`, disables `GetModuleFileNameA`, `timeBeginPeriod`, and the `Sleep` dllimport declaration
- **`src/raudio.c`**: guards the `#include <objbase.h>` (WASAPI/COM) block with `!defined(PLATFORM_XBOX)` — these headers don't exist in nxdk

### Copy Xbox-specific source files into raylib

```sh
cp src/platforms/rcore_xbox.c  /path/to/raylib/src/platforms/
cp src/external/dirent_xbox.h  /path/to/raylib/src/external/
```

### Copy the build system

```sh
cp -r projects/Xbox  /path/to/raylib/projects/
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
# Output: projects/Xbox/bin/libraylib.lib
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

Load the `.iso` in xemu via **File → Load Xbox ISO**, or burn/inject onto real hardware.

---

## Samples

| Sample | ISO | Description |
|---|---|---|
| `basic_window` | `window.iso` | Minimal raylib window, FPS counter, exit with **BACK** |
| `sprite_animation` | `animation.iso` | Sprite sheet animation — **DPAD** changes speed |
| `cubicmap` | `cubicmap.iso` | 3D cubicmap with orbital camera — **A** pauses rotation |
| `audio_music` | `music.iso` | Music stream playback — **A** pause, **B** restart, **DPAD** volume & pan |

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

Rumble: `SetGamepadVibration(gamepad, leftMotor, rightMotor, duration)` is supported via `SDL_GameControllerRumble`.

---

## Supported resolutions

| Resolution | Standard |
|---|---|
| 640 × 480 | NTSC (default) |
| 720 × 480 | NTSC widescreen |

Pass the desired resolution to `InitWindow()`. The backend calls `XVideoSetMode()` accordingly.

---

## Known limitations

- **Audio is s16 only** — nxdk's XAudio HAL outputs signed 16-bit stereo at 48 000 Hz. The patch overrides `AUDIO_DEVICE_FORMAT` to `ma_format_s16` and `AUDIO_DEVICE_SAMPLE_RATE` to `48000` for `PLATFORM_XBOX`.
- **Software rendering only** — The Xbox GPU (NV2A) is not used for rendering. Expect lower performance for complex 3D scenes compared to hardware OpenGL.
- **No keyboard or mouse** — Xbox has no keyboard/mouse support. All input is via gamepad.
- **File paths must use `D:\`** — Resources on the ISO are accessible under `D:\resources\filename`. Use backslash paths or pass the full path when loading assets.
- **Floating-point string functions** — `atof()` and `strtof()` are missing from pdclib; they are shimmed to `strtod()` in `xbox_compat.h`.

---

## Repository structure

```
raylib4Xbox/
├── raylib_xbox.patch           # Minimal patch for raylib's rcore.c and raudio.c
├── src/
│   ├── platforms/
│   │   └── rcore_xbox.c        # Xbox platform backend (display, input, timing)
│   └── external/
│       └── dirent_xbox.h       # POSIX dirent shim using nxdk's FindFirstFileA
├── projects/
│   └── Xbox/
│       ├── Makefile             # Builds libraylib.lib via nxdk toolchain
│       └── include/
│           ├── xbox_compat.h   # MSVC CRT shims (fopen_s, atof, stricmp, …)
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

nxdk emulates the Windows ABI (i386-pc-win32 clang target) but provides none of the Win32 windowing APIs (`CreateWindowEx`, `GetDC`, `wglCreateContext`, etc.). `PLATFORM_DESKTOP_WIN32` needs all of these. A dedicated platform file avoids patching the existing desktop backend.

### Why the software renderer?

The Xbox's NV2A GPU has a proprietary pushbuffer interface exposed via `pbkit`. It is not compatible with OpenGL or Vulkan at the API level. raylib's software renderer (`rlsw`) implements OpenGL 1.1 semantics in software and outputs a pixel buffer — exactly what is needed here.

### The `-include xbox_compat.h` scope problem

When nxdk's Makefile compiles all sources with the same `$(CFLAGS)`, a global `-include xbox_compat.h` would also reach pdclib's internal files and trigger a `__STDC_WANT_LIB_EXT1__` redefinition error. The Makefile solves this with a GNU Make target-specific variable:

```makefile
$(RAYLIB_OBJS): CFLAGS += -include $(CURDIR)/include/xbox_compat.h
```

This applies the force-include only to the six raylib translation units, not to nxdk's own library sources.

---

## Credits

- [raylib](https://www.raylib.com/) by Ramon Santamaria (@raysan5)
- [raylib4Consoles](https://github.com/raylib4Consoles) — multi-console raylib fork
- [nxdk](https://github.com/XboxDev/nxdk) — open-source Xbox toolchain by the XboxDev community
- [xemu](https://xemu.app/) — Original Xbox emulator
