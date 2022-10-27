/*******************************************************************************************
 *
 *   raylib [core] example - Basic window
 *
 *   Welcome to raylib!
 *
 *   To test examples, just press F6 and execute raylib_compile_execute script
 *   Note that compiled executable is placed in the same folder as .c file
 *
 *   You can find all basic examples on C:\raylib\raylib\examples folder or
 *   raylib official webpage: www.raylib.com
 *
 *   Enjoy using raylib. :)
 *
 *   This example has been created using raylib 1.0 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h
 *for details)
 *
 *   Copyright (c) 2013-2016 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

#if defined(PLATFORM_WEB)
    #include <emscripten/emscripten.h>
#endif

void UpdateDrawFrame(void);
void init(void);

struct Screen {
  int screenWidth;
  int screenHeight;

  int virtualScreenWidth;
  int virtualScreenHeight;

  float virtualRatio;
};

struct App {
  Camera2D worldSpaceCamera;  // Game world camera
  Camera2D screenSpaceCamera; // Smoothing camera
  RenderTexture2D target;
  Rectangle sourceRec;
  Rectangle destRec;
};

struct Screen *screen;
struct App *app;

void init(void) {
	screen = (struct Screen *) malloc(sizeof(struct Screen));
	app = (struct App *) malloc(sizeof(struct App));

  const int screenWidth = 800;
  const int screenHeight = 450;

  const int virtualScreenWidth = 160;
  const int virtualScreenHeight = 90;

  const float virtualRatio = (float)screenWidth / (float)virtualScreenWidth;

  struct Screen s = {screenWidth, screenHeight, virtualScreenWidth,
                     virtualScreenHeight, virtualRatio};

  InitWindow(screenWidth, screenHeight,
             "raylib [core] example - smooth pixel-perfect camera");

  Camera2D worldSpaceCamera = {0}; // Game world camera
  worldSpaceCamera.zoom = 1.0f;

  Camera2D screenSpaceCamera = {0}; // Smoothing camera
  screenSpaceCamera.zoom = 1.0f;

  RenderTexture2D target = LoadRenderTexture(
      virtualScreenWidth,
      virtualScreenHeight); // This is where we'll draw all our objects.

  // The target's height is flipped (in the source Rectangle), due to OpenGL
  // reasons
  Rectangle sourceRec = {0.0f, 0.0f, (float)target.texture.width,
                         -(float)target.texture.height};
  Rectangle destRec = {-virtualRatio, -virtualRatio,
                       screenWidth + (virtualRatio * 2),
                       screenHeight + (virtualRatio * 2)};

  struct App a = {worldSpaceCamera, screenSpaceCamera, target, sourceRec,
                  destRec};

	memcpy(screen, &s, sizeof(struct Screen));
	memcpy(app, &a, sizeof(struct App));
}

int main(void) {
  init();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(UpdateDrawFrame, 0, 1);
#else
  SetTargetFPS(60); // Set our game to run at 60 frames-per-second
  //--------------------------------------------------------------------------------------

  // Main game loop
  while (!WindowShouldClose()) // Detect window close button or ESC key
  {
    UpdateDrawFrame();
  }
#endif

  // De-Initialization
  //--------------------------------------------------------------------------------------
  UnloadRenderTexture(app->target); // Unload render texture

  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

Vector2 origin = {0.0f, 0.0f};

float rotation = 0.0f;

float cameraX = 0.0f;
float cameraY = 0.0f;

Rectangle rec01 = {70.0f, 35.0f, 20.0f, 20.0f};
Rectangle rec02 = {90.0f, 55.0f, 30.0f, 10.0f};
Rectangle rec03 = {80.0f, 65.0f, 15.0f, 25.0f};

void UpdateDrawFrame(void) {
  // Update
  //----------------------------------------------------------------------------------
  rotation +=
      60.0f * GetFrameTime(); // Rotate the rectangles, 60 degrees per second

  // Make the camera move to demonstrate the effect
  cameraX = (sinf(GetTime()) * 50.0f) - 10.0f;
  cameraY = cosf(GetTime()) * 30.0f;

  // Set the camera's target to the values computed above
  app->screenSpaceCamera.target = (Vector2){cameraX, cameraY};

  // Round worldSpace coordinates, keep decimals into screenSpace coordinates
  app->worldSpaceCamera.target.x = (int)app->screenSpaceCamera.target.x;
  app->screenSpaceCamera.target.x -= app->worldSpaceCamera.target.x;
  app->screenSpaceCamera.target.x *= screen->virtualRatio;

  app->worldSpaceCamera.target.y = (int)app->screenSpaceCamera.target.y;
  app->screenSpaceCamera.target.y -= app->worldSpaceCamera.target.y;
  app->screenSpaceCamera.target.y *= screen->virtualRatio;
  //----------------------------------------------------------------------------------

  // Draw
  //----------------------------------------------------------------------------------
  BeginTextureMode(app->target);
  ClearBackground(RAYWHITE);

  BeginMode2D(app->worldSpaceCamera);
  DrawRectanglePro(rec01, origin, rotation, BLACK);
  DrawRectanglePro(rec02, origin, -rotation, RED);
  DrawRectanglePro(rec03, origin, rotation + 45.0f, BLUE);
  EndMode2D();
  EndTextureMode();

  BeginDrawing();
  ClearBackground(RED);

  BeginMode2D(app->screenSpaceCamera);
  DrawTexturePro(app->target.texture, app->sourceRec, app->destRec, origin,
                 0.0f, WHITE);
  EndMode2D();

  DrawText(TextFormat("Screen resolution: %ix%i", screen->screenWidth,
                      screen->screenHeight),
           10, 10, 20, DARKBLUE);
  DrawText(TextFormat("World resolution: %ix%i", screen->virtualScreenWidth,
                      screen->virtualScreenHeight),
           10, 40, 20, DARKGREEN);
  DrawFPS(GetScreenWidth() - 95, 10);
  EndDrawing();
  //----------------------------------------------------------------------------------
}
