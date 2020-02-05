#include "utils.h"


float clamp(float x, float a, float b) // keep x between a and b
{
  return x < a ? a : ( x > b ? b : x);
}

void freeTexture(SDL_Texture* tex)
{
    //Free texture if it exists
    if( tex )
    {
        SDL_DestroyTexture( tex );
        tex= NULL;
    }
}

// convert and integer to a string.
char* int_to_ascii(int num)
{
    int count = (num == 0) ? 1 : 0;
    // get the length of the number first
    int numCpy = num;
    while (numCpy)
    {
        numCpy /= 10;
        ++count;
    }
    char* str = malloc(count + 1); // +1 for the null terminator.
    str[count] = '\0';
    --count;
    str[0] = '0';
    while(num)
    {
        str[count] = (num % 10) + '0';
        num /= 10;
        --count;
    }
    // caller of this function has to release the memory allocated.
    
    return str;
}

// get the bigger of the two.
int max(int x, int y)
{
    return ( x >= y ) ? x : y;
}

int min(int x, int y)
{
    return ( x <= y ) ? x : y;
}

// create a texture from a string.
void loadFromRenderedTexture(SDL_Texture** tex, const char* str, TTF_Font* gFont, SDL_Color color ,SDL_Renderer* rend)
{
    // free pre exitsting texture.
    freeTexture(*tex);
    
    //Render text surface
    SDL_Surface* textSurface = TTF_RenderText_Solid( gFont, str, color );
    if( textSurface == NULL )
        printf( "Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError() );
    else
    {
        //Create texture from surface pixels
        *tex = SDL_CreateTextureFromSurface( rend, textSurface );
        if( tex == NULL )
            printf( "Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError() );
        //Get rid of old surface
        SDL_FreeSurface( textSurface );
    }
}

// draw the grid
void drawGrid(const SDL_Rect camera, const int grid_size, SDL_Renderer* renderer)
{
    // find the 'screen number' the camera is sitting on
    int screenX = (camera.x + camera.w / 2) / PLAY_AREA_WIDTH; // only interested in the integer part.
    int screenY = (camera.y + camera.h / 2) / camera.h;
    //draw lines from two screens before the current screen to two screens past, both above and bellow
    int fromX = (screenX - 2) * PLAY_AREA_WIDTH;
    int toX = (screenX + 2) * PLAY_AREA_WIDTH;
    
    // vertical lines
    for (int i = fromX,counter = 0; i <= toX; i += grid_size, ++counter)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0xc0, 0x00, 0x00);
        if (!(counter%10))
            SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0x00); // every tenth line make a brighter one, ease of viewing.
        SDL_RenderDrawLine(renderer, i-camera.x, ((screenY -2) * camera.h) -camera.y, i-camera.x, ((screenY + 2) * camera.h)-camera.y);
    }
    
    // horizontal lines
    for (int i = (screenY -2) * camera.h,counter = 0; i <= (screenY + 2) * camera.h; i += grid_size,++counter)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0xc0, 0x00, 0x00);
        if (!(counter%10))
            SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 0x00); // every tenth line make a brighter one, ease of viewing.
        
        SDL_RenderDrawLine(renderer, fromX-camera.x, i-camera.y, toX-camera.x, i-camera.y);
    }
}

//draws a flashing cursor.
void drawCursor(const float cursor_time, const SDL_Rect cursor, SDL_Renderer* renderer)
{
    if (cursor_time > 500)
    {
        SDL_SetRenderDrawColor(renderer, 0x00, 0xc0, 0x00, 0x00);
        SDL_RenderFillRect(renderer, &cursor);
    }
}

// get the quadrant of a point, i.e NW, SE, etc
enum Quadrant getQuad(const SDL_Point point)
{
    enum Quadrant qudrant;
    if (point.x >= 0) // east side
    {
        if (point.y >= 0)
            qudrant = SE;
        else
            qudrant = NE;
    }else // west side
    {
        if (point.y >= 0)
            qudrant = SW;
        else
            qudrant = NW;
    }
    return qudrant;
}
















