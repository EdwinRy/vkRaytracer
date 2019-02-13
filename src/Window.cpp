#include "Window.hpp"
#include <SDL2/SDL.h>

namespace VkRaytracer
{
    Window::Window(
        std::string title, 
        unsigned int width, 
        unsigned int height,
        int winPosX,
        int winPosY
    )
    {
        this->title = (char*)title.data();
        this->width = width;
        this->height = height;
        
        this->winPosX = winPosX == -1 ? SDL_WINDOWPOS_CENTERED : winPosX;
        this->winPosY = winPosY == -1 ? SDL_WINDOWPOS_CENTERED : winPosY;

    }

    bool Window::Open()
    {
        if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        {
            std::cout << "Could not initialise SDL!\n";
            return false;
        }

        this->sdlWindow = SDL_CreateWindow(
            this->title,
            this->winPosX, this->winPosY,
            this->width, this->height,
            SDL_WINDOW_VULKAN
        );

        if(this->sdlWindow == NULL)
        {
            printf("Could not create SDL window: %s\n", SDL_GetError());
            return false;
        }

        this->isOpen = true;
        return true;
    }

    void Window::Update()
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            if(event.type == SDL_QUIT)
            {
                this->Close();
            }
        }
        SDL_Delay(1);

    }

    void Window::Close()
    {
        if(isOpen)
        {
            std::cout << "Closing window\n";
            SDL_DestroyWindow((SDL_Window*)this->sdlWindow);
            SDL_Quit();
            this->isOpen = false;
        }
    }

}