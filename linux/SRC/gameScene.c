
#include <SDL2/SDL_ttf.h>
#include <unistd.h>
#include <stdio.h>
#include "inputCtrl.h"
#include "stateMachine.h"
#include "gameScene.h"
#include "utils.h"

#define WORLD_SIZE 100 // increase the world by chonks of 100

static void processEvents(inputEvent);
static void update(float);
static void render();
static void onEnter();
static void onExit();
void clearWorld(); // clear all the bits and start new
void refocus(); // focus the camera as so to put the cursor in the middle of the screen when changing zoom.
void drawBits(bool universe); //draw the bits of the universe
bool prepareUniverse(); //prepare the array that holds the bits of the world.
bool resizeUniverse(); // increase the size of the world according to the furthest bit
bool enterPoint(const SDL_Point point, bool universe); // enter or clear a bit in a universe
bool getBitState(const SDL_Point point,bool universe);// check if a bit is on or off.
void setBit(const SDL_Point point, bool on, bool universe); // for a given coordinate set a bit to on or off
void getNeigboursState(const SDL_Point point,bool neigbours[], bool universe); // populate the array with info on all 8 neighbours of a point.
void applyRules(); // apply the conway rules of the simulation.

typedef struct textures
{
    SDL_Texture* myTex1;
    SDL_Texture* myTex2;
    TTF_Font* myFont;
}Textures;

Scene* GameScene = NULL;
cursorStrct cursor;
extern bool isRunning;
float cursor_time; // accumulate time for the cursor flasing action.
float input_delay_time; // for cursor movment on the grid, we need to control the speed.
float delay_time; // a delay timer to slow iterations
int delayStep; // step through constant delay values, ie 0, 500, 1000 ms.
float movment_timer; // accumulated timer for cursor movment purposes.
int movment_counter; // how many cursor movments have we made, needed so that after x amount we can reduce the input_delay_time so as to move faster.
int zooms[NUMBER_ZOOMS] = {5, 10, 20, 25, 50}; // the possible sizes of cursors and the grid
int zoom_index = 0; // the current zoom size index.
int number_bits = 0; // number of bits in the universe.
Textures myTextures;
char* message = NULL; // the text message on above the grid.
bool gameState; // edit state, or game state?
bool show_grid;
bool out_of_memory; // are we out of memory?
long long world_size; // the current width and height of our 2d universe.
typedef char** quarter; // a quarter of a world, i.e NW, SE, etc.

struct World
{
    quarter quad[4];
};
typedef struct World world; // one of the two possible world
world twoWorlds[2]; // a container of two universes. the universe will alernate pointing between one of those two.
bool universe; // just to alternate 0 index or 1 in the twoWorlds[2] array.


Scene* getGameScene(SDL_Renderer* rend, SDL_Window* win)
{
    GameScene = malloc(sizeof(Scene));
    GameScene->m_renderer = rend;
    GameScene->m_window = win;
    GameScene->onEnter = &onEnter;
    GameScene->onExit = &onExit;
    GameScene->update = &update;
    GameScene->render = &render;
    GameScene->processEvents = &processEvents;
    GameScene->m_next = NULL;
    GameScene->m_texture = NULL;
    
    return GameScene;
}

static void onEnter()
{
    out_of_memory = false;
    world_size = WORLD_SIZE; // staring point of the universe size
    hud.x = 0;
    hud.y = 0;
    hud.w = PLAY_AREA_WIDTH;
    hud.h = TEXT_STRIP_HEIGHT;
    play_area.x = 0;
    play_area.y = TEXT_STRIP_HEIGHT;
    play_area.w = play_area.h = PLAY_AREA_WIDTH;
    camera.w = PLAY_AREA_WIDTH;
    camera.h = PLAY_AREA_WIDTH;
    camera.x = 0;
    camera.y = 0;
    cursor.cursor.w = cursor.cursor.h = zooms[zoom_index];
    cursor.cursor.x = camera.x;
    cursor.cursor.y = camera.y;
    cursor.pos.x = cursor.pos.y = 0;
    refocus(); // center the view around the cursor.
    cursor_time = 0;
    input_delay_time = SLOW_MV_DELAY; // 500 ms delay initialy for cursor movment on the grid
    movment_timer = 0;
    movment_counter = 0;
    // load the font
    myTextures.myFont = TTF_OpenFont( "pixelFont.ttf", 18 );
    message = "number of bits \0";
    if (!prepareUniverse())
    {
        message = "out of memory\0";
        out_of_memory = true;
    }
    
    gameState = false; // start off in edit state.
    universe = false; // start off with world 0.
    delay_time = 0;
    delayStep = 0; // intial delay step.
    show_grid = true;
}

static void onExit()
{
    message = NULL;
    world world0 = twoWorlds[0];
    world world1 = twoWorlds[1];
    for (int j = 0; j < 4; ++j) // for each quadrant
     {
        quarter regionA = world0.quad[j];
        quarter regionB = world1.quad[j];
        for (int i = 0; i < world_size; ++i)
         {
           free(regionA[i]);
           free(regionB[i]);
           regionA[i] = NULL;
           regionB[i] = NULL;
         }
            free(regionA);
            free(regionB);
            regionA = NULL;
            regionB = NULL;
     }
    free(myTextures.myFont);
    
    freeTexture(myTextures.myTex1);
    freeTexture(myTextures.myTex2);
    myTextures.myFont = NULL;
    
}

static void render()
{
    SDL_Rect cam = {camera.x, camera.y, camera.w, camera.h};
    SDL_Rect curs = {cursor.cursor.x - camera.x, cursor.cursor.y - camera.y, cursor.cursor.w, cursor.cursor.h};
    SDL_RenderSetViewport(GameScene->m_renderer, &play_area); // render the grid only to the area bellow the hud
    if (!gameState) // don't show flashing cursor during simulation.
    drawCursor(cursor_time, curs,GameScene->m_renderer);
    if (show_grid)
    drawGrid(cam, cursor.cursor.w, GameScene->m_renderer);
    drawBits(universe);
    // render the cursor positon text
    SDL_RenderSetViewport(GameScene->m_renderer, &hud); // render the text to the hud area
    int x = cursor.pos.x;
    int y = cursor.pos.y;
    int length = snprintf(NULL, 0, "%d",x) + snprintf(NULL, 0, "%d",y) + 1; //the first snprint is only to get the size of the requiret malloc.
    char* str = malloc(length + 2); // +2 for null terminator and the ',' separating the two coordinates
    snprintf(str, length + 1, "%d,%d",x,y);
    SDL_Color color = {0x00,0xc0,0x00,0x00};
    loadFromRenderedTexture(&myTextures.myTex1, str,myTextures.myFont, color ,GameScene->m_renderer ); // creates a texture from a string.
    free(str);
    
    // render the number of bits or 'out of memory message'
    length = snprintf(NULL, 0, "%d",number_bits) + 20; // get the length in chars for ' number bits ' + the number of chars in the numbers of bits + null terminator.
    char* str2 = malloc(length);
    snprintf(str2, length, "%s%d",message,number_bits);
    loadFromRenderedTexture(&myTextures.myTex2, str2,myTextures.myFont, color ,GameScene->m_renderer ); // creates a texture from a string.
    free(str2);
    
    int width,height;
    SDL_QueryTexture(myTextures.myTex1, NULL, NULL, &width, &height);
    SDL_Rect rect = {width / 5, 10, width, height};
    SDL_QueryTexture(myTextures.myTex2, NULL, NULL, &width, &height);
    SDL_Rect rect2 = {width /1.1, 10, width, height};
    SDL_RenderCopy(GameScene->m_renderer,myTextures.myTex1, NULL,&rect );
    SDL_RenderCopy(GameScene->m_renderer,myTextures.myTex2, NULL,&rect2 );
}

static void processEvents(inputEvent e)
{
    switch (e)
    {
        case zoomEv: // will cycle between 5 different grid sizes, each a sequential multiple of 5.
            zoom_index = (zoom_index + 1) % NUMBER_ZOOMS;
            cursor.cursor.h = cursor.cursor.w = zooms[zoom_index];
            cursor.cursor.x = cursor.pos.x * cursor.cursor.w; // update the cursor positon so as to stay on the same grid square after scaling
            cursor.cursor.y = cursor.pos.y * cursor.cursor.h;
            refocus();
            break;
            
        case startEv:
        {
            gameState = !gameState;
            break;
        }
        case enterEv:
        {
            if (gameState || out_of_memory) return; // while simulation is running, it's not possible to enter new bits, must enter edit state first.
            
            SDL_Point point = {cursor.pos.x,cursor.pos.y};
            if(enterPoint(point, universe))
                ++number_bits;
            else
            {
                if (!out_of_memory) // the failure to enter a point could have been due to lack of memory
                --number_bits;
            }
            break;
        }
        case clearEv:
            clearWorld();
            break;
        case stepEv:
            if (gameState) // only allow step throughs in edit mode
                return;
            applyRules();
            break;
        case delayEv:
            delayStep = ((delayStep + 500) % 1500);
            break;
        case gridEv:
            show_grid = !show_grid;
            break;
        case quitEv:
            isRunning = false;
            break;
            
        default:
            break;
            
    }
    
    bool movment_allowed = (movment_timer > input_delay_time);
    if (movment_counter > 15)
        input_delay_time = FAST_MV_DELAY;
    if (isState(mvUpSt))
    {
        if (movment_allowed)
        {
            ++movment_counter;
            movment_timer = 0;
            cursor.cursor.y -= cursor.cursor.h; // world coordinates
            cursor.pos.y -=1; // grid coordinates
            if (cursor.cursor.y < camera.y) // we've reached the top edge of the camera view
                camera.y -= cursor.cursor.h; // move the camera up.
        }
    }
    else if(isState(mvDownSt))
    {
        if (movment_allowed)
        {
            ++movment_counter;
            movment_timer = 0;
            cursor.cursor.y += cursor.cursor.h;
            cursor.pos.y +=1;
            if (cursor.cursor.y >= camera.y + camera.h) // we've reached the botom edge of the camera view
                camera.y += cursor.cursor.h; // move the camera down.
        }
    }
    else if (isState(mvLeftSt))
    {
        if (movment_allowed)
        {
            ++movment_counter;
            movment_timer = 0;
            cursor.cursor.x -= cursor.cursor.w;
            cursor.pos.x -=1;
            if (cursor.cursor.x < camera.x) // we've reached the left edge of the camera view
                camera.x -= cursor.cursor.w; // move he camera left
        }
    }
    else if (isState(mvRightSt))
    {
        if (movment_allowed)
        {
            ++movment_counter;
            movment_timer = 0;
            cursor.cursor.x += cursor.cursor.w;
            cursor.pos.x +=1;
            if (cursor.cursor.x >= camera.x + camera.w) // we've reached the right edge of the camera view
                camera.x += cursor.cursor.w; // move the camera right.
        }
    }else
    {
        movment_counter = 0; // start slow again.
        input_delay_time = SLOW_MV_DELAY;
    }
}

static void update(float dt)
{
    cursor_time = fmod(cursor_time + dt, 1000.0);
    delay_time += dt;
    movment_timer += dt;
    if (delay_time > delayStep)
    {
        delay_time = 0;
        if (gameState && !out_of_memory)
            applyRules(); // the magic happens here.
    }
}


// focus the camera as so to put the cursor in the middle of the screen when changing zoom.
void refocus()
{
    int d = camera.w / cursor.cursor.w; // how many squares are in the current zoom of the camera
    camera.x = cursor.cursor.x - ((int)(d/2)) * cursor.cursor.w;
    camera.y = cursor.cursor.y - ((int)(d/2)) * cursor.cursor.w;
}

// draw the bits of the universe
void drawBits(bool universe)
{
    SDL_Rect rect = cursor.cursor; // just for the cursor size
    SDL_SetRenderDrawColor(GameScene->m_renderer, 0x00, 0xff, 0x00, 0x00);
    world current_world = twoWorlds[universe];
    
    for (int i = 0; i < world_size; ++i)
    {
        for (int q = 0; q < 4; ++q)
        {
            quarter quar = current_world.quad[q];
            char* row = quar[i];
            for (int j = 0; j < world_size; ++j)
            {
                int charNum = j / 8;
                char target = row[charNum];
                int bitNum = j % 8;
                if ((1<<bitNum) & target) // the bit is on
                 {
                     switch (q)
                     {
                         case NW: // northwest quadrant
                             rect.x = -j * rect.w - camera.x; // screen = world - camera;
                             rect.y = -i * rect.h - camera.y;
                             break;
                             
                         case NE:
                             rect.x = j * rect.w - camera.x; // screen = world - camera;
                             rect.y = -i * rect.h - camera.y;
                             break;
                             
                          case SE:
                             rect.x = j * rect.w - camera.x; // screen = world - camera;
                             rect.y = i * rect.h - camera.y;
                             break;
                             
                         case SW:
                             rect.x = -j * rect.w - camera.x; // screen = world - camera;
                             rect.y = i * rect.h - camera.y;
                             break;
                             
                     }
                     SDL_RenderFillRect(GameScene->m_renderer, &rect);
                 }
            }
        }
    }
}

//prepare the array that holds the bits of the world.
bool prepareUniverse()
{
    // we have the curretn world size, allocate memory equivalent to world_size in bits width wise and world_size in char* in height wise
    world world0;
    world world1;
    long double width = ceill((long double)world_size / 8.f); // we need the world size in bits not whole chars, to save memory
    
    for (int i = 0; i < 4; ++i)
    {
        world0.quad[i] = malloc(world_size * sizeof(char*));
        world1.quad[i] = malloc(world_size * sizeof(char*));
        if ((world0.quad[i] == NULL) || (world1.quad[i] == NULL))
           return false;
        twoWorlds[0] = world0;
        twoWorlds[1] = world1;
        for (int j = 0; j < world_size; ++j) // allocate the memory for the rows;
        {
            char* ptr0 = malloc(width);
            char* ptr1 = malloc(width);
            if ((!ptr0) || (!ptr1))
                return false;
            memset(ptr0, 0, width);
            memset(ptr1, 0, width);
            world0.quad[i][j] = ptr0;
            world1.quad[i][j] = ptr1;
        }
    }
    // point universe to twoWorlds[0];
    universe = false;
    return true;
}

// enter or clear a bit in a universe, if it was on then turn off, if off then turn on
bool enterPoint(const SDL_Point point, bool universe)
{
    // if the requested point is out of bounds, resize the worlds
    int bigger = max((abs(point.x)),(abs(point.y)));
    
    while (bigger >= world_size)
    {
        printf("resizing world\n");
       if(!resizeUniverse())
        {
            out_of_memory = true;
            message = "out of memory!\0";
            printf("failed to increase world size\n");
            return false;
        }
    }
    
    world current_world = twoWorlds[universe];
    // find the right quadrant first
    enum Quadrant qudrant = getQuad(point);
    quarter worldPart = current_world.quad[qudrant];
    // find char number and bit number in that char for the x coordinate i.e x = 77, thats 9 chars + 5 bits.
    int charNum = (abs(point.x)) / 8; // for 8 bits in a char
    int bitNUm = (abs(point.x)) % 8;
    char mask = 1 << bitNUm;
    char* row = worldPart[(abs(point.y))];
    if(row[charNum] ^= mask) // the xor bit wise flips the state of the said bit.
        return true;
    
    return false;
}

// increase the size of the world according to the furthest bit
bool resizeUniverse()
{
    // to avoid multiple reallocs resize world by 100 units at a time
    long double width = ceill((long double)(world_size + WORLD_SIZE) / 8.f); // we need the world size in bits not whole chars, to save memory
    long double prev_width = ceill((long double)world_size / 8.f);
    world world0 = twoWorlds[0];
    world world1 = twoWorlds[1];
    for (int i = 0; i < 4; ++i)
    {
        quarter quadA = world0.quad[i];
        quarter quadB = world1.quad[i];
        // add new rows to each quarter
        char** temp0 = realloc(quadA, (world_size + WORLD_SIZE)*sizeof(char*));
        char** temp1 = realloc(quadB, (world_size + WORLD_SIZE)*sizeof(char*));
        if ( (temp0 == NULL) || (temp1 == NULL) )
        {
            printf("failed to increase world size, failed to add rows to quad %d\n",i);
            return false;
        }
        quadA = temp0;
        quadB = temp1;
        // allocate memory for the new added rows
        for (long long j = world_size; j < (world_size + WORLD_SIZE); ++j)
        {
            char* newRowA = malloc(width);
            char* newRowB = malloc(width);
            if ((!newRowA) || (!newRowB))
            {
                printf("failed to increase world size, failed to allocate memory to new rows\n");
                return false;
            }
            memset(newRowA, 0, width);
            memset(newRowB, 0, width);
            quadA[j] = newRowA;
            quadB[j] = newRowB;
        }
      // lastly, expand priot rows to the new width and set the extended portion to zeros
        for ( long long j = 0; j < world_size; ++j)
        {
            char* oldRowA = quadA[j];
            char* oldRowB = quadB[j];
            char* row0 = realloc(oldRowA, width);
            char* row1 = realloc(oldRowB, width);
            if ((row0 == NULL) || (row1 == NULL))
            {
                printf("failed to increase world size, failed to expand prior rows to new width\n");
                return false;
            }
            memset((row0 + (int)prev_width), 0, width - prev_width);
            memset(row1 + (int)prev_width, 0, width - prev_width);
            quadA[j] = row0;
            quadB[j] = row1;
        }
        world0.quad[i] = quadA;
        world1.quad[i] = quadB;
        twoWorlds[0] = world0;
        twoWorlds[1] = world1;
    }
    world_size += WORLD_SIZE;
    return true;
}

// check if a bit is on or off.
bool getBitState(const SDL_Point point,bool universe)
{
    // first off, if the requested point is out of bounds then return false.
    int bigger = max((abs(point.x)),(abs(point.y)));
    if (bigger >= world_size)
      return false;
    // get the right quadrant first.
    enum Quadrant quad = getQuad(point);
    // get the requested world
    world current_world = twoWorlds[universe];
    // get the right quarter
    char** worldPart = current_world.quad[quad];
    // find char number and bit number in that char for the x coordinate i.e x = 77, thats 9 chars + 5 bits.
    int charNum = (abs(point.x)) / 8; // for 8 bits in a char
    int bitNUm = (abs(point.x)) % 8;
    char mask = 1 << bitNUm;
    char* row = worldPart[(abs(point.y))];
    if(row[charNum] & mask)
        return true;
    return false;
}
// for a given coordinate set a bit to on or off
void setBit(const SDL_Point point, bool on, bool universe)
{
    bool state = getBitState(point, universe);
    if (state != on) // this will flip the state of a bit only if it's current state and requested state are opposite
        enterPoint(point, universe);
}
// populate the array with info on all 8 neighbours of a point.
void getNeigboursState(const SDL_Point point,bool neigbours[], bool universe)
{
    // from the north neigbour, clockwise, these are offsets to apply to the point to get neighbours coordinates
    static SDL_Point offsets[8] = { {0,-1}, {1, -1}, {1,0}, {1,1}, {0,1}, {-1,1}, {-1,0}, {-1,-1} };
    for ( int i = 0; i < 8; ++i )
    {
        SDL_Point neigbour;
        neigbour.x = point.x + offsets[i].x;
        neigbour.y = point.y + offsets[i].y;
        neigbours[i] = getBitState(neigbour, universe);
    }
    
}
 // apply the conway rules of the simulation.
// or feel free to experiment with different rules and see the results
void applyRules()
{
    // iterate throug each bit in the current universe and apply the rules to set bits accordingly in the target universe.
    // the conway game of life rules are as follows:
    // 1) an off bit turn on if it has exactly 3 neighbours
    // 2) an on bit turns off if it has less than 2 or more than 3 on neighbours
    
    for ( long long i = (-world_size - 3 ); i <= (world_size + 3); ++i) // check a bit further than world size
    {
        for ( long long j = (-world_size - 3 ); j <= (world_size + 3); ++j) // check a bit further than world size
        {
            SDL_Point point = {(int)j,(int)i};
            bool self = getBitState(point, universe);
            bool neighbours[8];
            getNeigboursState(point, neighbours, universe);
            int numNeighbours = 0; // the numbers of on neighbours.
            for (int num = 0; num < 8; ++ num)
            {
                if (neighbours[num])
                    ++numNeighbours;
            }
            // we have a point and it's 8 neighbours state, apply rules
            if (self) // our bit is on
            {
                if ((numNeighbours < 2) || (numNeighbours > 3))
                {
                    setBit(point, false, !universe);
                    --number_bits;
                }else // stay on
                    setBit(point, true, !universe);
                    
                
            }else // bit is off
            {
                
               if (numNeighbours == 3)
               {
                   setBit(point, true, !universe); // rule number 2
                   ++number_bits;
               }else // stay off
                 setBit(point, false, !universe);
            }
            
        }
    }
    
    universe = !universe;
}

// clear all the bits and start new
void clearWorld()
{
    world world0 = twoWorlds[0];
    world world1 = twoWorlds[1];
    long double width = ceill((long double)world_size / 8.f); // the worlds width in chars
    for (int i = 0; i < 4; ++i)
    {
        quarter quadA = world0.quad[i];
        quarter quadB = world1.quad[i];
        
        for (long long j = 0; j < world_size; ++j)
        {
            memset(quadA[j], 0, width);
            memset(quadB[j], 0, width);
        }
        world0.quad[i] = quadA;
        world1.quad[i] = quadB;
    }
    twoWorlds[0] = world0;
    twoWorlds[1] = world1;
    number_bits = 0;
}



































