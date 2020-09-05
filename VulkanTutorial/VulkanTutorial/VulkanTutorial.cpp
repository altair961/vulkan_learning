#include <iostream>
#include "vulkan/vulkan.h"
#include <vector>

#define ASSERT_VULKAN(val)\
        if (val != VK_SUCCESS) {\
            __debugbreak();\
        }

VkInstance instance;
VkDevice device;

void printStats(VkPhysicalDevice &device) {
    VkPhysicalDeviceProperties properties;
    vkGetPhysicalDeviceProperties(device, &properties);

    std::cout << "Name: " << properties.deviceName << std::endl;
    uint32_t apiVer = properties.apiVersion;

    std::cout << "API version: " << "\t\t\t" << VK_VERSION_MAJOR(apiVer) << "." << VK_VERSION_MINOR(apiVer) << "." << VK_VERSION_PATCH(apiVer) << std::endl;
    std::cout << "Driver Version: " << "\t\t" << properties.driverVersion << std::endl;
    std::cout << "Vendor ID: " << "\t\t\t" << properties.vendorID << std::endl;
    std::cout << "Device ID: " << "\t\t\t" << properties.deviceID << std::endl;
    std::cout << "Device Type: " << "\t\t\t" << properties.deviceType << std::endl;
    std::cout << "descreteQueuePriorities: " << "\t" << properties.limits.discreteQueuePriorities << std::endl;

    VkPhysicalDeviceFeatures features;
    vkGetPhysicalDeviceFeatures(device, &features);
    std::cout << "Geometry Shader: " << "\t\t" << features.geometryShader << std::endl;

    VkPhysicalDeviceMemoryProperties memProp;
    vkGetPhysicalDeviceMemoryProperties(device, &memProp); // &memProp means "to take address of memProp variable"

    uint32_t amountOfQueueFamilies = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, NULL); // when we pass NULL 
    // instead of array with queue family properties the vkGetPhysicalDeviceQueueFamilyProperties
    // function will write into amountOfQueueFamilies a value with number of queue family properties available in the graphics card
    
    VkQueueFamilyProperties *familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies]; // new operator returns address of memory and we store it into so called pointer that is *someName
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties); // here we put familyProperties without &, because it is a pointer - it is already an address of memory where object is located
    // when we call vkGetPhysicalDeviceQueueFamilyProperties the second time and we pass it
    // familyProperties array instead of NULL and amountOfQueueFamilies the familyProperties[] is
    // populated with available queue family properties available in the graphics card

    std::cout << "Amount of queue families: " << "\t" << amountOfQueueFamilies << std::endl;
    for (int i = 0; i < amountOfQueueFamilies; i++)
    {
        std::cout << std::endl;
        std::cout << "Queue Family #" << "\t\t\t\t" << i << std::endl;
        std::cout << "VK_QUEUE_GRAPHICS_BIT " << "\t\t\t" << ((familyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_COMPUTE_BIT " << "\t\t\t" << ((familyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_TRANSFER_BIT " << "\t\t\t" << ((familyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << std::endl;
        std::cout << "VK_QUEUE_SPARSE_BINDING_BIT " << "\t\t" << ((familyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0) << std::endl;
        std::cout << "Queue Count: " << "\t\t\t\t" << familyProperties[i].queueCount << std::endl;
        std::cout << "Timestamp Valid Bits: " << "\t\t\t" << familyProperties[i].timestampValidBits << std::endl;
        uint32_t width = familyProperties[i].minImageTransferGranularity.width;
        uint32_t height = familyProperties[i].minImageTransferGranularity.height;
        uint32_t depth = familyProperties[i].minImageTransferGranularity.depth;
        std::cout << "Min Image Timestamp Granularity: " << "\t" << width << ", " << height << ", " << depth << std::endl;
    }

    std::cout << std::endl;

    delete[] familyProperties;
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

    uint32_t amountOfLayers = 0;
    vkEnumerateInstanceLayerProperties(&amountOfLayers, NULL);
    VkLayerProperties* layers = new VkLayerProperties[amountOfLayers];
    vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);

    std::cout << "Amount of Instance Layers: " << amountOfLayers << std::endl;
    for (int i = 0; i < amountOfLayers; i++)
    {
        std::cout << std::endl;
        std::cout << "Layers Name: " << layers[i].layerName << std::endl;
        std::cout << "Specification Version: " <<layers[i].specVersion << std::endl;
        std::cout << "Implementation Version: " <<layers[i].implementationVersion << std::endl;
        std::cout << "Description: " <<layers[i].description << std::endl;
    }
    std::cout << std::endl;

    uint32_t amountOfExtensions = 0; // Extensions are not nice to haves. Theay are essential part of Vulkan. Since it is crossplatform it must have some platform specific functionalities. E.g. some pieces of functionality related to window drawing. They are essential but differ for each platform.
    vkEnumerateInstanceExtensionProperties(NULL, &amountOfExtensions, NULL);
    VkExtensionProperties *extensions = new VkExtensionProperties[amountOfExtensions];
    vkEnumerateInstanceExtensionProperties(NULL, &amountOfExtensions, extensions);

    std::cout << std::endl;

    std::cout << "Amount of Extensions: " << amountOfExtensions << std::endl;
    for (int i = 0; i < amountOfExtensions; i++)
    {
        std::cout << std::endl;
        std::cout << "Name of extension: " << extensions[i].extensionName << std::endl;
        std::cout << "Spec Version: " << extensions[i].specVersion << std::endl;
    }

    std::cout << std::endl;

    //Normaly we should check if the validation layer is presented in the graphics card. But this layer is presented in every graphics card, so we do not check.
    const std::vector<const char*> validationLayers = { //set of c-strings
        "VK_LAYER_LUNARG_standard_validation"
    };

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = NULL;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
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

    float queuePrios[] = {1.0f, 1.0f, 1.0f, 1.0f };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = NULL;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; // Here, instead of hard coding 0 we actually need to find out what index has queue family that we are interested in. VkQueueFamilyProperties[amountOfQueueFamilies] contains all queue families.
    deviceQueueCreateInfo.queueCount = 1; // Here, instead of hardcoding 1 we need to find out the valid value.
    deviceQueueCreateInfo.pQueuePriorities = queuePrios;

    VkPhysicalDeviceFeatures usedFeatures = {};

    VkDeviceCreateInfo deviceCreateInfo;

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = NULL;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = NULL;
    deviceCreateInfo.enabledExtensionCount = 0;
    deviceCreateInfo.ppEnabledExtensionNames = NULL;
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    //pick "best suitable physical device" here instead of just taking the first one
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, NULL, &device);
    ASSERT_VULKAN(result);

    return 0;
}
