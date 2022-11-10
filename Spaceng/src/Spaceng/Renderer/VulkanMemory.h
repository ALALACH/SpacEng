#pragma once
#include "VulkanCore.h"



namespace Spaceng
{
	class VulkanBufferMemory
	{
	public:
		static VkResult AllocateBufferMemory(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size,
			VkBuffer* buffer, VkDeviceMemory* memory, VkDescriptorBufferInfo BufferDescriptor, void* mapped, VkDevice Device, VkPhysicalDeviceMemoryProperties DeviceMemProperties, void* data = nullptr
			,bool DescriptorAccess=false, bool mapAccess = false);

		static void DeallocateBufferMemory(VkDevice Device, VkBuffer* buffer, VkDeviceMemory* memory);
	
		static uint32_t getMemoryType(VkPhysicalDeviceMemoryProperties DeviceMemProperties, VkMemoryPropertyFlags memoryPropertyFlags,
			VkMemoryRequirements memReq, VkBool32* memTypeFound);
	};

}




