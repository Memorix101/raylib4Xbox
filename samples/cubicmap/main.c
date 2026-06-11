/*******************************************************************************************
*
*   raylib [models] example - cubicmap rendering (Xbox / nxdk port)
*
*   A: pause / resume orbital camera
*   BACK: exit
*
********************************************************************************************/

#include "raylib.h"
#include <hal/xbox.h>

int main(void)
{
    const int screenWidth  = 640;
    const int screenHeight = 480;

    InitWindow(screenWidth, screenHeight, "raylib [models] example - cubicmap rendering");

    Camera camera = { 0 };
    camera.position   = (Vector3){ 16.0f, 14.0f, 16.0f };
    camera.target     = (Vector3){ 0.0f, 0.0f, 0.0f };
    camera.up         = (Vector3){ 0.0f, 1.0f, 0.0f };
    camera.fovy       = 45.0f;
    camera.projection = CAMERA_PERSPECTIVE;

    Image    image    = LoadImage("D:\\resources\\cubicmap.png");
    Texture2D cubicmap = LoadTextureFromImage(image);

    Mesh  mesh  = GenMeshCubicmap(image, (Vector3){ 1.0f, 1.0f, 1.0f });
    Model model = LoadModelFromMesh(mesh);

    Texture2D texture = LoadTexture("D:\\resources\\cubicmap_atlas.png");
    model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = texture;

    Vector3 mapPosition = { -16.0f, 0.0f, -8.0f };

    UnloadImage(image);

    bool pause = false;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        // Exit
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_MIDDLE_LEFT)) break;

        // A button: toggle pause
        if (IsGamepadButtonPressed(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN)) pause = !pause;

        if (!pause) UpdateCamera(&camera, CAMERA_ORBITAL);

        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                DrawModel(model, mapPosition, 1.0f, WHITE);
            EndMode3D();

            // Minimap overlay (scaled to fit 640x480)
            DrawTextureEx(cubicmap,
                          (Vector2){ screenWidth - cubicmap.width*4.0f - 20, 20.0f },
                          0.0f, 4.0f, WHITE);
            DrawRectangleLines(screenWidth - cubicmap.width*4 - 20, 20,
                               cubicmap.width*4, cubicmap.height*4, GREEN);

            DrawText("cubicmap image used to", screenWidth - 165, 90, 10, GRAY);
            DrawText("generate map 3d model", screenWidth - 165, 104, 10, GRAY);
            DrawText(pause ? "PAUSED (press A)" : "press A to pause", 10, screenHeight - 30, 10, DARKGRAY);
            DrawText("Press BACK to exit", 10, screenHeight - 15, 10, DARKGRAY);
            DrawFPS(10, 10);
        EndDrawing();
    }

    UnloadTexture(cubicmap);
    UnloadTexture(texture);
    UnloadModel(model);

    CloseWindow();
    XReboot();
    return 0;
}
