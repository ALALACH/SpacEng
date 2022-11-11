#pragma once
#include "VulkanCore.h"



namespace Spaceng
{
	struct Buffer
	{
		VkBufferUsageFlags usageflags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		VkMemoryPropertyFlags MemoryPropertyflags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
		VkDeviceSize size;
		VkBuffer buffer = VK_NULL_HANDLE;
		VkDeviceMemory memory = VK_NULL_HANDLE;
		VkDescriptorBufferInfo BufferDescriptor;
		void* mapped = nullptr;
	};

	class VulkanBufferMemory
	{
	public:
		static VkResult AllocateBufferMemory(Buffer& Buffer, VkDevice Device, VkPhysicalDeviceMemoryProperties DeviceMemProperties, void* data = nullptr
			,bool descriptorAccess, bool mapAccess);

		static void DeallocateBufferMemory(VkDevice Device, VkBuffer* buffer, VkDeviceMemory* memory);
	
		static uint32_t getMemoryType(VkPhysicalDeviceMemoryProperties DeviceMemProperties, VkMemoryPropertyFlags memoryPropertyFlags,
			VkMemoryRequirements memReq, VkBool32* memTypeFound);
	};

}




