#pragma once
#include "Spaceng.h"


namespace Spaceng
{ 
	class EditorLayer : public Layer
	{
	public:

		EditorLayer(const std::string Name);
		virtual ~EditorLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnUpdate(float Timestep) override;
		virtual void OnEvent(Event& Event) override;
		bool OnKeyPressed(KeyPressedEvent& e);
	private :
		std::string m_DebugName;


		//first object
		struct
		{
			VkBufferUsageFlags usageflags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
			VkBuffer buffer = VK_NULL_HANDLE;
			VkDeviceMemory memory = VK_NULL_HANDLE;
			void* mapped = nullptr;
			VkDescriptorBufferInfo BufferDescriptor;
			struct UBOMatrices {
				glm::mat4 projection;
				glm::mat4 model;
				glm::mat4 view;
				glm::vec3 camPos;
			} uboMatrices;

		}Skybox;
	};



}
