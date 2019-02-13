#include "pVkManager.hpp"
#include "VkManager.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>
namespace VkRaytracer
{
    typedef unsigned int uint32;

    void VkManager::InitVk()
    {
        this->GenerateApplicationInfo();
        this->GenerateExtentionList();
        this->CreateVkInstance();
        this->ChoosePhysicalDevice();
        this->CreateLogicalDevice();
        this->SetupSurface();
        this->CreatePresentationQueue();
    }

    void VkManager::GenerateApplicationInfo()
    {
        this->pvk = new pVkManager();
        this->pvk->vkAppInfo.apiVersion = VK_API_VERSION_1_0;
        this->pvk->vkAppInfo.applicationVersion = VK_MAKE_VERSION(1,0,0);
        this->pvk->vkAppInfo.engineVersion = VK_MAKE_VERSION(1,0,0);
        this->pvk->vkAppInfo.pApplicationName = "VkRaytracer Test";
        this->pvk->vkAppInfo.pEngineName = "VkRaytracer";
        this->pvk->vkAppInfo.pNext = nullptr;
        this->pvk->vkAppInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    }

    void VkManager::GenerateExtentionList()
    {
        unsigned int extensionCount;

        if(!SDL_Vulkan_GetInstanceExtensions(
            (SDL_Window*)this->window->GetWindow(),
            &extensionCount,
            nullptr
        ));
        {
            std::cout << "Could not get count of vk instance extensions\n";
        }

        std::vector<const char*> extensionNames(extensionCount);

        SDL_Vulkan_GetInstanceExtensions(
            (SDL_Window*)this->window->GetWindow(),
            &extensionCount,
            extensionNames.data()
        );

        printf("%i Vulkan extensions found\n", extensionCount);

        for(const char* extension : extensionNames)
            printf("Found extension: %s\n", extension);

        this->extensions = extensionNames;
    }

    bool VkManager::CreateVkInstance()
    {
        pvk->createInfo.enabledExtensionCount = extensions.size();
        pvk->createInfo.enabledLayerCount = 0;
        pvk->createInfo.flags = 0;
        pvk->createInfo.pApplicationInfo = &this->pvk->vkAppInfo;
        pvk->createInfo.pNext = nullptr;
        pvk->createInfo.ppEnabledExtensionNames = extensions.data();
        pvk->createInfo.ppEnabledLayerNames = nullptr;
        pvk->createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        if(vkCreateInstance(
            &this->pvk->createInfo, nullptr, &this->pvk->instance
        ) != VK_SUCCESS)
        {
            std::cout << "Could not create vulkan instance!\n";
        }

        std::cout << "Vulkan instance created successfully\n";
    }

    bool VkManager::ChoosePhysicalDevice()
    {
        unsigned int physicalDeviceCount = 0;

        if(vkEnumeratePhysicalDevices(
                this->pvk->instance, 
                &physicalDeviceCount, 
                nullptr
            ) != VK_SUCCESS)
        {
            std::cout << "Failed to enumerate physical devices\n";
        }

        if(physicalDeviceCount == 0)
        {
            std::cout << "Failed to find a card with Vulkan support\n";
            return false;
        }

        std::cout<< "physical devices: " << physicalDeviceCount << "\n";

        std::vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);

        vkEnumeratePhysicalDevices(this->pvk->instance, 
            &physicalDeviceCount, 
            physicalDevices.data()
        );

        unsigned long long maxDevScore = 0;
        for(VkPhysicalDevice dev : physicalDevices)
        {
            unsigned long long score = this->pvk->RatePhysicalDevice(dev);
            if(score > maxDevScore)
            {
                maxDevScore = score;
                this->pvk->physicalDevice = dev;
            }
        }
    }

    unsigned long long pVkManager::RatePhysicalDevice(VkPhysicalDevice device)
    {
        unsigned long long score = 0;
        VkPhysicalDeviceProperties dProp = {};
        //VkPhysicalDeviceFeatures dFeat;
        vkGetPhysicalDeviceProperties(device, &dProp);
        //vkGetPhysicalDeviceFeatures(device, &dFeat);

        //If the device is a discrete gpu
        if(dProp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }

        //Increase score by maximum texture size (larger textures == better)
        score += dProp.limits.maxImageDimension2D;
        std::cout << "Found device: " << dProp.deviceName << "\n";
        std::cout << "Device score: " << score << "\n";

        if(!GetPhysicalDeviceQueueFamilies(device).foundGraphics)
        {
            std::cout << "Device: " << dProp.deviceName 
            << " not suitable for graphics\n";
            return 0;
        }
        if(!GetPhysicalDeviceQueueFamilies(device).foundPresent)
        {
            std::cout << "Device: " << dProp.deviceName 
            << " not suitable for presentation\n";
            return 0;
        }
        return score;
    }

    QueueFamilyIndices pVkManager::GetPhysicalDeviceQueueFamilies(
        VkPhysicalDevice dev)
    {
        unsigned int queueFamCount;
        vkGetPhysicalDeviceQueueFamilyProperties(dev,&queueFamCount,nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            dev, &queueFamCount, queueFamilies.data()
        );

        QueueFamilyIndices indices = {};
        unsigned long long i = 0;
        for(VkQueueFamilyProperties queueFam : queueFamilies)
        {
            if
            (
                queueFam.queueCount > 0 &&
                queueFam.queueFlags & VK_QUEUE_GRAPHICS_BIT
            )
            {
                indices.graphics = i;
                indices.foundGraphics = true;
            }
            if(indices.foundGraphics) break;
            i++;
        }

        return indices;
    }

    bool VkManager::CreateLogicalDevice()
    {
        QueueFamilyIndices familyIndices = 
            pvk->GetPhysicalDeviceQueueFamilies(this->pvk->physicalDevice);

        this->pvk->familyIndices = familyIndices;

        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = familyIndices.graphics;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pNext = nullptr;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures devFeatures;
        vkGetPhysicalDeviceFeatures(this->pvk->physicalDevice, &devFeatures);

        VkDeviceCreateInfo devCreateInfo = {};
        devCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        devCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        devCreateInfo.queueCreateInfoCount = 1;
        devCreateInfo.pEnabledFeatures = &devFeatures;
        devCreateInfo.flags = 0;

        if
        (
            vkCreateDevice(
                this->pvk->physicalDevice,
                &devCreateInfo,
                nullptr,
                &this->pvk->device
            ) != VK_SUCCESS
        )
        {
            std::cout << "Failed to create Vulkan logical device\n";
            this->~VkManager();
            exit(-1);
        }

        VkQueue graphicsQueue = {};
        vkGetDeviceQueue(
            this->pvk->device,familyIndices.graphics,0, &graphicsQueue
        );

        return true;
    }

    bool VkManager::SetupSurface()
    {
        this->pvk->surface = {};

        if(!SDL_Vulkan_CreateSurface(
                (SDL_Window*)this->window->GetWindow(), 
                this->pvk->instance, &this->pvk->surface
        ))
        {
            printf("Could not create Vulkan surface\n");
            printf("Error: %s\n", SDL_GetError());
        }

        printf("Created Vulkan surface\n");

    }

    void VkManager::CreatePresentationQueue()
    {
        VkQueue presentQueue;

        std::vector<VkDeviceQueueCreateInfo> queueCreateInformation;
        std::vector<unsigned int> queueFamilies = 
        {
            this->pvk->familyIndices.graphics,
            this->pvk->familyIndices.present
        };

        float queuePriority = 1;

        for(unsigned int queueFamily : queueFamilies)
        {
            VkDeviceQueueCreateInfo devQueueCreateInfo = {};
            devQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
            devQueueCreateInfo.queueFamilyIndex = queueFamily;
            devQueueCreateInfo.queueCount = 1;
            devQueueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInformation.push_back(devQueueCreateInfo);
        }

        vkGetDeviceQueue(
            pvk->device, 
            pvk->familyIndices.present, 
            0, 
            &presentQueue
        );



    }

}