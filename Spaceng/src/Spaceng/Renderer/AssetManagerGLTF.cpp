#include "PCH.h"
#include"AssetManagerGLTF.h"

namespace Spaceng
{
	Model::Model()
	{

	}
	Model::~Model()
	{

	}
	Texture::Texture()
	{

	}
	Texture::~Texture()
	{

	}

	VkGLTFAsset::VkGLTFAsset(std::string name ,AssetType type, bool DepthStencil, std::string filepath)
	{

		UniformBuffer.size = sizeof(UBOMatrices);
		Name = name;
		Type = type;
		VertexShaderFile =filepath + "\\assets\\Shaders\\" + name + ".vert.spv"; 
		FragmentShaderFile =filepath + "\\assets\\Shaders\\" + name + ".frag.spv";
		if (DepthStencil) { DepthStencilEnabled = true; }
	}


	VkGLTFAsset::~VkGLTFAsset() 
	{

	}


	void Model::LoadFromFile(VkDevice* Device, VkPhysicalDevice* PhysicalDevice ,std::string filename)
	{
		tinygltf::Model model;
	}
	 
	void Model::DrawVerticesAndIndices()
	{

	}


	//Texture
	/*
	void Texture::loadFromFile(std::string filename, VkFormat format, VkDevice* Device,VkPhysicalDevice* PhysicalDevice, VkCommandPool pool, VkQueue copyQueue,
		VkImageUsageFlags imageUsageFlags, VkImageLayout ImageLayout, bool linear)
	{
		ktxTexture* KtxTexture;
		std::ifstream f(filename.c_str());
		SE_ASSERT(!f.fail(), "Could not Open File");
		ktxResult result = ktxTexture_CreateFromNamedFile(filename.c_str(), KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT, &KtxTexture);
		SE_ASSERT(result == KTX_SUCCESS, "Could not load file");

		width = KtxTexture->baseWidth;
		height = KtxTexture->baseHeight;
		mipLevels = KtxTexture->numLevels;

		ktx_uint8_t* ktxTextureData = ktxTexture_GetData(KtxTexture);
		ktx_size_t ktxTextureSize = KtxTexture->dataSize;

		//format
		VkFormatProperties formatProperties;
		vkGetPhysicalDeviceFormatProperties(*PhysicalDevice, format, &formatProperties);


		//Optimal Image Creation
		//Default
		//Linear Implementation Added but obscured
		//CopyCommand
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandPool = pool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		commandBufferAllocateInfo.commandBufferCount = 1;
		VkCommandBuffer CopyCmd;
		VK_CHECK_RESULT(vkAllocateCommandBuffers(*Device, &commandBufferAllocateInfo, &CopyCmd));
		VkCommandBufferBeginInfo cmdBufferBeginInfo{};
		cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		VK_CHECK_RESULT(vkBeginCommandBuffer(CopyCmd, &cmdBufferBeginInfo));

		if (!linear)
		{
			//memory
			VkMemoryRequirements memReqs;
			VkDeviceMemory memory;
			VkBuffer stagingbuffer;

			VkBufferCreateInfo BufferCI{};
			BufferCI.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			BufferCI.size = ktxTextureSize;
			BufferCI.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			BufferCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			VK_CHECK_RESULT(vkCreateBuffer(*Device, &BufferCI, nullptr, &stagingbuffer));

			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vkGetBufferMemoryRequirements(*Device, stagingbuffer, &memReqs);
			memAllocInfo.allocationSize = memReqs.size;
			VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
			vkGetPhysicalDeviceMemoryProperties(*PhysicalDevice, &deviceMemoryProperties);
			memAllocInfo.memoryTypeIndex = VulkanBufferMemory::getMemoryType(deviceMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memReqs);
			VK_CHECK_RESULT(vkAllocateMemory(*Device, &memAllocInfo, nullptr, &memory));
			VK_CHECK_RESULT(vkBindBufferMemory(*Device, stagingbuffer, memory, 0));

			//Host-Access
			void* data;
			VK_CHECK_RESULT(vkMapMemory(*Device, memory, 0, memReqs.size, 0, &data));
			memcpy(data, ktxTextureData, memReqs.size);
			vkUnmapMemory(*Device, memory);


			// Setup buffer copy regions for each mip level
			std::vector<VkBufferImageCopy> bufferCopyRegions;

			for (uint32_t i = 0; i < mipLevels; i++)
			{
				ktx_size_t offset;
				KTX_error_code result = ktxTexture_GetImageOffset(KtxTexture, i, 0, 0, &offset);
				SE_ASSERT(result == KTX_SUCCESS , "Couldn't get image offset");

				VkBufferImageCopy bufferCopyRegion = {};
				bufferCopyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				bufferCopyRegion.imageSubresource.mipLevel = i;
				bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent.width = std::max(1u, KtxTexture->baseWidth >> i);
				bufferCopyRegion.imageExtent.height = std::max(1u, KtxTexture->baseHeight >> i);
				bufferCopyRegion.imageExtent.depth = 1;
				bufferCopyRegion.bufferOffset = offset;

				bufferCopyRegions.push_back(bufferCopyRegion);
			}

			//Image
			VkImageCreateInfo ImageCI{};
			ImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			ImageCI.imageType = VK_IMAGE_TYPE_2D;
			ImageCI.format = format;
			ImageCI.extent = { width, height, 1 };
			ImageCI.mipLevels = mipLevels;
			ImageCI.arrayLayers = 1;
			ImageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			ImageCI.tiling = VK_IMAGE_TILING_LINEAR;
			ImageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			ImageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			ImageCI.usage = imageUsageFlags;
			if (!(ImageCI.usage & VK_IMAGE_USAGE_TRANSFER_DST_BIT))
			{
				ImageCI.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
			}
			VK_CHECK_RESULT(vkCreateImage(*Device, &ImageCI, nullptr, &image));

			vkGetImageMemoryRequirements(*Device,image, &memReqs);

			memAllocInfo.allocationSize = memReqs.size;

			memAllocInfo.memoryTypeIndex = VulkanBufferMemory::getMemoryType(deviceMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, memReqs);
			VK_CHECK_RESULT(vkAllocateMemory(*Device, &memAllocInfo, nullptr, &memory));
			VK_CHECK_RESULT(vkBindImageMemory(*Device, image, memory, 0));

			{
				//Image Memory Layout Optimal for copying
				VkImageSubresourceRange subresourceRange = {};
				subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				subresourceRange.baseMipLevel = 0;
				subresourceRange.levelCount = 1;
				subresourceRange.layerCount = 1;
				VkImageMemoryBarrier imageMemoryBarrier{};
				imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
				imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
				imageMemoryBarrier.srcAccessMask = 0;
				imageMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
				imageMemoryBarrier.image = image;
				imageMemoryBarrier.subresourceRange = subresourceRange;

				vkCmdPipelineBarrier(
					CopyCmd,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
					0,
					0, nullptr,
					0, nullptr,
					1, &imageMemoryBarrier);
			}

			//Copying from staging buffer to Image then setting Layout again

			vkCmdCopyBufferToImage(CopyCmd, stagingbuffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 
				static_cast<uint32_t>(bufferCopyRegions.size()), bufferCopyRegions.data());

			{
			//Image Memory Layout :Shader Read-only Layout
			imageLayout = ImageLayout;
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			imageMemoryBarrier.newLayout = ImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;

			vkCmdPipelineBarrier(
				CopyCmd,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);
			}
			//Submitting CopyCmd

			VK_CHECK_RESULT(vkEndCommandBuffer(CopyCmd));

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &CopyCmd;
			VkFenceCreateInfo FenceCI{};
			FenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			FenceCI.flags = 0;
			VkFence fence;
			VK_CHECK_RESULT(vkCreateFence(*Device, &FenceCI, nullptr, &fence));
			VK_CHECK_RESULT(vkQueueSubmit(copyQueue, 1, &submitInfo, fence));
			VK_CHECK_RESULT(vkWaitForFences(*Device, 1, &fence, VK_TRUE, UINT64_MAX));

			vkDestroyFence(*Device, fence, nullptr);
			vkFreeCommandBuffers(*Device, pool, 1, &CopyCmd);

			//Local stagingbuffer Cleanup
			vkFreeMemory(*Device, memory, nullptr);
			vkDestroyBuffer(*Device, stagingbuffer, nullptr);

		}

		else
		{ 
			SE_ASSERT(formatProperties.linearTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT , "Format Feature missing");
			//Image
			VkImageCreateInfo ImageCI{};
			ImageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			ImageCI.imageType = VK_IMAGE_TYPE_2D;
			ImageCI.format = format;
			ImageCI.extent = { width, height, 1 };
			ImageCI.mipLevels = 1;
			ImageCI.arrayLayers = 1;
			ImageCI.samples = VK_SAMPLE_COUNT_1_BIT;
			ImageCI.tiling = VK_IMAGE_TILING_LINEAR;
			ImageCI.usage = imageUsageFlags;
			ImageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			ImageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

			VkImage MappableImage;
			VK_CHECK_RESULT(vkCreateImage(*Device, &ImageCI, nullptr, &MappableImage));
			
			//memory
			VkMemoryRequirements memReqs;
			VkDeviceMemory memory;

			VkMemoryAllocateInfo memAllocInfo{};
			memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			vkGetImageMemoryRequirements(*Device, MappableImage, &memReqs);
			memAllocInfo.allocationSize = memReqs.size;
			VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
			vkGetPhysicalDeviceMemoryProperties(*PhysicalDevice, &deviceMemoryProperties);
			memAllocInfo.memoryTypeIndex = VulkanBufferMemory::getMemoryType(deviceMemoryProperties, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,memReqs);
			VK_CHECK_RESULT(vkAllocateMemory(*Device, &memAllocInfo, nullptr, &memory));
			VK_CHECK_RESULT(vkBindImageMemory(*Device, MappableImage, memory, 0));

			//sub-layout
			VkImageSubresource SubResource = {};
			SubResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			SubResource.mipLevel = 0;
			VkSubresourceLayout SubresourceLayout;
			vkGetImageSubresourceLayout(*Device, MappableImage, &SubResource, &SubresourceLayout);

			//Host-Access
			void* data;
			VK_CHECK_RESULT(vkMapMemory(*Device, memory, 0, memReqs.size, 0, &data));
			memcpy(data, ktxTextureData, memReqs.size);
			vkUnmapMemory(*Device, memory);

			image = MappableImage;
			imageLayout = ImageLayout;
			imagedeviceMemory = memory;

			//Image_Memory_Layout
			VkImageSubresourceRange subresourceRange = {};
			subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.layerCount = 1;
			VkImageMemoryBarrier imageMemoryBarrier{};
			imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			imageMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageMemoryBarrier.srcAccessMask = 0;
			imageMemoryBarrier.newLayout = ImageLayout;
			imageMemoryBarrier.image = image;
			imageMemoryBarrier.subresourceRange = subresourceRange;
			switch (ImageLayout)
			{
			case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
				break;

			case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
				imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
				break;

			case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
				if (imageMemoryBarrier.srcAccessMask == 0)
				{
					imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
				}
				imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				break;
			default:
				break;
			}


			// Put barrier inside setup command buffer
			vkCmdPipelineBarrier(
				CopyCmd,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
				0,
				0, nullptr,
				0, nullptr,
				1, &imageMemoryBarrier);

			//Submitting CopyCmd

			VK_CHECK_RESULT(vkEndCommandBuffer(CopyCmd));

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &CopyCmd;
			VkFenceCreateInfo FenceCI{};
			FenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			FenceCI.flags = 0;
			VkFence fence;
			VK_CHECK_RESULT(vkCreateFence(*Device, &FenceCI, nullptr, &fence));
			VK_CHECK_RESULT(vkQueueSubmit(copyQueue, 1, &submitInfo, fence));
			VK_CHECK_RESULT(vkWaitForFences(*Device, 1, &fence, VK_TRUE, UINT64_MAX));

			vkDestroyFence(*Device, fence, nullptr);
			vkFreeCommandBuffers(*Device, pool, 1, &CopyCmd);
		}
		//cleanup KTX
		ktxTexture_Destroy(KtxTexture);

		//Sampler
		VkSamplerCreateInfo samplerCreateInfo = {};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.magFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.minFilter = VK_FILTER_LINEAR;
		samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerCreateInfo.mipLodBias = 0.0f;
		samplerCreateInfo.compareOp = VK_COMPARE_OP_NEVER;
		samplerCreateInfo.minLod = 0.0f;
		samplerCreateInfo.maxLod =0.0f;
		VkPhysicalDeviceFeatures DeviceFeatures;
		vkGetPhysicalDeviceFeatures(*PhysicalDevice, &DeviceFeatures);
		VkPhysicalDeviceProperties DeviceProperties;
		vkGetPhysicalDeviceProperties(*PhysicalDevice, &DeviceProperties);
		samplerCreateInfo.maxAnisotropy = DeviceFeatures.samplerAnisotropy ? DeviceProperties.limits.maxSamplerAnisotropy : 1.0f;
		samplerCreateInfo.anisotropyEnable = DeviceFeatures.samplerAnisotropy;
		samplerCreateInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		VK_CHECK_RESULT(vkCreateSampler(*Device, &samplerCreateInfo, nullptr, &sampler));

		//ImageView
		VkImageViewCreateInfo ImageviewCI = {};
		ImageviewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		ImageviewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
		ImageviewCI.format = format;
		ImageviewCI.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
		ImageviewCI.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
		ImageviewCI.subresourceRange.levelCount = 1;
		ImageviewCI.image = image;
		VK_CHECK_RESULT(vkCreateImageView(*Device, &ImageviewCI, nullptr, &view));

		//texture Descriptor
		TextureDescriptor.sampler = sampler;
		TextureDescriptor.imageView = view;
		TextureDescriptor.imageLayout = imageLayout;
	}
	*/
	void Texture::LoadfromglTfImage(tinygltf::Image& gltfimage, std::string path, VkDevice* device, VkPhysicalDevice* PhysicalDevice, VkQueue copyQueue)
	{

	}

	void Texture::Destroy(VkDevice* Device)
	{
		vkDestroyImageView(*Device, view, nullptr);
		vkDestroyImage(*Device, image, nullptr);
		if (sampler)
		{
			vkDestroySampler(*Device, sampler, nullptr);
		}
		vkFreeMemory(*Device, imagedeviceMemory, nullptr);
	}

}