#pragma once
#include "VulkanCore.h"
#include "VulkanMemory.h"

#include "glm/glm/glm.hpp"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>


//stb_image included in Tinygltf
#define STB_IMAGE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"



namespace Spaceng
{

	enum AssetType
	{
		texture_type,
		model_type,
		Simple_Mesh_type
	};


	class Texture
	{
		friend class VulkanRenderer;
	public:
		Texture();
		~Texture();

		void LoadfromglTfImage(tinygltf::Image& gltfimage, std::string path, VkDevice* device, VkPhysicalDevice* PhysicalDevice, VkQueue copyQueue);

		void loadFromFile(std::string filename, VkFormat format, VkDevice* Device, VkPhysicalDevice* PhysicalDevice, VkCommandPool pool, VkQueue copyQueue,
			VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL , bool linear = false);

		void Destroy(VkDevice* Device);

	private:
		VkImage image = VK_NULL_HANDLE;
		VkImageLayout imageLayout;
		VkDeviceMemory imagedeviceMemory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		uint32_t width = 0, height = 0;
		uint32_t mipLevels = 0;
		uint32_t layerCount = 0;
		VkSampler sampler = VK_NULL_HANDLE;
		VkDescriptorImageInfo TextureDescriptor;
	};



	class Model
	{
		friend class VulkanRenderer;
	public:
		Model();
		~Model();
		void LoadFromFile(VkDevice* Device, VkPhysicalDevice* PhysicalDevice, std::string filename);
		void DrawVerticesAndIndices();

	private:
		Texture texture;

	};


	class VkGLTFAsset
	{
		friend class VulkanRenderer;
	public:
		VkGLTFAsset(std::string name , AssetType type, bool DepthStencil, std::string filepath);
		~VkGLTFAsset();

		inline const std::string getName() { return Name; };
		inline const uint32_t getType() { return Type; };

	private:
		uint32_t Type;
		std::string Name;

		Buffer UniformBuffer;
		Model AssetModel;
		

		VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;

		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		VkPipeline Pipeline = VK_NULL_HANDLE;

		bool DepthStencilEnabled = false;

		std::string VertexShaderFile ;
		std::string FragmentShaderFile ;
		std::string AssetFile;

		struct UBMatrix {
			glm::mat4 projection;
			glm::mat4 model;
			glm::mat4 view;
			glm::vec3 camPos;
		}UBOMatrices;
	};
}
