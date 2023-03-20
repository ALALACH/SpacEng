#pragma once
#include "VulkanCore.h"
#include "VulkanMemory.h"

#include "glm/glm/glm.hpp"
#include <glm/glm/gtc/matrix_transform.hpp>
#include <glm/glm/gtc/type_ptr.hpp>

#define Vertex_Binding_Index_0 0
//stb_image included in Tinygltf
#define STB_IMAGE_IMPLEMENTATION
#include "tinygltf/tiny_gltf.h"



namespace Spaceng
{

	enum AssetType
	{
		Video,
		model_type,
		Simple_Mesh_type
	};


	class Texture
	{
		friend class VulkanRenderer;
	public:
		Texture();
		~Texture();
		Texture& operator=(const Texture& Other);

		void LoadfromglTfImage(tinygltf::Image& gltfimage, std::string path, VkDevice* device, VkPhysicalDevice* PhysicalDevice, VkQueue copyQueue);
		void loadFromFile(std::string filename, VkFormat format, VkDevice* Device, VkPhysicalDevice* PhysicalDevice, VkCommandPool pool, VkQueue copyQueue,
			VkImageUsageFlags imageUsageFlags = VK_IMAGE_USAGE_SAMPLED_BIT,
			VkImageLayout ImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL , bool linear = false , bool EnabledMip = false);
		void LoadFrom_RGBA_Buffer();

		void Destroy(VkDevice* Device);

	private:
		VkImage image = VK_NULL_HANDLE;
		VkDeviceMemory imagedeviceMemory = VK_NULL_HANDLE;
		VkImageView view = VK_NULL_HANDLE;
		uint32_t width = 0, height = 0;
		uint32_t mipLevels = 0;
		VkSampler sampler = VK_NULL_HANDLE;
		VkDescriptorImageInfo TextureDescriptor;
		std::string DebugName;
	};



	class Model
	{
		friend class VulkanRenderer;
	public:
		Model();
		~Model();
		void LoadFromFile(VkDevice* Device, VkPhysicalDevice* PhysicalDevice, std::string filename);
		void Draw(VkCommandBuffer cmd);
		void generateQuad(VkDevice* Device, VkPhysicalDevice* PhysicalDevice); //for custom purposes

		
	private:
		uint32_t Index_;
		Buffer VertexBuffer;
		Buffer IndexBuffer;
		
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
		Texture AssetTexture;
		

		VkDescriptorSetLayout DescriptorSetLayout = VK_NULL_HANDLE;
		VkDescriptorPool DescriptorPool = VK_NULL_HANDLE;
		VkDescriptorSet DescriptorSet = VK_NULL_HANDLE;
		std::vector<VkWriteDescriptorSet> writeDescriptorSets;

		VkPipelineLayout PipelineLayout = VK_NULL_HANDLE;
		VkPipeline Pipeline = VK_NULL_HANDLE;

		bool DepthStencilEnabled = false;

		std::string Filepath;
		std::string VertexShaderFile ;
		std::string FragmentShaderFile ;

		struct UBMatrix {
			glm::mat4 projection;
			glm::mat4 view;
			glm::mat4 model;
			glm::vec3 camPos;
		}UBOMatrices;
	};
}
