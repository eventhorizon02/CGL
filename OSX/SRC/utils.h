// a collection of utillity functions

#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include "def.h"


float clamp(float x, float a, float b); //keep x between a and b
void freeTexture(SDL_Texture*); // free a texture.
char* int_to_ascii(int); // convert and integer to a string.
int max(int,int); // get the bigger of the two.
int min(int,int); // get the smaller of the two.
void loadFromRenderedTexture(SDL_Texture**, const char*, TTF_Font*, SDL_Color ,SDL_Renderer* ); // create a texture from a string.
void drawGrid(const SDL_Rect camera,const int grid_size ,SDL_Renderer* renderer);
void drawCursor(const float cursor_time, const SDL_Rect cursor, SDL_Renderer* renderer);
enum Quadrant getQuad(const SDL_Point); // get the quadrant of a point, i.e NW, SE, etc
