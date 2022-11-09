#pragma once
#include "VulkanCore.h"



namespace Spaceng
{
	class VulkanMemory
	{
		VkResult AllocateBufferMemory(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size,
			VkBuffer* buffer, VkDeviceMemory* memory, void* data, VkDevice Device, VkPhysicalDeviceMemoryProperties DeviceMemProperties);
	
		uint32_t getMemoryType(VkPhysicalDeviceMemoryProperties DeviceMemProperties, VkMemoryPropertyFlags memoryPropertyFlags,
			VkMemoryRequirements memReq, VkBool32* memTypeFound);
	};

}




