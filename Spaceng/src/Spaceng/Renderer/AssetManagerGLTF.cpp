#include "PCH.h"
#include"AssetManagerGLTF.h"

namespace Spaceng
{

	VkGLTFAsset::VkGLTFAsset(std::string name ,AssetType type)
	{

		UniformBuffer.size = sizeof(UBOMatrices);
		Name = name;
		Type = type;
	}


	VkGLTFAsset::~VkGLTFAsset() 
	{

	}


	void VkGLTFAsset::LoadFromFile(std::string filename)
	{

	}





}