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
		static VkResult AllocateBufferMemory(Buffer& Buffer, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags MemoryPropertyflags, VkDevice Device,
			VkPhysicalDeviceMemoryProperties DeviceMemProperties, bool descriptorAccess, bool mapAccess, void* data = nullptr);

		static void DeallocateBufferMemory(VkDevice* Device, VkBuffer* buffer, VkDeviceMemory* memory);
	
		static uint32_t getMemoryType(VkPhysicalDeviceMemoryProperties DeviceMemProperties, VkMemoryPropertyFlags memoryPropertyFlags,
			VkMemoryRequirements memReq, VkBool32* memTypeFound = nullptr);
	};

}




