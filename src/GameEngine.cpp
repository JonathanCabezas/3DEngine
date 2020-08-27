#ifndef _GameEngine
#define _GameEngine

#include <SDL2/SDL.h>

#include "Renderer.cpp"

enum COLOR
{
	BLACK        = 0x000000,
	GREY         = 0x808080,
	BLUE         = 0x0000ff,
	GREEN        = 0x008000,
	CYAN         = 0x00ffff,
	RED          = 0xcf0000,
	MAGENTA      = 0xff00ff,
	YELLOW 	     = 0xffff00,
	WHITE        = 0xffffff,
};

class GameEngine {
    protected:
        SDL_Window* window;
        SDL_Renderer* SDL_renderer;
        Renderer renderer;

    public:
        GameEngine() {

        }

        virtual bool OnCreate()						                	= 0;
        virtual bool OnKeyPressed(SDL_Keycode kc)	                    = 0;	
        virtual bool OnKeyReleased(SDL_Keycode kc)	                    = 0;	
        virtual bool OnUpdate(float fElapsedTime)		                = 0;	

        bool CreateWindow(const char* title, int width, int height) {
            if (SDL_Init(SDL_INIT_VIDEO) != 0) {
                fprintf(stderr,"Échec de l'initialisation de la SDL (%s)\n",SDL_GetError());
                return -1;
            }

            window = SDL_CreateWindow(title,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      SDL_WINDOWPOS_UNDEFINED,
                                      width,
                                      height,
                                      SDL_WINDOW_SHOWN);

            if(window) {
                SDL_renderer = SDL_CreateRenderer(window, -1, 0);
            } else {
                fprintf(stderr,"Erreur de création de la fenêtre: %s\n",SDL_GetError());
            }

            if (!SDL_renderer) {
                fprintf(stderr,"Erreur de création du moteur de rendu: %s\n",SDL_GetError());
            }

            renderer = Renderer(SDL_renderer, width, height);

            return 1;
        }

        void Start() {
            bool running = true;

            if (!OnCreate()) {
                running = false;
            }

            Uint32 lastUpdate = SDL_GetTicks();
            SDL_Event event;

            while(running) {
                Uint32 current = SDL_GetTicks();
                float fElapsedTime = (current - lastUpdate) / 1000.0f;

                while (SDL_PollEvent(&event)) {
                    switch (event.type) {
                        case SDL_KEYDOWN:
                            OnKeyPressed(event.key.keysym.sym);
                            break;
                        case SDL_KEYUP:
                            OnKeyReleased(event.key.keysym.sym);
                            break;
                        case SDL_QUIT:
                            running = false;
                            break;
                    }
                }

                if (!OnUpdate(fElapsedTime)) {
                    running = false;
                }

                SDL_RenderPresent(SDL_renderer);
                lastUpdate = current;
            }

            SDL_DestroyWindow(window);
            SDL_DestroyRenderer(SDL_renderer);
            SDL_Quit();
        }
};

#endif