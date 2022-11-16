#include "PCH.h"
#include"AssetManagerGLTF.h"

namespace Spaceng
{

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


	void VkGLTFAsset::LoadFromFile(std::string filename)
	{

	}





}