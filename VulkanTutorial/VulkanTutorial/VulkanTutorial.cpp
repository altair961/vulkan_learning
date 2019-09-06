// VulkanTutorial.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "vulkan\vulkan.h"

int main()
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pNext = NULL;
	appInfo.pApplicationName = "Vulkan Tutorial";
	appInfo.apiVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.pEngineName = "Vulkan Tutorial Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 0);
	appInfo.apiVersion = VK_VERSION_1_1;

	return 0;
}