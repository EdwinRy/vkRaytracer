#include <vulkan/vulkan.h>
#pragma once
namespace VkRaytracer
{
    struct QueueFamilyIndices
    {
        unsigned int graphics;
        bool foundGraphics;
        unsigned int present;
        bool foundPresent;
    };

    class pVkManager
    {
        public:
        VkApplicationInfo vkAppInfo;
        VkInstanceCreateInfo createInfo;
        VkInstance instance;
        VkPhysicalDevice physicalDevice;
        VkDevice device;
        VkSurfaceKHR surface;
        QueueFamilyIndices familyIndices;

        QueueFamilyIndices GetPhysicalDeviceQueueFamilies(VkPhysicalDevice dev);
        unsigned long long RatePhysicalDevice(VkPhysicalDevice device);
    };

}