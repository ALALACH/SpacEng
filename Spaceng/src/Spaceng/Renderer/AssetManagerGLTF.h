#pragma once
#include "VulkanCore.h"
#include "VulkanMemory.h"

#include "glm/glm/glm.hpp"


namespace Spaceng
{

	enum AssetType
	{
		TextureType,
		MeshType
	};

	class VkGLTFAsset
	{
		friend class VulkanRenderer;
	public:
		VkGLTFAsset(AssetType type);
		~VkGLTFAsset();

		void LoadFromFile(std::string filename);

	private:
		Buffer UniformBuffer;
		struct UBMatrix {
			glm::mat4 projection;
			glm::mat4 model;
			glm::mat4 view;
			glm::vec3 camPos;
		}UBOMatrices;


		
	};
}
