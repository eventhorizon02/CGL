// definitions and globlas.

#ifndef __def__
#define __def__

#include "inputCtrl.h"

#define PLAY_AREA_WIDTH 500
#define PLAY_AREA_HEIGHT 550
#define TEXT_STRIP_HEIGHT 50
#define NUMBER_ZOOMS 5
#define SLOW_MV_DELAY 150 // the slow movment delay time
#define FAST_MV_DELAY 50 // the fast movment delay time

enum Quadrant { NW, NE, SE, SW };
static SDL_Rect camera; // the camera view area.
static SDL_Rect hud; // the 'UI' area at the top of the screen
static SDL_Rect play_area; // the main area

struct Cursor
{
    SDL_Rect cursor; // the cursor size and world position (rect)
    SDL_Point pos; // the cursor grid position
};

typedef struct Cursor cursorStrct;

struct scene
{
  SDL_Renderer* m_renderer;
  SDL_Texture* m_texture;
  SDL_Window* m_window;
  void(*update)(float);
  void(*render)();
  void(*processEvents)(inputEvent);
  void(*onEnter)();
  void(*onExit)();
  struct scene* m_next;
};

typedef struct scene Scene;

#endif

