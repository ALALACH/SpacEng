#include"PCH.h"
#include"VulkanMemory.h"




namespace Spaceng
{

	VkResult VulkanBufferMemory::ConstructBuffer( Buffer& Buffer,VkDeviceSize size, VkBufferUsageFlags usageflags, VkMemoryPropertyFlags MemoryPropertyflags, VkDevice Device,
		VkPhysicalDevice* PhysicalDevice, bool descriptorAccess, bool mapAccess , void* data)
	{
		Buffer.usageflags = usageflags;
		Buffer.MemoryPropertyflags = MemoryPropertyflags;
		Buffer.mapaccess = mapAccess;
		Buffer.size = size;

		VkBufferCreateInfo BufferCI{};
		BufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		BufferCI.usage = Buffer.usageflags;
		BufferCI.size = Buffer.size;
		BufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		VK_CHECK_RESULT(vkCreateBuffer(Device, &BufferCI, nullptr, &Buffer.buffer));

		VkMemoryRequirements memReq;
		vkGetBufferMemoryRequirements(Device, Buffer.buffer, &memReq);

		VkBool32* memTypeFound = nullptr;
		VkMemoryAllocateInfo memAlloc{};
		memAlloc.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memAlloc.allocationSize = memReq.size;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		vkGetPhysicalDeviceMemoryProperties(*PhysicalDevice, &deviceMemoryProperties);
		memAlloc.memoryTypeIndex = getMemoryType(deviceMemoryProperties, Buffer.MemoryPropertyflags, memReq, memTypeFound);

		VkMemoryAllocateFlagsInfoKHR allocFlagsInfo{};
		if (Buffer.usageflags & VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT) 
		{
			allocFlagsInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO_KHR;
			allocFlagsInfo.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT_KHR;
			memAlloc.pNext = &allocFlagsInfo;
		}

		VK_CHECK_RESULT(vkAllocateMemory(Device, &memAlloc, nullptr, &Buffer.memory));

		if (data != nullptr) //MeshBuffer Mapping
		{
			//Host Access to Device Memory Objects
			VK_CHECK_RESULT(vkMapMemory(Device, Buffer.memory, 0, Buffer.size, 0, &Buffer.mapped));
			memcpy(&Buffer.mapped, data, Buffer.size);

			//guarantee that writes to the memory object from the host are made available to the host domain
			if ((Buffer.MemoryPropertyflags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0)
			{
				VkMappedMemoryRange mappedRange{};
				mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
				mappedRange.memory = Buffer.memory;
				mappedRange.offset = 0;
				mappedRange.size = Buffer.size;
				VK_CHECK_RESULT(vkFlushMappedMemoryRanges(Device, 1, &mappedRange));
			}
			vkUnmapMemory(Device, Buffer.memory);
			Buffer.mapped = nullptr;
		}
		if (!data && mapAccess) //UniformBuffers Updates Specefic 
		{
			VK_CHECK_RESULT(vkMapMemory(Device, Buffer.memory, 0, Buffer.size, 0, &Buffer.mapped));
		}
		if (descriptorAccess)
		{
			Buffer.BufferDescriptor.buffer = Buffer.buffer;
			Buffer.BufferDescriptor.offset = 0;
			Buffer.BufferDescriptor.range = VK_WHOLE_SIZE;
		}
		// Attach the memory to the buffer object
		VK_CHECK_RESULT(vkBindBufferMemory(Device, Buffer.buffer, Buffer.memory, 0));

		return VK_SUCCESS;
	}


	void VulkanBufferMemory::DeallocateBufferMemory(VkDevice* Device, Buffer* buffer)
	{
		if (buffer->mapaccess)
			vkUnmapMemory(*Device, buffer->memory);
		vkFreeMemory(*Device, buffer->memory, nullptr);
		//VK_SPEC : If a memory object is mapped at the time it is freed, it is implicitly unmapped.
	}


	uint32_t VulkanBufferMemory::getMemoryType(VkPhysicalDeviceMemoryProperties DeviceMemoryProperties, VkMemoryPropertyFlags memoryPropertyFlags,
		VkMemoryRequirements memReq, VkBool32* memTypeFound)
	{
		for (uint32_t i = 0; i < DeviceMemoryProperties.memoryTypeCount; i++)
		{
			if ((memReq.memoryTypeBits & 1) == 1)
			{
				if ((DeviceMemoryProperties.memoryTypes[i].propertyFlags & memoryPropertyFlags) == memoryPropertyFlags)
				{
					if (memTypeFound){
						*memTypeFound = true;
					}
					return i;
				}
			}
			memReq.memoryTypeBits >>= 1;
		}

		if (memTypeFound)
		{
			*memTypeFound = false;
			return 0;
		}
		else
		{
			SE_LOG_ERROR("Could not find a matching memory type");
			__debugbreak();
			return 0;
		}
	}

}