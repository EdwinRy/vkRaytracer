#include "Window.hpp"
#include "VkManager.hpp"
#include <iostream>

using namespace VkRaytracer;
int main()
{
    Window window = Window("title", 500, 500);
    window.Open();

    VkManager vkm(window);

    vkm.InitVk();

    while(window.isOpen)
    {
        window.Update();
    }

    window.Close();

    std::cout << "ok\n";
    return 0;
}