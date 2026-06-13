/*******************************************************************************************
*
*   raylib [models] example - model loading (Xbox / nxdk port)
*
*   A: toggle bounding box
*   BACK: exit
*
*   NOTE: Drag & drop and mouse input are not available on Xbox.
*         The castle.obj is loaded at startup from D:\resources\.
*
********************************************************************************************/

#include "raylib.h"
#include <hal/xbox.h>

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - loading");

    Camera camera = { 0 };
    camera.position   = (Vector3){ 50.0f, 50.0f, 50.0f };
    camera.target     = (Vector3){ 0.0f, 12.0f, 0.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Model model       = LoadModel("D:\\resources\\models\\obj\\castle.obj");
    Texture2D texture = LoadTexture("D:\\resources\\models\\obj\\castle_diffuse.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Vector3 position = { 0.0f, 0.0f, 0.0f };
    BoundingBox bounds = GetMeshBoundingBox(model.meshes[0]);

    bool showBounds = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) break;

        // A button: toggle bounding box
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) showBounds = !showBounds;

        UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawModel(model, position, 1.0f, WHITE);
                DrawGrid(20, 10.0f);
                if (showBounds) DrawBoundingBox(bounds, GREEN);
            EndMode3D();

            DrawText("A: toggle bounding box", 10, 10, 16, DARKGRAY);
            DrawText("(c) Castle 3D model by Alberto Cano", screenWidth - 220, screenHeight - 20, 10, GRAY);
            DrawText("Press BACK to exit", 10, screenHeight - 20, 10, DARKGRAY);
            DrawFPS(screenWidth - 80, 10);
        EndDrawing();
    }

    UnloadTexture(texture);
    UnloadModel(model);
    CloseWindow();
    XReboot();
    return 0;
}
