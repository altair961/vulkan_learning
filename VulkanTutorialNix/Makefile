VULKAN_SDK_PATH = /home/altair/vulkansdk-linux-x86_64-1.1.130.0/1.1.130.0/x86_64
CFLAGS = -std=c++17 -I$(VULKAN_SDK_PATH)/include -g
LDFLAGS = -L$(VULKAN_SDK_PATH)/lib `pkg-config --static --libs glfw3` -lvulkan

VulkanTest: VulkanTutorial.cpp
	g++ $(CFLAGS) -o VulkanTutorial.o VulkanTutorial.cpp $(LDFLAGS)

.PHONY: test clean

test: VulkanTutorial
	LD_LIBRARY_PATH=$(VULKAN_SDK_PATH)/lib VK_LAYER_PATH=$(VULKAN_SDK_PATH)/etc/vulkan/explicit_layer.d ./VulkanTutorial

clean:
	rm -f VulkanTutorial
