/*******************************************************************************************
*
*   raylib [audio] example - music stream (Xbox / nxdk port)
*
*   A:           pause / resume
*   B:           restart from beginning
*   DPAD LEFT/RIGHT:  pan left / right
*   DPAD UP/DOWN:     volume up / down
*   BACK:        exit
*
********************************************************************************************/

#include "raylib.h"
#include <hal/xbox.h>

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib [audio] example - music stream");

    InitAudioDevice();

    Music music = LoadMusicStream("D:\\resources\\country.mp3");
    PlayMusicStream(music);

    float timePlayed = 0.0f;
    bool  pause      = false;
    float pan        = 0.0f;
    float volume     = 0.8f;

    SetMusicPan(music, pan);
    SetMusicVolume(music, volume);

    SetTargetFPS(30);

    while (!WindowShouldClose())
    {
        // Exit
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) break;

        UpdateMusicStream(music);

        // A: pause / resume
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN))
        {
            pause = !pause;
            if (pause) PauseMusicStream(music);
            else       ResumeMusicStream(music);
        }

        // B: restart
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_RIGHT))
        {
            StopMusicStream(music);
            PlayMusicStream(music);
            pause = false;
        }

        // DPAD LEFT/RIGHT: pan
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
        {
            pan -= 0.02f;
            if (pan < -1.0f) pan = -1.0f;
            SetMusicPan(music, pan);
        }
        else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
        {
            pan += 0.02f;
            if (pan > 1.0f) pan = 1.0f;
            SetMusicPan(music, pan);
        }

        // DPAD UP/DOWN: volume
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_UP))
        {
            volume += 0.02f;
            if (volume > 1.0f) volume = 1.0f;
            SetMusicVolume(music, volume);
        }
        else if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_LEFT_FACE_DOWN))
        {
            volume -= 0.02f;
            if (volume < 0.0f) volume = 0.0f;
            SetMusicVolume(music, volume);
        }

        timePlayed = GetMusicTimePlayed(music) / GetMusicTimeLength(music);
        if (timePlayed > 1.0f) timePlayed = 1.0f;

        BeginDrawing();
            ClearBackground(RAYWHITE);

            DrawText("MUSIC SHOULD BE PLAYING!", 130, 100, 20, LIGHTGRAY);

            // Pan control bar
            DrawText("DPAD LEFT/RIGHT: PAN", 190, 160, 10, DARKBLUE);
            DrawRectangle(170, 178, 300, 12, LIGHTGRAY);
            DrawRectangleLines(170, 178, 300, 12, GRAY);
            DrawRectangle((int)(170 + (pan + 1.0f)/2.0f*300 - 5), 170, 10, 28, DARKGRAY);

            // Progress bar
            DrawRectangle(120, 220, 400, 12, LIGHTGRAY);
            DrawRectangle(120, 220, (int)(timePlayed*400.0f), 12, MAROON);
            DrawRectangleLines(120, 220, 400, 12, GRAY);

            DrawText("B: RESTART    A: PAUSE/RESUME", 175, 250, 14, DARKGRAY);

            // Volume bar
            DrawText("DPAD UP/DOWN: VOLUME", 190, 300, 10, DARKGREEN);
            DrawRectangle(170, 318, 300, 12, LIGHTGRAY);
            DrawRectangleLines(170, 318, 300, 12, GRAY);
            DrawRectangle((int)(170 + volume*300 - 5), 310, 10, 28, DARKGRAY);

            if (pause) DrawText("PAUSED", 285, 380, 20, RED);

            DrawText("Press BACK to exit", 10, screenHeight - 20, 10, DARKGRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadMusicStream(music);
    CloseAudioDevice();
    CloseWindow();
    XReboot();
    return 0;
}
