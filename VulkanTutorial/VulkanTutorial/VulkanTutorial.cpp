#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_INCLUDE_VULKAN // here we explicitly say that we want to work with Vulkan instead of default OpenGL
#include <GLFW\glfw3.h>
//#include "vulkan/vulkan.h" we do not need it as we can access Vulkan through GLFW
#include <vector>
#include <iostream>
#include <fstream>

#define ASSERT_VULKAN(val)\
        if (val != VK_SUCCESS) {\
            __debugbreak();\
        }

VkInstance instance;
VkSurfaceKHR surface;
VkDevice device;
VkSwapchainKHR swapchain;
VkImageView* imageViews;
VkShaderModule shaderModuleVert;
VkShaderModule shaderModuleFrag;
uint32_t amountOfImagesInSwapchain = 0;
GLFWwindow* window;

const uint32_t WIDTH = 400;
const uint32_t HEIGHT = 300;


void printStats(VkPhysicalDevice& device) {
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
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, nullptr); // when we pass nullptr 
    // instead of array with queue family properties the vkGetPhysicalDeviceQueueFamilyProperties
    // function will write into amountOfQueueFamilies a value with number of queue family properties available in the graphics card

    VkQueueFamilyProperties* familyProperties = new VkQueueFamilyProperties[amountOfQueueFamilies]; // new operator returns address of memory and we store it into so called pointer that is *someName
    vkGetPhysicalDeviceQueueFamilyProperties(device, &amountOfQueueFamilies, familyProperties); // here we put familyProperties without &, because it is a pointer - it is already an address of memory where object is located
    // when we call vkGetPhysicalDeviceQueueFamilyProperties the second time and we pass it
    // familyProperties array instead of nullptr and amountOfQueueFamilies the familyProperties[] is
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

    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);
    std::cout << "Surface capabilities: " << std::endl;
    std::cout << "\tminImageCount: " << surfaceCapabilities.minImageCount << std::endl;
    std::cout << "\tmaxImageCount: " << surfaceCapabilities.maxImageCount << std::endl;
    std::cout << "\tcurrentExtent: " << surfaceCapabilities.currentExtent.width << "/" << surfaceCapabilities.currentExtent.height << std::endl;
    std::cout << "\tminImageExtent: " << surfaceCapabilities.minImageExtent.width << "/" << surfaceCapabilities.minImageExtent.height << std::endl;
    std::cout << "\tmaxImageExtent: " << surfaceCapabilities.maxImageExtent.width << "/" << surfaceCapabilities.maxImageExtent.height << std::endl;
    std::cout << "\tmaxImageArrayLayers: " << surfaceCapabilities.maxImageArrayLayers << std::endl;
    std::cout << "\tsupportedTransforms: " << surfaceCapabilities.supportedTransforms << std::endl;
    std::cout << "\tcurrentTransform: " << surfaceCapabilities.currentTransform << std::endl;
    std::cout << "\tsupportedCompositeAlpha: " << surfaceCapabilities.supportedCompositeAlpha << std::endl;
    std::cout << "\tsupportedUsageFlags: " << surfaceCapabilities.supportedUsageFlags << std::endl;

    uint32_t amountOfFormats = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, nullptr);
    VkSurfaceFormatKHR* surfaceFormats = new VkSurfaceFormatKHR[amountOfFormats];
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &amountOfFormats, surfaceFormats);
    
    std::cout << std::endl;
    std::cout << "Amount of formats: " << amountOfFormats << std::endl;
    for (int i = 0; i < amountOfFormats; i++)
    {
        std::cout << surfaceFormats[i].format << std::endl;
    }

    uint32_t amountOFPresentationModes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOFPresentationModes, nullptr);
    VkPresentModeKHR* presentModes = new VkPresentModeKHR[amountOFPresentationModes];
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &amountOFPresentationModes, presentModes);

    std::cout << std::endl;
    std::cout << "Amount of Presentation Modes: " << amountOFPresentationModes << std::endl;
    for (uint32_t i = 0; i < amountOFPresentationModes; i++) 
    {
        std::cout << "Supported presentation mode: " << presentModes[i] << std::endl;
    }

    std::cout << std::endl;
    delete[] familyProperties;
    delete[] surfaceFormats;
    delete[] presentModes;
}

std::vector<char> readFile(const std::string &filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    
    //ifstream returns boolean and we can open the file if it is true
    if (file) {
        size_t fileSize = (size_t)file.tellg(); //tellg() gives us a position of the reader at the moment
        std::vector<char> fileBuffer(fileSize);
        file.seekg(0); // here we have set the reader at the beginning of the file
        file.read(fileBuffer.data(), fileSize);
        file.close();
        return fileBuffer;
    }
    else {
        throw std::runtime_error("Failed to open file!!!");
    }
}

void startGlfw() {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // here we say, that we do not want to use OpenGL
    glfwWindowHint(GLFW_RESIZABLE, FALSE);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan Tutorial", nullptr, nullptr);
}

void createShaderModule(const std::vector<char>& code, VkShaderModule* shaderModule) {
	VkShaderModuleCreateInfo shaderCreateInfo;
	shaderCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderCreateInfo.pNext = nullptr;
	shaderCreateInfo.flags = 0;
	shaderCreateInfo.codeSize = code.size();
	shaderCreateInfo.pCode = (uint32_t*)code.data();

    VkResult result = vkCreateShaderModule(device, &shaderCreateInfo, nullptr, shaderModule);
    ASSERT_VULKAN(result);
}

void startVulkan() {
    VkApplicationInfo appInfo;
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = "Vulkan Tutorial";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Super Vulkan Engine Turbo Mega";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_1;

    uint32_t amountOfLayers = 0;
    VkResult result = vkEnumerateInstanceLayerProperties(&amountOfLayers, nullptr);
    ASSERT_VULKAN(result);

    VkLayerProperties* layers = new VkLayerProperties[amountOfLayers];
    result = vkEnumerateInstanceLayerProperties(&amountOfLayers, layers);
    ASSERT_VULKAN(result);

    std::cout << "Amount of Instance Layers: " << amountOfLayers << std::endl;
    for (int i = 0; i < amountOfLayers; i++)
    {
        std::cout << std::endl;
        std::cout << "Layers Name: " << layers[i].layerName << std::endl;
        std::cout << "Specification Version: " << layers[i].specVersion << std::endl;
        std::cout << "Implementation Version: " << layers[i].implementationVersion << std::endl;
        std::cout << "Description: " << layers[i].description << std::endl;
    }
    std::cout << std::endl;

    uint32_t amountOfExtensions = 0; // Extensions are not nice to haves. Theay are essential part of Vulkan. Since it is crossplatform it must have some platform specific functionalities. E.g. some pieces of functionality related to window drawing. They are essential but differ for each platform.
    result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, nullptr);
    ASSERT_VULKAN(result);

    VkExtensionProperties* extensions = new VkExtensionProperties[amountOfExtensions];
    result = vkEnumerateInstanceExtensionProperties(nullptr, &amountOfExtensions, extensions);
    ASSERT_VULKAN(result);

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

    const std::vector<const char*> usedExtensions = {
        "VK_KHR_win32_surface",
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME
    };

    uint32_t amountOfGlfwExtensions = 0;
    auto glfwExtensions = glfwGetRequiredInstanceExtensions(&amountOfGlfwExtensions);

    VkInstanceCreateInfo instanceInfo;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.pApplicationInfo = &appInfo;
    instanceInfo.enabledLayerCount = validationLayers.size();
    instanceInfo.ppEnabledLayerNames = validationLayers.data();
    instanceInfo.enabledExtensionCount = amountOfGlfwExtensions;
    instanceInfo.ppEnabledExtensionNames = glfwExtensions;

    result = vkCreateInstance(&instanceInfo, nullptr, &instance);
    ASSERT_VULKAN(result);

    VkWin32SurfaceCreateInfoKHR surfaceCreateInfo;
    surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
    surfaceCreateInfo.pNext = nullptr;
    surfaceCreateInfo.flags = 0;
    surfaceCreateInfo.hinstance = nullptr;
    surfaceCreateInfo.hwnd = nullptr;

    result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
    ASSERT_VULKAN(result);

    result = vkCreateWin32SurfaceKHR(instance, &surfaceCreateInfo, nullptr, &surface);
    ASSERT_VULKAN(result);

    uint32_t amountOfPhysicalDevices = 0;
    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, nullptr); // when we pass nullptr 
    ASSERT_VULKAN(result);

    // instead of array with physical graphic cards the vkEnumeratePhysicalDevices 
    // function will write into amountOfPhysicalDevices a value with number of graphic cards installed into the computer

    // VkPhysicalDevice *physicalDevices = new VkPhysicalDevice[amountOfPhysicalDevices]; // define an array of size equal to amountOfPhysicalDevices
    std::vector<VkPhysicalDevice> physicalDevices;
    // vector does not have the size it is created with in the C++ standard, so we have to resize it to the value that we need
    physicalDevices.resize(amountOfPhysicalDevices);

    result = vkEnumeratePhysicalDevices(instance, &amountOfPhysicalDevices, physicalDevices.data()); // when we call vkEnumeratePhysicalDevices the second time and we pass it
    ASSERT_VULKAN(result);
    // physicalDevices array instead of nullptr and amountOfPhysicalDevices the physicalDevices[] is
    // populated with available physical graphic cards

    for (int i = 0; i < amountOfPhysicalDevices; i++)
    {
        printStats(physicalDevices[i]);
    }

    float queuePrios[] = { 1.0f, 1.0f, 1.0f, 1.0f };

    VkDeviceQueueCreateInfo deviceQueueCreateInfo;
    deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueCreateInfo.pNext = nullptr;
    deviceQueueCreateInfo.flags = 0;
    deviceQueueCreateInfo.queueFamilyIndex = 0; // Here, instead of hard coding 0 we actually need to find out what index has queue family that we are interested in. VkQueueFamilyProperties[amountOfQueueFamilies] contains all queue families.
    deviceQueueCreateInfo.queueCount = 1; // Here, instead of hardcoding 1 we need to find out the valid value.
    deviceQueueCreateInfo.pQueuePriorities = queuePrios;

    VkPhysicalDeviceFeatures usedFeatures = {};
    const std::vector<const char*> deviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    VkDeviceCreateInfo deviceCreateInfo;

    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.pNext = nullptr;
    deviceCreateInfo.flags = 0;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &deviceQueueCreateInfo;
    deviceCreateInfo.enabledLayerCount = 0;
    deviceCreateInfo.ppEnabledLayerNames = nullptr;
    deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &usedFeatures;

    //pick "best suitable physical device" here instead of just taking the first one
    result = vkCreateDevice(physicalDevices[0], &deviceCreateInfo, nullptr, &device); // we do not use our own allocator so we pass in nullptr
    ASSERT_VULKAN(result);

    VkQueue queue;
    vkGetDeviceQueue(device, 0, 0, &queue);


    VkBool32 surfaceSupport = false;
    result = vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevices[0], 0, surface, &surfaceSupport);
    ASSERT_VULKAN(result);

    if (!surfaceSupport) {
        std::cerr << "Surface not supported!" << std::endl;
        __debugbreak();
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.pNext = nullptr;
    swapchainCreateInfo.flags = 0;
    swapchainCreateInfo.surface = surface;
    swapchainCreateInfo.minImageCount = 3; // TODO: civ
    swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_UNORM; // TODO: civ
    swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR; // TODO: civ
    swapchainCreateInfo.imageExtent = VkExtent2D{ WIDTH, HEIGHT };
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE; // TODO: civ
    swapchainCreateInfo.queueFamilyIndexCount = 0;
    swapchainCreateInfo.pQueueFamilyIndices = nullptr;
    swapchainCreateInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = VK_PRESENT_MODE_FIFO_KHR; // TODO: civ
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    result = vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &swapchain);
    ASSERT_VULKAN(result);

    vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, nullptr);
    VkImage* swapchainImages = new VkImage[amountOfImagesInSwapchain];
    result = vkGetSwapchainImagesKHR(device, swapchain, &amountOfImagesInSwapchain, swapchainImages);
    ASSERT_VULKAN(result);
    
    imageViews = new VkImageView[amountOfImagesInSwapchain];
    for (int i = 0; i < amountOfImagesInSwapchain; i++)
    {
        VkImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        imageViewCreateInfo.pNext = nullptr;
        imageViewCreateInfo.flags = 0;
        imageViewCreateInfo.image = swapchainImages[i];
        imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_UNORM; // TODO: civ
        imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
        imageViewCreateInfo.subresourceRange.levelCount = 1;
        imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
        imageViewCreateInfo.subresourceRange.layerCount = 1;

        result = vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageViews[i]);
        ASSERT_VULKAN(result);
    }
       
    // We have to read the compiled vertex and fragment shaders so we could use them from this code
    auto shaderCodeVert = readFile("vert.spv");
    auto shaderCodeFrag = readFile("frag.spv");

    // the sizes of files must be the same as sizes reported by OS in file explorer. This way we additionally check out files for correctness.
    //std::cout << shaderCodeVert.size() << std::endl; 
    //std::cout << shaderCodeFrag.size() << std::endl; 

    createShaderModule(shaderCodeVert, &shaderModuleVert);
    createShaderModule(shaderCodeFrag, &shaderModuleFrag);

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoVert;
    shaderStageCreateInfoVert.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoVert.pNext = nullptr;
    shaderStageCreateInfoVert.flags = 0;
    shaderStageCreateInfoVert.stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageCreateInfoVert.module = shaderModuleVert;
    shaderStageCreateInfoVert.pName = "main"; // we show here, that enter point is a function "main" in the vertex shader
    shaderStageCreateInfoVert.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStageCreateInfoFrag;
    shaderStageCreateInfoFrag.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageCreateInfoFrag.pNext = nullptr;
    shaderStageCreateInfoFrag.flags = 0;
    shaderStageCreateInfoFrag.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageCreateInfoFrag.module = shaderModuleFrag;
    shaderStageCreateInfoFrag.pName = "main"; // we show here, that enter point is a function "main" in the fragment shader
    shaderStageCreateInfoFrag.pSpecializationInfo = nullptr;

    VkPipelineShaderStageCreateInfo shaderStages[] = { shaderStageCreateInfoVert, shaderStageCreateInfoFrag };

    VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo;
    vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputCreateInfo.pNext = nullptr;
    vertexInputCreateInfo.flags = 0;
    vertexInputCreateInfo.vertexBindingDescriptionCount = 0;
    vertexInputCreateInfo.pVertexBindingDescriptions = nullptr;
    vertexInputCreateInfo.vertexAttributeDescriptionCount = 0;
    vertexInputCreateInfo.pVertexAttributeDescriptions = nullptr;


    delete[] swapchainImages;
    delete[] layers;
    delete[] extensions;
    // we do not delete physicalDevices vector because it is stored in the stack memory so when the execution goes beyond the curly braces the memory is freed up automatically
    // delete[] physicalDevices; 
}

void gameLoop() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
    }
}

void shutdownVulkan() {
    vkDeviceWaitIdle(device); //when this function returns we can be sure, that all work of this device is ended. Everything is gone from this device. We need ofcourse make sure we do not give new tasks to this device again

    // freeing up resources should happen in reverse sequence from the sequence of creating. E.g.: First we created instance, second - device, so we have to delete first device and then instance
    for (int i = 0; i < amountOfImagesInSwapchain; i++)
    {
        vkDestroyImageView(device, imageViews[i], nullptr);
    }

    delete[] imageViews;
    vkDestroyShaderModule(device, shaderModuleVert, nullptr);
    vkDestroyShaderModule(device, shaderModuleFrag, nullptr);
    vkDestroySwapchainKHR(device, swapchain, nullptr);
    vkDestroyDevice(device, nullptr); // we didn't use our own allocator so we place here nullptr
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr); // when we destroy instance all the physical devices are getting destroyed as well
}

void shutdownGlfw() {
    glfwDestroyWindow(window);
}

int main()
{
    startGlfw();
    startVulkan();
    gameLoop();
    shutdownVulkan();
    shutdownGlfw();

    return 0;
}
