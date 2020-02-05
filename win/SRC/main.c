#include <stdio.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include "def.h"
#include "stateMachine.h"
#include "introScene.h"


float getDeltaTime(); // get the ellapsed time since last upate.

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void closeAll();
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool isRunning = true;

int main(int argc, char* args[])
{
   if (!init())
   {
       printf("initialization failed\n");
       closeAll();
       return 0;
   }
   initCtrl(); // init the input control
   fsm* myMachine = getNewMachine(renderer, window);
   Scene* intro = getIntroScene(renderer, window);
   pushState(intro);
    while(isRunning)
    {
        myMachine->processEvents();
        myMachine->update(getDeltaTime());
        myMachine->render();
    }
    
    closeAll();
    return 0;
}

float getDeltaTime()
{
  static long long  last = 0;
  long long now = SDL_GetTicks();
  float deltaTime = (float)(now -last);
  last = now;
  return deltaTime;
 }

// initialize window and renderer. and keyboard state
bool init()
{
  isRunning = true;
  bool success = true;
  SDL_Init(SDL_INIT_VIDEO);
 window=SDL_CreateWindow("Conway's Game of Life",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,PLAY_AREA_WIDTH,PLAY_AREA_HEIGHT, SDL_WINDOW_RESIZABLE);
  renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

  // init ttf
    if( TTF_Init() == -1 )
    {
        printf( "SDL_ttf could not be initialized! SDL_ttf Error: %s\n", TTF_GetError() );
        success = false;
    }
  
  return success;
}


// free everyting
void closeAll()
{
  //Destroy window
    SDL_DestroyWindow( window);
    window = NULL;
    SDL_DestroyRenderer( renderer );
  renderer = NULL;
//Quit SDL subsystems
   TTF_Quit();
   SDL_Quit();
}




















