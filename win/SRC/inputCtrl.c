
#include <SDL.h>
#include "inputCtrl.h"

const Uint8 *state;


void initCtrl()
{
   state = SDL_GetKeyboardState(NULL);
}

// which event happended
inputEvent getEvent()
{
    SDL_Event event;
    inputEvent myEvent = noEv;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_QUIT:
                myEvent = quitEv;
                break;
                
            case SDL_KEYUP:
            {
                if ( (event.key.keysym.sym) == SDLK_SPACE )
                    myEvent = startEv;
                else if ( (event.key.keysym.sym) == SDLK_ESCAPE )
                    myEvent = gridEv;
                else if ( (event.key.keysym.sym) == SDLK_TAB )
                    myEvent = zoomEv;
                else if ( (event.key.keysym.sym) == SDLK_RETURN)
                    myEvent = enterEv;
                else if ( (event.key.keysym.sym) == SDLK_d)
                    myEvent = delayEv;
                else if ((event.key.keysym.sym) == SDLK_s)
                    myEvent = stepEv;
                else if ((event.key.keysym.sym) == SDLK_c)
                    myEvent = clearEv;
                break;
            }
        }
    }
    return myEvent;
}

 // is a state
bool isState(inputStates myState)
{
    static int map[4] = { SDL_SCANCODE_LEFT, SDL_SCANCODE_RIGHT, SDL_SCANCODE_UP, SDL_SCANCODE_DOWN };
    if (state[map[myState]])
        return true;
   
    return false;
}









