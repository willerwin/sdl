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

//The surface contained by the window
SDL_Surface* fg_pScreenSurface = NULL;

//Current displayed image
SDL_Surface* fg_pCurrentSurface = NULL;

//Starts up SDL and creates a window
bool Init();

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

         // Apply the image stretched
         SDL_Rect vStretchRect;
         vStretchRect.x = 0;
         vStretchRect.y = 0;
         vStretchRect.w = SCREEN_WIDTH;
         vStretchRect.h = SCREEN_HEIGHT;
         SDL_BlitScaled(fg_pCurrentSurface, NULL, fg_pScreenSurface, &vStretchRect);

         //Update the surface
         SDL_UpdateWindowSurface(fg_pWindow);
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

   //Initialize PNG loading
   int iImgFlags = IMG_INIT_PNG;
   if (!(IMG_Init(iImgFlags) & iImgFlags))
   {
      printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
      return false;
   }

   //Get window surface
   fg_pScreenSurface = SDL_GetWindowSurface(fg_pWindow);

   return true;
}

bool LoadMedia()
{
   // Load default surface
   fg_pCurrentSurface = LoadSurface("loaded.png");
   if (fg_pCurrentSurface == NULL)
   {
      printf("Failed to load stretching image!\n");
      return false;
   }

   return true;
}

void Close()
{
   //Deallocate surface
   SDL_FreeSurface(fg_pCurrentSurface);
   fg_pCurrentSurface = NULL;

   //Destroy window
   SDL_DestroyWindow(fg_pWindow);
   fg_pWindow = NULL;

   //Quit SDL subsystems
   SDL_Quit();
}

SDL_Surface* LoadSurface(std::string sPath)
{
   //The final optimized image
   SDL_Surface* pOptimizedSurface = NULL;

   //Load image at specified path
   SDL_Surface* pLoadedSurface = IMG_Load(sPath.c_str());
   if (pLoadedSurface == NULL)
      printf("Unable to load image %s! SDL Error: %s\n", sPath.c_str(), IMG_GetError());
   else
   {
      //Convert surface to screen format
      pOptimizedSurface = SDL_ConvertSurface(pLoadedSurface, fg_pScreenSurface->format, NULL);
      if (pOptimizedSurface == NULL)
         printf("Unable to optimize image %s! SDL Error: %s\n", sPath.c_str(), SDL_GetError());

      //Get rid of old loaded surface
      SDL_FreeSurface(pLoadedSurface);
   }

   return pOptimizedSurface;
}