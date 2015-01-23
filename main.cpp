/*This source code copyrighted by Lazy Foo' Productions (2004-2013)
and may not be redistributed without written permission.*/

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Texture wrapper class
class CTexture
{
public:
   CTexture();
   ~CTexture();

   bool LoadFromFile(std::string sPath);

   bool LoadFromRenderedText(std::string sTextureText, SDL_Color vTextColor);

   void Free();

   void SetColor(Uint8 iRed, Uint8 iGreen, Uint8 iBlue);

   void SetBlendMode(SDL_BlendMode EBlending);

   void SetAlpha(Uint8 iAlpha);

   void Render(int iX, int iY, SDL_Rect* pClip, double dAngle, SDL_Point* pCenter, SDL_RendererFlip eFlip);

   int GetWidth();
   int GetHeight();

private:
   SDL_Texture* m_pTexture;

   int m_iWidth;
   int m_iHeight;
};

//The window we'll be rendering to
SDL_Window* fg_pWindow = NULL;

//The window renderer
SDL_Renderer* fg_pRenderer = NULL;

//Font
TTF_Font* fg_pFont = NULL;

//Walking animation
CTexture fg_TextTexture;

//Starts up SDL and creates a window
bool Init();

//Loads individual image as texture
SDL_Texture* LoadTexture(std::string sPath);

//Loads media
bool LoadMedia();

//Frees media shuts down SDL
void Close();

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

   double dDegrees = 0;
   SDL_RendererFlip eFlipType = SDL_FLIP_NONE;

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
         SDL_SetRenderDrawColor(fg_pRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
         SDL_RenderClear(fg_pRenderer);

         fg_TextTexture.Render((SCREEN_WIDTH - fg_TextTexture.GetWidth()) / 2, (SCREEN_HEIGHT - fg_TextTexture.GetHeight()) / 2, 
                                NULL, 0, NULL, SDL_FLIP_NONE);

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

   fg_pRenderer = SDL_CreateRenderer(fg_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

   //Initialize SDL_ttf
   if (TTF_Init() == -1)
   {
      printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
      return false;
   }

   return true;
}

bool LoadMedia()
{
   fg_pFont = TTF_OpenFont("lazy.ttf", 28);
   if (fg_pFont == NULL)
   {
      printf("Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError());
      return false;
   }

   SDL_Color vTextColor = { 0, 0, 0 };
   if (!fg_TextTexture.LoadFromRenderedText("The quick brown fox jumps over the lazy dog", vTextColor))
   {
      printf("Failed to render text texture\n");
      return false;
   }

   return true;
}

void Close()
{
   fg_TextTexture.Free();

   TTF_CloseFont(fg_pFont);
   fg_pFont = NULL;

   //Destroy window
   SDL_DestroyRenderer(fg_pRenderer);
   SDL_DestroyWindow(fg_pWindow);
   fg_pWindow = NULL;
   fg_pRenderer = NULL;

   //Quit SDL subsystems
   TTF_Quit();
   IMG_Quit();
   SDL_Quit();
}

CTexture::CTexture()
{
   m_pTexture = NULL;
   m_iWidth = 0;
   m_iHeight = 0;
}

CTexture::~CTexture()
{
   Free();
}

bool CTexture::LoadFromFile(std::string sPath)
{
   // Get rid of preexisting texture
   Free();

   //Load image at specified path
   SDL_Surface* pLoadedSurface = IMG_Load(sPath.c_str());
   if (pLoadedSurface == NULL)
   {
      printf("Unable to load image %s! SDL_image Error: %s\n", sPath.c_str(), IMG_GetError());
      return false;
   }

   //Color key image
   SDL_SetColorKey(pLoadedSurface, SDL_TRUE, SDL_MapRGB(pLoadedSurface->format, 0, 0xFF, 0xFF));

   //Create texture from surface pixels
   m_pTexture = SDL_CreateTextureFromSurface(fg_pRenderer, pLoadedSurface);
   if (m_pTexture == NULL)
   {
      printf("Unable to create texture from %s! SDL Error: %s\n", sPath.c_str(), SDL_GetError());
      return false;
   }

   m_iWidth = pLoadedSurface->w;
   m_iHeight = pLoadedSurface->h;

   //Get rid of old loaded surface
   SDL_FreeSurface(pLoadedSurface);

   return true;
}

bool CTexture::LoadFromRenderedText(std::string sTextureText, SDL_Color vTextColor)
{
   Free();

   SDL_Surface* pTextSurface = TTF_RenderText_Solid(fg_pFont, sTextureText.c_str(), vTextColor);
   if (pTextSurface == NULL)
   {
      printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
      return false;
   }

   m_pTexture = SDL_CreateTextureFromSurface(fg_pRenderer, pTextSurface);
   if (m_pTexture == NULL)
   {
      printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
      return false;
   }

   m_iWidth = pTextSurface->w;
   m_iHeight = pTextSurface->h;

   SDL_FreeSurface(pTextSurface);

   return true;
}

void CTexture::Free()
{
   if (m_pTexture != NULL)
   {
      SDL_DestroyTexture(m_pTexture);
      m_pTexture = NULL;
      m_iWidth = 0;
      m_iHeight = 0;
   }
}

void CTexture::SetColor(Uint8 iRed, Uint8 iGreen, Uint8 iBlue)
{
   SDL_SetTextureColorMod(m_pTexture, iRed, iGreen, iBlue);
}

void CTexture::SetBlendMode(SDL_BlendMode EBlending)
{
   SDL_SetTextureBlendMode(m_pTexture, EBlending);
}

void CTexture::SetAlpha(Uint8 iAlpha)
{
   SDL_SetTextureAlphaMod(m_pTexture, iAlpha);
}

void CTexture::Render(int iX, int iY, SDL_Rect* pClip, double dAngle, SDL_Point* pCenter, SDL_RendererFlip eFlip)
{
   // Set rendering space and render to screen
   SDL_Rect vRenderQuad = { iX, iY, m_iWidth, m_iHeight };

   // Set slip rendering dimensions
   if (pClip != NULL)
   {
      vRenderQuad.w = pClip->w;
      vRenderQuad.h = pClip->h;
   }

   SDL_RenderCopyEx(fg_pRenderer, m_pTexture, pClip, &vRenderQuad, dAngle, pCenter, eFlip);
}

int CTexture::GetWidth()
{
   return m_iWidth;
}

int CTexture::GetHeight()
{
   return m_iHeight;
}