#include "Window.hpp"
#include <vector>
#pragma once
class pVkManager{};
namespace VkRaytracer
{
    class VkManager
    {
        public:
        VkManager(Window &window){this->window = &window;}
        // ~VkManager();
        void InitVk();
        void Cleanup();
        pVkManager* GetPVK(){return this->pvk;}

        private:
        pVkManager* pvk;
        Window *window;

        std::vector<const char*> extensions;

        void GenerateApplicationInfo();
        void GenerateExtentionList();
        bool CreateVkInstance();
        bool ChoosePhysicalDevice();
        bool CreateLogicalDevice();
        bool SetupSurface();
        void CreatePresentationQueue();
    };
}
