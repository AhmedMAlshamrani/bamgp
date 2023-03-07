#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


const int FPS = 30;
const int FRAME_DELAY = 1000/FPS;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const int BALL_RADIUS = 10;
const int BALL_SPEED = 5;


SDL_Window *window = nullptr;
SDL_Surface *sprite = nullptr;
SDL_Surface *backGroundImage = nullptr;
SDL_Surface *backBuffer = nullptr;
SDL_Surface *PaddleSprite = nullptr;


Mix_Chunk *hitSound = nullptr;
Mix_Music *backGroundMusic = nullptr;


TTF_Font *gameFont = nullptr;


float inputDirectionX = 0.0f;
float inputDirectionY = 0.0f;
float movementSpeed = 6.0f;


float ballXVel = 1.0f;
float ballYVel = 1.0f;
float ballMovementSpeed = 10.0f;


SDL_Rect ballRect;
SDL_Rect PaddleRect;


bool LoadFiles();
void FreeFiles();
bool ProgramIsRunning();
SDL_Surface* LoadImage(const char* fileName);
void DrawImage(SDL_Surface* image, SDL_Surface* destSurface, int x, int y);
void DrawImageFrame(SDL_Surface* image, SDL_Surface* destSurface, int x, int y, int width, int height, int frame);
void DrawText(SDL_Surface* surface, const char* string, int x, int y, TTF_Font* font, Uint8 r, Uint8 g, Uint8 b);
bool RectsOverlap(SDL_Rect rect1, SDL_Rect rect2);


int main(int argc, char* args[])
{
    std::cout << "Hello World" << std::endl;

    int highScore = 0;

    bool GameStarted = false;// added GameStarted
    ballRect.x = 0;
    ballRect.y = 250;
    ballRect.w = 20;
    ballRect.h = 20;

    PaddleRect.x = 0;
    PaddleRect.y = 550;
    PaddleRect.w = 100;
    PaddleRect.h = 90;

    if (ballRect.y >= SCREEN_HEIGHT)
{
    // The ball went behind the player - restart the game or update the high score
    if (SDL_GetTicks() > highScore)
    {
        highScore = SDL_GetTicks();
    }
    
    ballRect.x = 0;
    ballRect.y = 250;
    ballXVel = 1.0f;
    ballYVel = 1.0f;
}



    if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        std::cout << "SDL failed to init!" << std::endl;
        SDL_Quit();
        return 1;
    }


    // load font
    if (TTF_Init() == -1)
        return 2;


    // load sdl mixer
    if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0)
        return 3;


    // create window
    window = SDL_CreateWindow(
        "Pong!",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        0);
   
    backBuffer = SDL_GetWindowSurface(window);
   
    if (LoadFiles()) {


        //play sound
        Mix_PlayChannel(-1, hitSound, -1);
        while(ProgramIsRunning())
        {
            // get the time at the start of the frame
            int frameStart = SDL_GetTicks();
            // reset the back buffer with the back ground
            SDL_BlitSurface(backGroundImage, NULL, backBuffer, NULL);

            const Uint8* keystate = SDL_GetKeyboardState(NULL);
                if (keystate[SDL_SCANCODE_SPACE] && !GameStarted)
    {
        GameStarted = true;

        ballXVel = (rand() % 2 == 0) ? -1.0f : 1.0f;
        ballYVel = (rand() % 2 == 0) ? -1.0f : 1.0f;
    }

        // updated ball position
        ballRect.x += ballXVel * ballMovementSpeed;
        ballRect.y += ballYVel * ballMovementSpeed;

        // check for collision within boundaries
        if (ballRect.x < 0 || ballRect.x + ballRect.w > SCREEN_WIDTH)
        {
            ballXVel = -ballXVel;
        }
        if (ballRect.y < 0)
        {
            ballYVel = -ballYVel;
        }
        if (ballRect.y + ballRect.h > SCREEN_HEIGHT)
        {
            //Ball go behind player
            if (SDL_GetTicks() > highScore)
            {
                highScore = SDL_GetTicks();
            }
        }

        


            // Keyboard events
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                    case SDL_KEYDOWN:
                        switch (event.key.keysym.sym)
                        {
                            case SDLK_LEFT:
                                PaddleRect.x -= movementSpeed;
                                break;
                            case SDLK_RIGHT:
                                PaddleRect.x += movementSpeed;
                                break;
                        }
                        break;
                        case SDL_QUIT:
                        return 0;
                }
            }

            // Paddle Boundaries
            if (PaddleRect.x < 0)
            {
                PaddleRect.x = 0;
            }
            else if (PaddleRect.x + PADDLE_WIDTH > SCREEN_WIDTH)
            {
                PaddleRect.x = SCREEN_WIDTH - PADDLE_WIDTH;
            }


            // draw the image
            ballRect.x = (ballRect.x + (ballRect.w/2.0f) < SCREEN_WIDTH) ? (ballRect.x + (inputDirectionX * movementSpeed)) : -(ballRect.w/2.0f) + 1;
            ballRect.x = (ballRect.x > -(ballRect.w/2.0f)) ? ballRect.x : SCREEN_WIDTH - (ballRect.w/2.0f) - 1;


            ballRect.y = (ballRect.y + (ballRect.h/2.0f) < SCREEN_HEIGHT) ? (ballRect.y + (inputDirectionY * movementSpeed)) : -(ballRect.h/2.0f) + 1;
            ballRect.y = (ballRect.y > -(ballRect.h/2.0f)) ? ballRect.y : SCREEN_HEIGHT - (ballRect.h/2.0f) - 1;
            

            // draw ball
            DrawImage(sprite, backBuffer, ballRect.x, ballRect.y);

            DrawImage(PaddleSprite, backBuffer, PaddleRect.x, PaddleRect.y);

            // font
            DrawText(backBuffer, "score", 10, 10, gameFont, 255u, 260u, 230u);


            // end draw frame
            SDL_UpdateWindowSurface(window);
           
            // find the number of milliseconds
            int frameTime = SDL_GetTicks() - frameStart;


            // if we are rendering faster than FPS sleep the cpu
            if (frameTime < FRAME_DELAY)
                SDL_Delay(FRAME_DELAY - frameTime);
        }
    }


    FreeFiles();


    SDL_DestroyWindow(window);
    SDL_Quit();


    return 0;
}


SDL_Surface* LoadImage(const char* fileName)
{
    SDL_Surface* imageLoaded = NULL;
    SDL_Surface* processedImage = NULL;


    imageLoaded = SDL_LoadBMP(fileName);


    if(imageLoaded != NULL)
    {
        processedImage = SDL_ConvertSurface(imageLoaded, backBuffer->format, 0);
        SDL_FreeSurface(imageLoaded);


        if(processedImage != NULL)
        {
            Uint32 colorKey = SDL_MapRGB(processedImage->format, 0xFF, 0, 0xFF);
            SDL_SetColorKey(processedImage, SDL_TRUE, colorKey);
        }
    }


    return processedImage;
}


bool ProgramIsRunning()
{
    SDL_Event event;
    inputDirectionX = 0.0f;
    inputDirectionY = 0.0f;


    // input buffer
    const Uint8* keys = SDL_GetKeyboardState(NULL);

   /* if (keys[SDL_SCANCODE_LEFT])
        inputDirectionX = -1.0f;
   
    if (keys[SDL_SCANCODE_RIGHT])
        inputDirectionX = 1.0f;
   
    if (keys[SDL_SCANCODE_UP])
        inputDirectionY = -1.0f;
   
    if (keys[SDL_SCANCODE_DOWN])
        inputDirectionY = 1.0f;*/


    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_QUIT)
            return false;
        if (event.type == SDL_KEYDOWN) {
            /*if (event.key.keysym.sym == SDLK_LEFT)
                inputDirectionX = -1.0f;
            if (event.key.keysym.sym == SDLK_RIGHT)
                inputDirectionX = 1.0f;*/
        }
        if (event.type == SDL_MOUSEMOTION) {
            float x = event.motion.x;
            float y = event.motion.y;
        }
    }
   
    return true;
}


void DrawImage(SDL_Surface* image, SDL_Surface* destSurface, int x, int y)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;


    SDL_BlitSurface( image, NULL, destSurface, &destRect);
}


void DrawImageFrame(SDL_Surface* image, SDL_Surface* destSurface,
                    int x, int y, int width, int height, int frame)
{
    SDL_Rect destRect;
    destRect.x = x;
    destRect.y = y;


    int collumns = (*image).w/width;


    SDL_Rect sourceRect;
    sourceRect.y = (frame/collumns)*height;
    sourceRect.x = (frame%collumns)*width;
    sourceRect.w = width;
    sourceRect.h = height;


    SDL_BlitSurface(image, &sourceRect, destSurface, &destRect);
}


bool LoadFiles()
{
    // load images
    backGroundImage = LoadImage("assets/graphics/pong_background.bmp");
    sprite = LoadImage("assets/graphics/ball.bmp");
    PaddleSprite = LoadImage("assets/graphics/spaceship.bmp");


    if(sprite == nullptr)
        return false;

    if(PaddleSprite == nullptr)
        return false;


    if(backGroundImage == nullptr)
        return false;
   
    // load font
    gameFont = TTF_OpenFont("assets/fonts/alfphabet.ttf", 30);


    if (gameFont == nullptr)
        return false;


    // load sounds
    hitSound = Mix_LoadWAV("assets/sounds/Luis Fonsi ‒ Despacito (Lyrics _ Lyric Video) ft. Daddy Yankee.wav");
    // The song used is from YouTube: Luis Fonsi ‒ Despacito (Lyrics / Lyric Video) ft. Daddy Yankee
    if (hitSound == nullptr)
        return false;


    // load music
    backGroundMusic = Mix_LoadMUS("assets/sounds/Luis Fonsi ‒ Despacito (Lyrics _ Lyric Video) ft. Daddy Yankee.wav");
    // The song used is from YouTube: Luis Fonsi ‒ Despacito (Lyrics / Lyric Video) ft. Daddy Yankee
    if(backGroundMusic == nullptr)
        return false;


    return true;


}


void FreeFiles()
{
    if(sprite != nullptr)
    {
        SDL_FreeSurface(sprite);
        sprite = nullptr;
    }


    if(backGroundImage != nullptr)
    {
        SDL_FreeSurface(backGroundImage);
        backGroundImage = nullptr;
    }


    if (gameFont != nullptr)
    {
        TTF_CloseFont(gameFont);
        gameFont = nullptr;
    }
   
    if(hitSound !=nullptr)
    {
        Mix_FreeChunk(hitSound);
        hitSound = nullptr;
    }

    if(PaddleSprite != nullptr)
    {
        SDL_FreeSurface(PaddleSprite);
        PaddleSprite = nullptr;
    }
}


void DrawText(SDL_Surface* surface, const char* string, int x, int y, TTF_Font* font, Uint8 r, Uint8 g, Uint8 b)
{
    SDL_Surface* renderedText = NULL;


    SDL_Color color;


    color.r = r;
    color.g = g;
    color.b = b;


    renderedText = TTF_RenderText_Solid( font, string, color );


    SDL_Rect pos;


    pos.x = x;
    pos.y = y;


    SDL_BlitSurface( renderedText, NULL, surface, &pos );
    SDL_FreeSurface(renderedText);
}


bool RectsOverlap(SDL_Rect rect1, SDL_Rect rect2)
{
    if(rect1.x >= rect2.x+rect2.w)
        return false;


    if(rect1.y >= rect2.y+rect2.h)
        return false;


    if(rect2.x >= rect1.x+rect1.w)
        return false;


    if(rect2.y >= rect1.y+rect1.h)
        return false;


    return true;
}