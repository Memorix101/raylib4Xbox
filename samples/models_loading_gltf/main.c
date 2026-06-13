/*******************************************************************************************
*
*   raylib [models] example - loading gltf (Xbox / nxdk port)
*
*   DPAD RIGHT: next animation
*   DPAD LEFT:  previous animation
*   BACK: exit
*
********************************************************************************************/

#include "raylib.h"
#include <stddef.h>     // NULL
#include <hal/xbox.h>

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - loading gltf");

    Camera camera = { 0 };
    camera.position   = (Vector3){ 6.0f, 6.0f, 6.0f };
    camera.target     = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model = LoadModel("D:\\resources\\models\\gltf\\robot.glb");
    Vector3 position = { 0.0f, 0.0f, 0.0f };

    int animCount = 0;
    ModelAnimation *anims = LoadModelAnimations("D:\\resources\\models\\gltf\\robot.glb", &animCount);

    unsigned int animIndex        = 0;
    unsigned int animCurrentFrame = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) break;

        UpdateCamera(&camera, CAMERA_ORBITAL);

        // Guard against empty/zero-keyframe animations to avoid modulo-by-zero
        if (anims != NULL && animCount > 0 && anims[animIndex].keyframeCount > 0)
        {
            if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_RIGHT))
                animIndex = (animIndex + 1) % animCount;
            else if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_LEFT_FACE_LEFT))
                animIndex = (animIndex + animCount - 1) % animCount;

            animCurrentFrame = (animCurrentFrame + 1) % anims[animIndex].keyframeCount;
            UpdateModelAnimation(model, anims[animIndex], (float)animCurrentFrame);
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                if (model.meshCount > 0) DrawModel(model, position, 1.0f, WHITE);
                DrawGrid(10, 1.0f);
            EndMode3D();

            if (anims != NULL && animCount > 0)
                DrawText(TextFormat("Animation: %s  (%d/%d)", anims[animIndex].name, animIndex + 1, animCount), 10, 40, 16, MAROON);
            DrawText("DPAD LEFT/RIGHT to switch animation", 10, 10, 16, GRAY);
            DrawText("Press BACK to exit", 10, screenHeight - 20, 10, DARKGRAY);
            DrawFPS(screenWidth - 80, 10);
        EndDrawing();
    }

    if (anims != NULL && animCount > 0) UnloadModelAnimations(anims, animCount);
    if (model.meshCount > 0) UnloadModel(model);
    CloseWindow();
    XReboot();
    return 0;
}
