/**********************************************************************************************
*
*   rcore_xbox - Functions to manage window, graphics device and inputs
*
*   PLATFORM: XBOX (Original) via nxdk
*       - Original Xbox (nxdk toolchain)
*
*   LIMITATIONS:
*       - Fixed 640x480 display (NTSC) or 720x480 (NTSC widescreen)
*       - No keyboard or mouse support
*       - Software renderer only (GRAPHICS_API_OPENGL_SOFTWARE / rlsw)
*
*   DEPENDENCIES:
*       - nxdk: hal/video.h, hal/xbox.h, hal/debug.h
*       - SDL2 (nxdk's Xbox SDL2 port) for display output and controller input
*       - rlsw: raylib's software OpenGL 1.1 implementation
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2013-2026 Ramon Santamaria (@raysan5) and contributors
*
**********************************************************************************************/

#include <hal/xbox.h>
#include <hal/video.h>
#include <hal/debug.h>
#include <SDL.h>
#include <windows.h>    // nxdk minimal Win32 stubs (QueryPerformanceCounter etc.)

#include <stdlib.h>
#include <string.h>

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef struct {
    SDL_Window      *window;
    SDL_Renderer    *renderer;
    SDL_Texture     *texture;
    unsigned int    *pixels;            // RGBA8888 pixel buffer (from software renderer)

    SDL_GameController *gamepad[MAX_GAMEPADS];
    SDL_JoystickID      gamepadId[MAX_GAMEPADS];

    LARGE_INTEGER timerFrequency;
} PlatformData;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
extern CoreData CORE;

static PlatformData platform = { 0 };

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
int InitPlatform(void);
bool InitGraphicsDevice(void);

static int GetGamepadSlot(SDL_JoystickID jid);

//----------------------------------------------------------------------------------
// Module Functions Definition: Window and Graphics Device
//----------------------------------------------------------------------------------

bool WindowShouldClose(void)
{
    if (CORE.Window.ready) return CORE.Window.shouldClose;
    else return true;
}

void ToggleFullscreen(void)      { TRACELOG(LOG_WARNING, "ToggleFullscreen() not available on Xbox"); }
void ToggleBorderlessWindowed(void) { TRACELOG(LOG_WARNING, "ToggleBorderlessWindowed() not available on Xbox"); }
void MaximizeWindow(void)        { TRACELOG(LOG_WARNING, "MaximizeWindow() not available on Xbox"); }
void MinimizeWindow(void)        { TRACELOG(LOG_WARNING, "MinimizeWindow() not available on Xbox"); }
void RestoreWindow(void)         { TRACELOG(LOG_WARNING, "RestoreWindow() not available on Xbox"); }

void SetWindowState(unsigned int flags) { TRACELOG(LOG_WARNING, "SetWindowState() not available on Xbox"); }
void ClearWindowState(unsigned int flags) { TRACELOG(LOG_WARNING, "ClearWindowState() not available on Xbox"); }

void SetWindowIcon(Image image)  { TRACELOG(LOG_WARNING, "SetWindowIcon() not available on Xbox"); }
void SetWindowIcons(Image *images, int count) { TRACELOG(LOG_WARNING, "SetWindowIcons() not available on Xbox"); }

void SetWindowTitle(const char *title)
{
    CORE.Window.title = title;
    if (platform.window) SDL_SetWindowTitle(platform.window, title);
}

void SetWindowPosition(int x, int y)  { TRACELOG(LOG_WARNING, "SetWindowPosition() not available on Xbox"); }
void SetWindowMonitor(int monitor)    { TRACELOG(LOG_WARNING, "SetWindowMonitor() not available on Xbox"); }

void SetWindowMinSize(int width, int height)
{
    CORE.Window.screenMin.width  = width;
    CORE.Window.screenMin.height = height;
}

void SetWindowMaxSize(int width, int height)
{
    CORE.Window.screenMax.width  = width;
    CORE.Window.screenMax.height = height;
}

void SetWindowSize(int width, int height) { TRACELOG(LOG_WARNING, "SetWindowSize() not available on Xbox"); }
void SetWindowOpacity(float opacity)      { TRACELOG(LOG_WARNING, "SetWindowOpacity() not available on Xbox"); }
void SetWindowFocused(void)               { TRACELOG(LOG_WARNING, "SetWindowFocused() not available on Xbox"); }

void *GetWindowHandle(void) { return NULL; }

int GetMonitorCount(void)          { return 1; }
int GetCurrentMonitor(void)        { return 0; }

Vector2 GetMonitorPosition(int monitor) { return (Vector2){ 0, 0 }; }

int GetMonitorWidth(int monitor)   { return CORE.Window.screen.width; }
int GetMonitorHeight(int monitor)  { return CORE.Window.screen.height; }
int GetMonitorPhysicalWidth(int monitor)  { return CORE.Window.screen.width; }
int GetMonitorPhysicalHeight(int monitor) { return CORE.Window.screen.height; }
int GetMonitorRefreshRate(int monitor)    { return 60; }

Vector2 GetWindowPosition(void)  { return (Vector2){ 0, 0 }; }
Vector2 GetWindowScaleDPI(void)  { return (Vector2){ 1.0f, 1.0f }; }

const char *GetMonitorName(int monitor) { return "Xbox Display"; }

void SetClipboardText(const char *text) { TRACELOG(LOG_WARNING, "SetClipboardText() not available on Xbox"); }
const char *GetClipboardText(void)      { return NULL; }
Image GetClipboardImage(void)           { return (Image){ 0 }; }

void ShowCursor(void)    { CORE.Input.Mouse.cursorHidden = false; }
void HideCursor(void)    { CORE.Input.Mouse.cursorHidden = true; }
void EnableCursor(void)  { CORE.Input.Mouse.cursorHidden = false; }
void DisableCursor(void) { CORE.Input.Mouse.cursorHidden = true; }

// Swap back buffer with front buffer (screen drawing)
void SwapScreenBuffer(void)
{
    int w = CORE.Window.render.width;
    int h = CORE.Window.render.height;

    // Pull pixels from the software renderer into our buffer
    rlCopyFramebuffer(0, 0, w, h, PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, platform.pixels);

    // Upload pixel buffer to streaming texture and blit to screen
    SDL_UpdateTexture(platform.texture, NULL, platform.pixels, w * 4);
    SDL_RenderClear(platform.renderer);
    SDL_RenderCopy(platform.renderer, platform.texture, NULL, NULL);
    SDL_RenderPresent(platform.renderer);
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Misc
//----------------------------------------------------------------------------------

double GetTime(void)
{
    LARGE_INTEGER now = { 0 };
    QueryPerformanceCounter(&now);
    return (double)(now.QuadPart - CORE.Time.base) / (double)platform.timerFrequency.QuadPart;
}

void OpenURL(const char *url)
{
    TRACELOG(LOG_WARNING, "OpenURL() not available on Xbox");
}

//----------------------------------------------------------------------------------
// Module Functions Definition: Inputs
//----------------------------------------------------------------------------------

int SetGamepadMappings(const char *mappings)
{
    TRACELOG(LOG_WARNING, "SetGamepadMappings() not implemented on Xbox");
    return 0;
}

void SetGamepadVibration(int gamepad, float leftMotor, float rightMotor, float duration)
{
    if ((gamepad < MAX_GAMEPADS) && platform.gamepad[gamepad])
    {
        SDL_GameControllerRumble(platform.gamepad[gamepad],
            (Uint16)(leftMotor * 65535.0f),
            (Uint16)(rightMotor * 65535.0f),
            (Uint32)(duration * 1000.0f));
    }
}

void SetMousePosition(int x, int y)
{
    CORE.Input.Mouse.currentPosition  = (Vector2){ (float)x, (float)y };
    CORE.Input.Mouse.previousPosition = CORE.Input.Mouse.currentPosition;
}

void SetMouseCursor(int cursor) { TRACELOG(LOG_WARNING, "SetMouseCursor() not available on Xbox"); }

const char *GetKeyName(int key)
{
    TRACELOG(LOG_WARNING, "GetKeyName() not available on Xbox");
    return "";
}

// Register all input events
void PollInputEvents(void)
{
#if defined(SUPPORT_GESTURES_SYSTEM)
    UpdateGestures();
#endif

    CORE.Input.Keyboard.keyPressedQueueCount  = 0;
    CORE.Input.Keyboard.charPressedQueueCount = 0;

    for (int i = 0; i < MAX_KEYBOARD_KEYS; i++) CORE.Input.Keyboard.keyRepeatInFrame[i] = 0;

    CORE.Input.Gamepad.lastButtonPressed = 0;

    for (int i = 0; i < MAX_TOUCH_POINTS; i++)
        CORE.Input.Touch.previousTouchState[i] = CORE.Input.Touch.currentTouchState[i];

    for (int i = 0; i < 260; i++)
    {
        CORE.Input.Keyboard.previousKeyState[i] = CORE.Input.Keyboard.currentKeyState[i];
        CORE.Input.Keyboard.keyRepeatInFrame[i]  = 0;
    }

    for (int i = 0; i < MAX_MOUSE_BUTTONS; i++)
        CORE.Input.Mouse.previousButtonState[i] = CORE.Input.Mouse.currentButtonState[i];

    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (CORE.Input.Gamepad.ready[i])
        {
            for (int k = 0; k < MAX_GAMEPAD_BUTTONS; k++)
                CORE.Input.Gamepad.previousButtonState[i][k] = CORE.Input.Gamepad.currentButtonState[i][k];
        }
    }

    CORE.Input.Mouse.previousWheelMove = CORE.Input.Mouse.currentWheelMove;
    CORE.Input.Mouse.currentWheelMove  = (Vector2){ 0.0f, 0.0f };
    CORE.Input.Mouse.previousPosition  = CORE.Input.Mouse.currentPosition;

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                CORE.Window.shouldClose = true;
                break;

            case SDL_CONTROLLERDEVICEADDED:
            {
                int jid = event.cdevice.which;
                // Find a free slot
                int slot = -1;
                for (int i = 0; i < MAX_GAMEPADS; i++)
                {
                    if (!CORE.Input.Gamepad.ready[i]) { slot = i; break; }
                }
                if (slot >= 0)
                {
                    platform.gamepad[slot] = SDL_GameControllerOpen(jid);
                    platform.gamepadId[slot] = SDL_JoystickInstanceID(
                        SDL_GameControllerGetJoystick(platform.gamepad[slot]));
                    CORE.Input.Gamepad.ready[slot]     = true;
                    CORE.Input.Gamepad.axisCount[slot] = SDL_JoystickNumAxes(
                        SDL_GameControllerGetJoystick(platform.gamepad[slot]));
                    // Triggers start at -1.0 (not pressed)
                    CORE.Input.Gamepad.axisState[slot][GAMEPAD_AXIS_LEFT_TRIGGER]  = -1.0f;
                    CORE.Input.Gamepad.axisState[slot][GAMEPAD_AXIS_RIGHT_TRIGGER] = -1.0f;
                    const char *name = SDL_GameControllerName(platform.gamepad[slot]);
                    if (name) strncpy(CORE.Input.Gamepad.name[slot], name, 63);
                    TRACELOG(LOG_INFO, "INPUT: Gamepad connected: slot %d (%s)", slot, name? name : "?");
                }
            } break;

            case SDL_CONTROLLERDEVICEREMOVED:
            {
                int slot = GetGamepadSlot(event.cdevice.which);
                if (slot >= 0)
                {
                    SDL_GameControllerClose(platform.gamepad[slot]);
                    platform.gamepad[slot]            = NULL;
                    platform.gamepadId[slot]          = 0;
                    CORE.Input.Gamepad.ready[slot]    = false;
                    memset(CORE.Input.Gamepad.name[slot], 0, 64);
                    TRACELOG(LOG_INFO, "INPUT: Gamepad disconnected: slot %d", slot);
                }
            } break;

            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
            {
                int slot = GetGamepadSlot(event.cbutton.which);
                if (slot < 0) break;
                int state  = (event.type == SDL_CONTROLLERBUTTONDOWN) ? 1 : 0;
                int button = -1;
                switch (event.cbutton.button)
                {
                    case SDL_CONTROLLER_BUTTON_Y:           button = GAMEPAD_BUTTON_RIGHT_FACE_UP;    break;
                    case SDL_CONTROLLER_BUTTON_B:           button = GAMEPAD_BUTTON_RIGHT_FACE_RIGHT; break;
                    case SDL_CONTROLLER_BUTTON_A:           button = GAMEPAD_BUTTON_RIGHT_FACE_DOWN;  break;
                    case SDL_CONTROLLER_BUTTON_X:           button = GAMEPAD_BUTTON_RIGHT_FACE_LEFT;  break;
                    case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:  button = GAMEPAD_BUTTON_LEFT_TRIGGER_1;  break; // White
                    case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: button = GAMEPAD_BUTTON_RIGHT_TRIGGER_1; break; // Black
                    case SDL_CONTROLLER_BUTTON_BACK:        button = GAMEPAD_BUTTON_MIDDLE_LEFT;      break;
                    case SDL_CONTROLLER_BUTTON_START:       button = GAMEPAD_BUTTON_MIDDLE_RIGHT;     break;
                    case SDL_CONTROLLER_BUTTON_DPAD_UP:     button = GAMEPAD_BUTTON_LEFT_FACE_UP;     break;
                    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:  button = GAMEPAD_BUTTON_LEFT_FACE_RIGHT;  break;
                    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:   button = GAMEPAD_BUTTON_LEFT_FACE_DOWN;   break;
                    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:   button = GAMEPAD_BUTTON_LEFT_FACE_LEFT;   break;
                    case SDL_CONTROLLER_BUTTON_LEFTSTICK:   button = GAMEPAD_BUTTON_LEFT_THUMB;       break;
                    case SDL_CONTROLLER_BUTTON_RIGHTSTICK:  button = GAMEPAD_BUTTON_RIGHT_THUMB;      break;
                    default: break;
                }
                if (button >= 0)
                {
                    CORE.Input.Gamepad.currentButtonState[slot][button] = state;
                    if (state) CORE.Input.Gamepad.lastButtonPressed = button;
                }
            } break;

            case SDL_CONTROLLERAXISMOTION:
            {
                int slot = GetGamepadSlot(event.caxis.which);
                if (slot < 0) break;
                int axis = -1;
                switch (event.caxis.axis)
                {
                    case SDL_CONTROLLER_AXIS_LEFTX:        axis = GAMEPAD_AXIS_LEFT_X;       break;
                    case SDL_CONTROLLER_AXIS_LEFTY:        axis = GAMEPAD_AXIS_LEFT_Y;       break;
                    case SDL_CONTROLLER_AXIS_RIGHTX:       axis = GAMEPAD_AXIS_RIGHT_X;      break;
                    case SDL_CONTROLLER_AXIS_RIGHTY:       axis = GAMEPAD_AXIS_RIGHT_Y;      break;
                    case SDL_CONTROLLER_AXIS_TRIGGERLEFT:  axis = GAMEPAD_AXIS_LEFT_TRIGGER; break;
                    case SDL_CONTROLLER_AXIS_TRIGGERRIGHT: axis = GAMEPAD_AXIS_RIGHT_TRIGGER; break;
                    default: break;
                }
                if (axis >= 0)
                {
                    // SDL: -32768..32767 → raylib: -1.0..1.0
                    float value = event.caxis.value / 32767.0f;
                    CORE.Input.Gamepad.axisState[slot][axis] = value;

                    // Map trigger axes to digital buttons as well
                    if (axis == GAMEPAD_AXIS_LEFT_TRIGGER || axis == GAMEPAD_AXIS_RIGHT_TRIGGER)
                    {
                        int btn = (axis == GAMEPAD_AXIS_LEFT_TRIGGER)
                            ? GAMEPAD_BUTTON_LEFT_TRIGGER_2
                            : GAMEPAD_BUTTON_RIGHT_TRIGGER_2;
                        int pressed = (value > -0.9f) ? 1 : 0;
                        CORE.Input.Gamepad.currentButtonState[slot][btn] = pressed;
                        if (pressed) CORE.Input.Gamepad.lastButtonPressed = btn;
                    }
                }
            } break;

            default: break;
        }
    }
}

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

// Initialize platform: graphics, inputs and more
int InitPlatform(void)
{
    // Software renderer is mandatory for this platform
    if (rlGetVersion() != RL_OPENGL_SOFTWARE)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: XBOX: Requires GRAPHICS_API_OPENGL_SOFTWARE");
        return -1;
    }

    int screenW = CORE.Window.screen.width;
    int screenH = CORE.Window.screen.height;

    // Clamp to valid Xbox resolutions: 640x480 default, 720x480 widescreen
    if (screenW <= 0) screenW = 640;
    if (screenH <= 0) screenH = 480;

    // Initialize Xbox video hardware
    XVideoSetMode(screenW, screenH, 32, REFRESH_DEFAULT);

    // Initialize SDL video + controller
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: XBOX: SDL_Init failed: %s", SDL_GetError());
        return -1;
    }

    platform.window = SDL_CreateWindow(
        CORE.Window.title,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screenW, screenH, 0);

    if (!platform.window)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: XBOX: SDL_CreateWindow failed: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Accelerated renderer — Xbox SDL backend uses pbkit under the hood
    platform.renderer = SDL_CreateRenderer(platform.window, -1, SDL_RENDERER_ACCELERATED);
    if (!platform.renderer)
    {
        TRACELOG(LOG_WARNING, "PLATFORM: XBOX: Accelerated renderer unavailable, falling back to software");
        platform.renderer = SDL_CreateRenderer(platform.window, -1, SDL_RENDERER_SOFTWARE);
    }
    if (!platform.renderer)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: XBOX: SDL_CreateRenderer failed: %s", SDL_GetError());
        SDL_DestroyWindow(platform.window);
        SDL_Quit();
        return -1;
    }

    // Streaming texture — we upload a fresh pixel buffer every frame
    // SDL_PIXELFORMAT_ABGR8888 = bytes in memory order: R, G, B, A (matches PIXELFORMAT_UNCOMPRESSED_R8G8B8A8)
    platform.texture = SDL_CreateTexture(
        platform.renderer,
        SDL_PIXELFORMAT_ABGR8888,
        SDL_TEXTUREACCESS_STREAMING,
        screenW, screenH);

    if (!platform.texture)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: XBOX: SDL_CreateTexture failed: %s", SDL_GetError());
        SDL_DestroyRenderer(platform.renderer);
        SDL_DestroyWindow(platform.window);
        SDL_Quit();
        return -1;
    }

    // Allocate the software renderer pixel buffer (RGBA8888)
    platform.pixels = (unsigned int *)RL_CALLOC(screenW * screenH, sizeof(unsigned int));
    if (!platform.pixels)
    {
        TRACELOG(LOG_FATAL, "PLATFORM: XBOX: Failed to allocate pixel buffer");
        SDL_DestroyTexture(platform.texture);
        SDL_DestroyRenderer(platform.renderer);
        SDL_DestroyWindow(platform.window);
        SDL_Quit();
        return -1;
    }

    CORE.Window.render.width        = screenW;
    CORE.Window.render.height       = screenH;
    CORE.Window.currentFbo.width    = screenW;
    CORE.Window.currentFbo.height   = screenH;
    CORE.Window.display.width       = screenW;
    CORE.Window.display.height      = screenH;
    CORE.Window.ready               = true;

    TRACELOG(LOG_INFO, "DISPLAY: Device initialized successfully");
    TRACELOG(LOG_INFO, "    > Display size: %i x %i", screenW, screenH);
    TRACELOG(LOG_INFO, "    > Render size:  %i x %i", screenW, screenH);

    // Initialize timing
    LARGE_INTEGER time = { 0 };
    QueryPerformanceCounter(&time);
    QueryPerformanceFrequency(&platform.timerFrequency);
    CORE.Time.base = time.QuadPart;
    InitTimer();

    CORE.Storage.basePath = GetWorkingDirectory();

    TRACELOG(LOG_INFO, "PLATFORM: XBOX (nxdk): Initialized successfully");
    return 0;
}

// Close platform
void ClosePlatform(void)
{
    if (platform.pixels)
    {
        RL_FREE(platform.pixels);
        platform.pixels = NULL;
    }

    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (platform.gamepad[i])
        {
            SDL_GameControllerClose(platform.gamepad[i]);
            platform.gamepad[i] = NULL;
        }
    }

    if (platform.texture)  { SDL_DestroyTexture(platform.texture);   platform.texture  = NULL; }
    if (platform.renderer) { SDL_DestroyRenderer(platform.renderer);  platform.renderer = NULL; }
    if (platform.window)   { SDL_DestroyWindow(platform.window);      platform.window   = NULL; }

    SDL_Quit();

    TRACELOG(LOG_INFO, "PLATFORM: XBOX: Closed successfully");
}

//----------------------------------------------------------------------------------
// Module Internal Helper Functions
//----------------------------------------------------------------------------------

static int GetGamepadSlot(SDL_JoystickID jid)
{
    for (int i = 0; i < MAX_GAMEPADS; i++)
    {
        if (CORE.Input.Gamepad.ready[i] && platform.gamepadId[i] == jid) return i;
    }
    return -1;
}
