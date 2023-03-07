#pragma once
#include "VulkanCore.h"



namespace Spaceng
{
	struct Buffer
	{
		VkBufferUsageFlags usageflags;
		VkMemoryPropertyFlags MemoryPropertyflags;
		VkDeviceSize size;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo BufferDescriptor;
		void* mapped = nullptr;
	};
	class VulkanBufferMemory
	{

	public:
		 static VkResult ConstructBuffer(Buffer& Buffer, VkDeviceSize size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags MemoryPropertyflags, VkDevice Device,
			VkPhysicalDevice* PhysicalDevice, bool descriptorAccess, bool mapAccess, void* data = nullptr);

		 static void DeallocateBufferMemory(VkDevice* Device, Buffer* buffer);
	
		 static uint32_t getMemoryType(VkPhysicalDeviceMemoryProperties DeviceMemProperties, VkMemoryPropertyFlags memoryPropertyFlags,
			VkMemoryRequirements memReq, VkBool32* memTypeFound = nullptr);
	};

}




