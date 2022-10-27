/*******************************************************************************************
 *
 *   raylib [core] example - Basic window (adapted for HTML5 platform)
 *
 *   This example is prepared to compile for PLATFORM_WEB, PLATFORM_DESKTOP and
 *PLATFORM_RPI As you will notice, code structure is slightly diferent to the
 *other examples... To compile it for PLATFORM_WEB just uncomment #define
 *PLATFORM_WEB at beginning
 *
 *   This example has been created using raylib 1.3 (www.raylib.com)
 *   raylib is licensed under an unmodified zlib/libpng license (View raylib.h
 *for details)
 *
 *   Copyright (c) 2015 Ramon Santamaria (@raysan5)
 *
 ********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include <stdio.h>

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

typedef struct Sprite {
  Vector2 position;
  Vector2 anchor;
  Texture2D texture;
  float rotation;
  float scale;
} Sprite;

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void); // Update and Draw one frame
int init();
int dispose();

Sprite SpriteLoad(const char *fileName);
void SpriteDraw(Sprite *sprite);

#ifndef V2CENTER
#define V2CENTER                                                               \
  (Vector2) { 0.5, 0.5 }
#endif

#ifndef V2UP
#define V2UP                                                                   \
  (Vector2) { 0, 1 }
#endif

float AngleDifference(float angle1, float angle2) {
  double diff = ((int)angle2 - (int)angle1 + 180) % 360 - 180;
  return diff < -180 ? diff + 360 : diff;
}

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
int screenWidth = 512;
int screenHeight = 512;

struct PawnState {
  Vector2 position;
  Vector2 direction;
  Vector2 targetPosition;
  float speed;
  Vector2 lookDirection;
} pawnState;

struct PawnConfig {
  float speed;
  float forceAcc;
  float forceBreak;
  float rotDump;
} pawnConfig;

struct GameAssets {
  Sprite crosshair;
  Sprite playership;
  Sprite tilefloor;
  Sprite locationmark;
} * gameAssets;

void PawnWASDControls(struct PawnState *state, struct PawnConfig *config);
void PawnPointerControls(struct PawnState *state, struct PawnConfig *config);

int init() {
  // SetConfigFlags(FLAG_MSAA_4X_HINT); // Set MSAA 4X hint before windows
  //  creation

  pawnConfig.speed = 7.0f;
  pawnConfig.forceAcc = 0.05f;
  pawnConfig.forceBreak = 0.3f;
  pawnConfig.rotDump = 0.1f;

  pawnState.direction = V2UP;
  pawnState.position = (Vector2){256, 256};

  return 0;
}

struct GameAssets load() {
  struct GameAssets ga = {
      SpriteLoad("res/crosshair.png"),
      SpriteLoad("res/playership.png"),
      SpriteLoad("res/tilefloor.png"),
      SpriteLoad("res/locationmark.png"),
  };

  return ga;
}

int dispose() {
  UnloadTexture(gameAssets->crosshair.texture);
  UnloadTexture(gameAssets->playership.texture);
  UnloadTexture(gameAssets->tilefloor.texture);
  UnloadTexture(gameAssets->locationmark.texture);

  return 0;
}

//----------------------------------------------------------------------------------
// Main Enry Point
//----------------------------------------------------------------------------------
int main() {
  // Initialization
  //--------------------------------------------------------------------------------------

  init();

  InitWindow(screenWidth, screenHeight, "tyndustre 220919");

  // DisableCursor();

  struct GameAssets ga = load();
  gameAssets = &ga;

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

  dispose();

  // De-Initialization
  //--------------------------------------------------------------------------------------
  CloseWindow(); // Close window and OpenGL context
  //--------------------------------------------------------------------------------------

  return 0;
}

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
void UpdateDrawFrame(void) {

  // pre
  Vector2 mousepos = GetMousePosition();

  // PawnWASDControls(&pawnState, &pawnConfig);

  PawnPointerControls(&pawnState, &pawnConfig);

  pawnState.lookDirection = Vector2Normalize(Vector2Lerp(
      pawnState.lookDirection,
      Vector2Normalize(Vector2Subtract(mousepos, pawnState.position)),
      pawnConfig.rotDump));

  // Draw
  //----------------------------------------------------------------------------------
  BeginDrawing();

  ClearBackground(RAYWHITE);
  DrawTextureTiled(gameAssets->tilefloor.texture, (Rectangle){0, 0, 1024, 1024},
                   (Rectangle){0, 0, 1024, 1024}, (Vector2){0, 0}, 0, 1, WHITE);
  DrawText("XXX", 190, 200, 20, LIGHTGRAY);

  gameAssets->crosshair.position = mousepos;
  gameAssets->playership.position = pawnState.position;
  gameAssets->playership.rotation =
      Vector2Angle(V2UP, pawnState.lookDirection) * RAD2DEG;
  gameAssets->locationmark.position = pawnState.targetPosition;
  gameAssets->locationmark.rotation += 1;
  gameAssets->locationmark.scale = 1.1 + sinf(GetTime()) * 0.1;

  SpriteDraw(&gameAssets->crosshair);
  SpriteDraw(&gameAssets->playership);
  SpriteDraw(&gameAssets->locationmark);

  EndDrawing();
  //----------------------------------------------------------------------------------
}

void PawnPointerControls(struct PawnState *state, struct PawnConfig *config) {
  Vector2 mousepos = GetMousePosition();

  if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    state->targetPosition = mousepos;

  Vector2 target = Vector2Subtract(
      state->targetPosition, Vector2Add(state->position, state->direction));
  float distance = Vector2Length(target);
  Vector2 direction = Vector2Normalize(target);
  Vector2 maxspeed = Vector2Scale(direction, fminf(distance, config->speed));

  Vector2 acceleration = Vector2Subtract(maxspeed, state->direction);
  Vector2 breakforce = Vector2Scale(
      state->direction,
      fminf(0,
            Vector2DotProduct(Vector2Normalize(state->direction), direction)));

  state->direction = Vector2Add(state->direction,
                                Vector2Scale(acceleration, config->forceAcc));
  state->direction = Vector2Add(state->direction,
                                Vector2Scale(breakforce, config->forceBreak));

  state->position.x += state->direction.x;
  state->position.y += state->direction.y;
}

void PawnWASDControls(struct PawnState *state, struct PawnConfig *config) {
  Vector2 inputDirection;

  inputDirection.x = 0;
  inputDirection.y = 0;

  // inputs
  if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
    inputDirection.x += 1;
  if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
    inputDirection.x -= 1;
  if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
    inputDirection.y -= 1;
  if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
    inputDirection.y += 1;

  float speed = (inputDirection.x || inputDirection.y || 0) * config->speed;
  state->speed = Lerp(state->speed, speed, config->forceAcc);

  Vector2Normalize(inputDirection);
  state->direction.x =
      Lerp(state->direction.x, inputDirection.x, config->rotDump);
  state->direction.y =
      Lerp(state->direction.y, inputDirection.y, config->rotDump);

  // apply move
  Vector2 move = Vector2Scale(state->direction, state->speed);
  state->position.x += move.x;
  state->position.y += move.y;
}

// ---

Sprite SpriteLoad(const char *fileName) {
  struct Sprite s;

  s.texture = LoadTexture(fileName);
  s.anchor = (Vector2){0.5, 0.5};
  s.position = (Vector2){0.0, 0.0};
  s.rotation = 0.0;
  s.scale = 1.0;

  return s;
}

void SpriteDraw(Sprite *sprite) {
  const float x = sprite->texture.width * sprite->scale * sprite->anchor.x;
  const float y = sprite->texture.height * sprite->scale * sprite->anchor.y;
  const Vector2 v0 = Vector2Rotate((Vector2){x, y}, sprite->rotation * DEG2RAD);
  const Vector2 v1 =
      (Vector2){sprite->position.x - v0.x, sprite->position.y - v0.y};

  DrawTextureEx(sprite->texture, v1, sprite->rotation, sprite->scale, WHITE);
}
