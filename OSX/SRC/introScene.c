
#include <stdio.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "introScene.h"
#include "gameScene.h"
#include "stateMachine.h"
#include "inputCtrl.h"
#include "utils.h"

static void processEvents(inputEvent);
static void update(float);
static void render();
static void onEnter();
static void onExit();

Scene* introScene = NULL;
extern bool isRunning;

typedef struct textures
{
    SDL_Texture* t1;
    SDL_Texture* t2;
    SDL_Texture* t3;
    SDL_Texture* t4;
    SDL_Texture* t5;
    SDL_Texture* t6;
    SDL_Texture* t7;
    SDL_Texture* t8;
    SDL_Texture* t9;
    SDL_Texture* t10;
    SDL_Texture* t11;
    SDL_Texture* t12;
    TTF_Font* f1;
    TTF_Font* f2;
}Textures;

static Textures myTextures;

Scene* getIntroScene(SDL_Renderer* rend, SDL_Window* win)
{
    introScene = malloc(sizeof(Scene));
    introScene->m_renderer = rend;
    introScene->m_window = win;
    introScene->onEnter = &onEnter;
    introScene->onExit = &onExit;
    introScene->update = &update;
    introScene->render = &render;
    introScene->processEvents = &processEvents;
    introScene->m_next = NULL;
    introScene->m_texture = NULL;
    
    return introScene;
}

static void onEnter()
{
    //Open the fonts
    myTextures.f1 = TTF_OpenFont( "pixelFont.ttf", 28 );
    myTextures.f2 = TTF_OpenFont( "pixelFont.ttf", 18 );
    SDL_Color color = { 0, 250, 150, 255 };
    SDL_Color color2 = { 255, 255, 0, 255 };
    
    
    loadFromRenderedTexture(&myTextures.t1, "WELCOME TO",myTextures.f1, color ,introScene->m_renderer ); // creates a texture from a string, stores
    loadFromRenderedTexture(&myTextures.t2, "CONWAY'S GAME OF LIFE",myTextures.f1, color2 ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t3, "CONTROLS:",myTextures.f2, color2 ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t4, "ARROWS - SCROLL AROUND THE GRID",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t5, "TAB - TOGGLE ZOOM LEVELS",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t6, "ENTER - ADD OR DELETE A BIT",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t7, "SPACE - START / STOP SIMULATION",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t8, "D - TOGGLE THE DELAY TIMES ",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t9, "S - STEP A GENERATION FORWAD ",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t10, "C - CLEAR THE UNIVERSE",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t11, "ESC - SHOW / HIDE GRID",myTextures.f2, color ,introScene->m_renderer );
    loadFromRenderedTexture(&myTextures.t12, "PRESS SPACE BAR",myTextures.f1, color2 ,introScene->m_renderer );
}

static void processEvents(inputEvent e)
{
    if (e == startEv)
    {
        Scene* game = getGameScene(introScene->m_renderer, introScene->m_window);
        changeState(game);

    }else if (e == quitEv)
      isRunning = false;
}

static void update(float dt)
{
    
}
static void render() // you might wander why I didn't just recycled the rect variable rather than just create a new one for every texture, I tried it didn't work.
{
    int width,height;
    
    SDL_QueryTexture(myTextures.t1, NULL, NULL, &width, &height);
    SDL_Rect rect = { width / 1.5, 10, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t1, NULL,&rect ); // "wlocome to"
    
    SDL_QueryTexture(myTextures.t2, NULL, NULL, &width, &height);
    SDL_Rect rect2 = { 20, 50, width, height } ;
    SDL_RenderCopy( introScene->m_renderer,myTextures.t2, NULL,&rect2 ); // "conway's game of life"
    
    SDL_QueryTexture(myTextures.t3, NULL, NULL, &width, &height);
    SDL_Rect rect3 = { 20, 120, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t3, NULL,&rect3 ); // "controls:"
    
    SDL_QueryTexture(myTextures.t4, NULL, NULL, &width, &height);
    SDL_Rect rect4 = { 20, 170, width, height} ;
    SDL_RenderCopy( introScene->m_renderer,myTextures.t4, NULL,&rect4 );
    
    SDL_QueryTexture(myTextures.t5, NULL, NULL, &width, &height);
    SDL_Rect rect5 = { 20, 200, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t5, NULL,&rect5);
    
    SDL_QueryTexture(myTextures.t6, NULL, NULL, &width, &height);
    SDL_Rect rect6 = { 20, 230, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t6, NULL,&rect6);
    
    SDL_QueryTexture(myTextures.t7, NULL, NULL, &width, &height);
    SDL_Rect rect7 = { 20, 260, width, height} ;
    SDL_RenderCopy( introScene->m_renderer,myTextures.t7, NULL,&rect7);
    
    SDL_QueryTexture(myTextures.t8, NULL, NULL, &width, &height);
    SDL_Rect rect8 = { 20, 290, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t8, NULL,&rect8);
    
    SDL_QueryTexture(myTextures.t9, NULL, NULL, &width, &height);
    SDL_Rect rect9 = { 20, 320, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t9, NULL,&rect9);
    
    SDL_QueryTexture(myTextures.t10, NULL, NULL, &width, &height);
    SDL_Rect rect10 = { 20, 350, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t10, NULL,&rect10);
    
    SDL_QueryTexture(myTextures.t11, NULL, NULL, &width, &height);
    SDL_Rect rect11 = { 20, 380, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t11, NULL,&rect11);
    
    SDL_QueryTexture(myTextures.t12, NULL, NULL, &width, &height);
    SDL_Rect rect12 = { width / 4, 450, width, height };
    SDL_RenderCopy( introScene->m_renderer,myTextures.t12, NULL,&rect12); // "press space bar"

}


static void onExit()
{
    free(myTextures.f1);
    free(myTextures.f2);
    freeTexture(myTextures.t1);
    freeTexture(myTextures.t2);
    freeTexture(myTextures.t3);
    freeTexture(myTextures.t4);
    freeTexture(myTextures.t5);
    freeTexture(myTextures.t6);
    freeTexture(myTextures.t7);
    freeTexture(myTextures.t8);
    freeTexture(myTextures.t9);
    freeTexture(myTextures.t10);
    freeTexture(myTextures.t11);
    freeTexture(myTextures.t12);
    
    myTextures.f1 = NULL;
    myTextures.f2 = NULL;
}


