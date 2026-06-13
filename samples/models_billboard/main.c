/*******************************************************************************************
*
*   raylib [models] example - billboard rendering (Xbox / nxdk port)
*
*   DPAD LEFT/RIGHT: not used - camera orbits automatically
*   BACK: exit
*
********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <hal/xbox.h>

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - billboard rendering");

    Camera camera = { 0 };
    camera.position   = (Vector3){ 5.0f, 4.0f, 5.0f };
    camera.target     = (Vector3){ 0.0f, 2.0f, 0.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Texture2D bill = LoadTexture("D:\\resources\\billboard.png");

    Vector3 billPositionStatic   = { 0.0f, 2.0f, 0.0f };
    Vector3 billPositionRotating = { 1.0f, 2.0f, 1.0f };

    Rectangle source = { 0.0f, 0.0f, (float)bill.width, (float)bill.height };

    Vector3 billUp = { 0.0f, 1.0f, 0.0f };
    Vector2 size   = { source.width/source.height, 1.0f };
    Vector2 origin = Vector2Scale(size, 0.5f);

    float distanceStatic   = 0.0f;
    float distanceRotating = 0.0f;
    float rotation         = 0.0f;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) break;

        UpdateCamera(&camera, CAMERA_ORBITAL);

        rotation        += 0.4f;
        distanceStatic   = Vector3Distance(camera.position, billPositionStatic);
        distanceRotating = Vector3Distance(camera.position, billPositionRotating);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawGrid(10, 1.0f);

                if (distanceStatic > distanceRotating)
                {
                    DrawBillboard(camera, bill, billPositionStatic, 2.0f, WHITE);
                    DrawBillboardPro(camera, bill, source, billPositionRotating, billUp, size, origin, rotation, WHITE);
                }
                else
                {
                    DrawBillboardPro(camera, bill, source, billPositionRotating, billUp, size, origin, rotation, WHITE);
                    DrawBillboard(camera, bill, billPositionStatic, 2.0f, WHITE);
                }
            EndMode3D();

            DrawText("Press BACK to exit", 10, screenHeight - 20, 10, DARKGRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(bill);
    CloseWindow();
    XReboot();
    return 0;
}
