#include <iostream>
#include "vulkan/vulkan.h"

#define ASSERT_VULKAN(val)\
        if (val != VK_SUCCESS) {\
            std::cout << "Error!";\
        }

VkInstance instance;

void printStats(VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::cout << "Name: " << properties.deviceName << std::endl;
    uint32_t apiVer = properties.apiVersion;
    std::cout << "API Version " << VK_VERSION_MAJOR(apiVer) << "."
                                << VK_VERSION_MINOR(apiVer) << "."
                                << VK_VERSION_PATCH(apiVer) << std::endl;
    std::cout << "Driver Version " << properties.driverVersion << std::endl;
    std::cout << "Vendor ID: " << properties.vendorID << std::endl;
    std::cout << "Device ID: " << properties.deviceID << std::endl;
    std::cout << "Device Type: " << properties.deviceType << std::endl;

    std::cout << std::endl;
}

int main()
{
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Super Vulkan Engine Turbo Mega";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = 0;
    instanceInfo.ppEnabledLayerNames = NULL;
    instanceInfo.enabledExtensionCount = 0;
    instanceInfo.ppEnabledExtensionNames = NULL;


    VkResult result = vkCreateInstance(&instanceInfo, NULL, &instance);

    ASSERT_VULKAN(result);

    uint32_t amountOfPhysicalDevices = 0;
    vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, NULL); // when we pass NULL 
    // instead of array with physical graphic cards the vkEnumeratePhysicalDevices 
    // function will write into amountOfPhysicalDevices a value with number of graphic cards installed into the computer

    VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices]; // define
    // an array of size equal to amountOfPhysicalDevices

    vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices); // when we call vkEnumeratePhysicalDevices the second time and we pass it
    // physicalDevices array instead of NULL and amountOfPhysicalDevices the physicalDevices[] is
    // populated with available physical graphic cards

    for (int i = 0; i < amountOfPhysicalDevices; i++)
    {
        printStats(physicalDevices[i]);
    }

    return 0;
}
