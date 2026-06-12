/*******************************************************************************************
*
*   raylib [textures] example - sprite animation (Xbox / nxdk port)
*
*   DPAD LEFT/RIGHT: change animation speed
*   BACK: exit
*
********************************************************************************************/

#include "raylib.h"
#include <hal/xbox.h>

#define MAX_FRAME_SPEED     15
#define MIN_FRAME_SPEED      1

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib [textures] example - sprite animation");

    Texture2D scarfy = LoadTexture("D:\\resources\\scarfy.png");

    Vector2 position = { 190.0f, 210.0f };
    Rectangle frameRec = { 0.0f, 0.0f, (float)scarfy.width/6, (float)scarfy.height };
    int currentFrame  = 0;
    int framesCounter = 0;
    int framesSpeed   = 8;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Exit
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) break;

        // Change animation speed with DPAD
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT)) framesSpeed++;
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))  framesSpeed--;
        if (framesSpeed > MAX_FRAME_SPEED) framesSpeed = MAX_FRAME_SPEED;
        if (framesSpeed < MIN_FRAME_SPEED) framesSpeed = MIN_FRAME_SPEED;

        // Advance animation frame
        framesCounter++;
        if (framesCounter >= (60/framesSpeed))
        {
            framesCounter = 0;
            currentFrame++;
            if (currentFrame > 5) currentFrame = 0;
            frameRec.x = (float)currentFrame*(float)scarfy.width/6;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            // Full spritesheet reference strip
            DrawTexture(scarfy, 15, 40, WHITE);
            DrawRectangleLines(15, 40, scarfy.width, scarfy.height, LIME);
            DrawRectangleLines(15 + (int)frameRec.x, 40 + (int)frameRec.y,
                               (int)frameRec.width, (int)frameRec.height, RED);

            // Speed indicator
            DrawText("FRAME SPEED: ", 20, 180, 10, DARKGRAY);
            DrawText(TextFormat("%02i FPS", framesSpeed), 440, 180, 10, DARKGRAY);
            DrawText("DPAD LEFT/RIGHT to CHANGE SPEED", 160, 200, 10, DARKGRAY);

            for (int i = 0; i < MAX_FRAME_SPEED; i++)
            {
                if (i < framesSpeed) DrawRectangle(110 + 21*i, 175, 20, 20, RED);
                DrawRectangleLines(110 + 21*i, 175, 20, 20, MAROON);
            }

            // Animated sprite
            DrawTextureRec(scarfy, frameRec, position, WHITE);

            DrawText("(c) Scarfy sprite by Eiden Marsal", screenWidth - 210, screenHeight - 20, 10, GRAY);
            DrawText("Press BACK to exit", 10, screenHeight - 20, 10, DARKGRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(scarfy);
    CloseWindow();
    XReboot();
    return 0;
}
