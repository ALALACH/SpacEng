#include "PCH.h"
#include"AssetManagerGLTF.h"

namespace Spaceng
{

	VkGLTFAsset::VkGLTFAsset(AssetType type)
	{
		UniformBuffer.size = sizeof(UBOMatrices);
	}


	VkGLTFAsset::~VkGLTFAsset() 
	{

	}


	void VkGLTFAsset::LoadFromFile(std::string filename)
	{

	}





}