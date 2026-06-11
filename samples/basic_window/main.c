/*******************************************************************************************
*
*   raylib [core] example - basic window (Xbox / nxdk)
*
*   Controller: Press BACK to exit and reboot to dashboard
*
********************************************************************************************/

#include "raylib.h"
#include <hal/xbox.h>   // XReboot()

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib on Xbox - basic window");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Press Back (or Start) to exit
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT) ||
            IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_RIGHT))
            break;

        BeginDrawing();
            ClearBackground(RAYWHITE);
            DrawText("raylib on Xbox!", 160, 180, 40, DARKBLUE);
            DrawText("Press BACK to exit", 180, 240, 20, DARKGRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    CloseWindow();
    XReboot();  // Return to Xbox dashboard
    return 0;
}
