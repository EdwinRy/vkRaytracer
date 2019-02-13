#pragma once
#include <iostream>
namespace VkRaytracer
{
    class Window
    {
        public:
        Window(
            std::string title = "vkRaytracer", 
            unsigned int width = 600, 
            unsigned int height = 400,
            int winPosX = -1,
            int winPosY = -1
        );

        void *GetWindow(){return this->sdlWindow;}
        bool Open();
        void Update();
        void Close();

        bool isOpen;

        private:
        void *sdlWindow;
        char* title;
        unsigned int width, height, winPosX, winPosY;

    };
}