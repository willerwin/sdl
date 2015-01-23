/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//The window we'll be rendering to
SDL_Window* fg_pWindow = NULL;

//The window renderer
SDL_Renderer* fg_pRenderer = NULL;

//Current displayed texture
SDL_Texture* fg_pTexture = NULL;

//Starts up SDL and creates a window
bool Init();

//Loads individual image as texture
SDL_Texture* LoadTexture(std::string sPath);

//Loads media
bool LoadMedia();

//Frees media shuts down SDL
void Close();

//Loads individual image
SDL_Surface* LoadSurface(std::string sPath);

int main( int argc, char* args[] )
{
	//Start up SDL and create window
   if (!Init())
   {
      printf("Failed to initialize\n");
      return 1;
   }

   //Load Media
   if (!LoadMedia())
   {
      printf("Failed to load media!\n");
      return 1;
   }

   // Main loop flag
   bool bQuit = false;

   // Event handler
   SDL_Event uEvent;

   //While application is running
   while (!bQuit)
   {
      //Handle events on queue
      while (SDL_PollEvent(&uEvent) != 0)
      {
         // User requests quit
         if (uEvent.type == SDL_QUIT)
         {
            bQuit = true;
         }

         //Clear screen
         SDL_RenderClear(fg_pRenderer);

         //Render texture to screen
         SDL_RenderCopy(fg_pRenderer, fg_pTexture, NULL, NULL);

         //Update screen
         SDL_RenderPresent(fg_pRenderer);
      }
   }

   //Free resources and close SDL
	Close();

	return 0;
}

bool Init()
{
   //Initialize SDL
   if (SDL_Init(SDL_INIT_VIDEO) < 0)
   {
      printf("SDL could not initialize! SDL_Error:%s\n", SDL_GetError());
      return false;
   }

   // Create window
   fg_pWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, 
                                 SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
   if (fg_pWindow == NULL)
   {
      printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
      return false;
   }

   fg_pRenderer = SDL_CreateRenderer(fg_pWindow, -1, SDL_RENDERER_ACCELERATED);
   if (fg_pRenderer == NULL)
   {
      printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
      return false;
   }

   //Initialize renderer color
   SDL_SetRenderDrawColor(fg_pRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

   //Initialize PNG loading
   int iImgFlags = IMG_INIT_PNG;
   if (!(IMG_Init(iImgFlags) & iImgFlags))
   {
      printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
      return false;
   }

   return true;
}

bool LoadMedia()
{
   // Load default surface
   fg_pTexture = LoadTexture("texture.png");
   if (fg_pTexture == NULL)
   {
      printf("Failed to load texture image!\n");
      return false;
   }

   return true;
}

void Close()
{
   //Deallocate surface
   SDL_DestroyTexture(fg_pTexture);
   fg_pTexture = NULL;

   //Destroy window
   SDL_DestroyRenderer(fg_pRenderer);
   SDL_DestroyWindow(fg_pWindow);
   fg_pWindow = NULL;
   fg_pRenderer = NULL;

   //Quit SDL subsystems
   IMG_Quit();
   SDL_Quit();
}

SDL_Texture* LoadTexture(std::string sPath)
{
   //The final texture
   SDL_Texture* pNewTexture = NULL;

   //Load image at specified path
   SDL_Surface* pLoadedSurface = IMG_Load(sPath.c_str());
   if (pLoadedSurface == NULL)
   {
      printf("Unable to load image %s! SDL_image Error: %s\n", sPath.c_str(), IMG_GetError());
   }
   else
   {
      //Create texture from surface pixels
      pNewTexture = SDL_CreateTextureFromSurface(fg_pRenderer, pLoadedSurface);
      if (pNewTexture == NULL)
         printf("Unable to create texture from %s! SDL Error: %s\n", sPath.c_str(), SDL_GetError());

      //Get rid of old loaded surface
      SDL_FreeSurface(pLoadedSurface);
   }

   return pNewTexture;
}