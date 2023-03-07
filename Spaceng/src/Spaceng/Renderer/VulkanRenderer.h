#pragma once
#include <string>
#include "VulkanCore.h"
#include <glfw/include/GLFW/glfw3.h>
#include "VulkanMemory.h"
#include "AssetManagerGLTF.h"


namespace Spaceng
{
#ifdef SE_DEBUG
	static bool Validation = true;
#else
	static bool Validation = false;
#endif

	
	class VulkanRenderer
	{
	public:
		~VulkanRenderer();
		void InitExtensions(std::vector<const char*> EnabledInstanceextensions, std::vector<const char*> EnabledDeviceextensions,
			VkPhysicalDeviceFeatures EnableddeviceFeatures);
		void InitRenderer();
		void CreateInstance();
		void CreateDevice();
		void SetupDepthStencil();
		void CreatePipelineCache();
		void CreateSemaphores();
		void SetupFunctionPtr();
		void getSupportedDepth();
		void SubmitInformation();
		void SetupRenderPass();



		void CreateSurfacePrimitives(GLFWwindow* Window);
		void CreateDisplayTemplate(uint32_t* width, uint32_t* height, bool vsync);

		void setView();
		void prepareUniformBuffer(VkGLTFAsset* Asset, bool mapAccess, bool descriptorAcess = true);
		void updateUniformBuffer(VkGLTFAsset* Asset);
		void prepareDescriptors(VkGLTFAsset* Asset);
		void UpdateDescriptorSet(VkGLTFAsset* Asset , bool updateUniform = true , bool updateTexture = true);
		void preparePipeline(VkGLTFAsset* Asset);



		void PrepareAsset(VkGLTFAsset* Asset ,AssetType Type , std::string filename);
		void CleanUpAsset(VkGLTFAsset* Asset);


		void GenerateDisplay(uint32_t* width, uint32_t* height, bool vsync, std::vector<VkGLTFAsset*>* Assets);
		void RecordCommandBuffers(std::vector<VkGLTFAsset*>* Assets);
		void render (std::vector<VkGLTFAsset*>* Assets);




		//helpers
		void cleanUpBuffer(VkDevice* Device, Buffer* buffer);
		VkPipelineShaderStageCreateInfo LoadShader(std::string Filename, VkShaderStageFlagBits Stage);
		uint32_t getQueueFamilyIndex(VkQueueFlags queueFlags) const;
		inline VkDevice GetDevice() { return Device; }
		void PushExtensionsandFeatures(std::vector<const char*> EnabledInstanceextensions, std::vector<const char*> EnabledDeviceextensions,
			VkPhysicalDeviceFeatures EnableddeviceFeatures);
		bool IsDeviceExtensionSupported(const std::string& DeviceextensionName) const;
		bool IsInstanceExtensionSupported(const std::string& InstanceextensionName) const;

	protected:
		//Instance
		 VkInstance Instance;
		 //the elements of an unordered_set are immutable
		 std::unordered_set<std::string> supportedInstanceExtensions;
		 std::vector<const char*> enabledInstanceExtensions;

		 //PhysicalDevice
		 VkPhysicalDevice PhysicalDevice;
		 VkPhysicalDeviceProperties deviceProperties;
		 VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		 VkPhysicalDeviceFeatures deviceFeatures;
		 std::unordered_set<std::string> SupportedDeviceExtensions;
		 std::vector<const char*> enabledDeviceExtensions;

		 //Device
		 VkDevice Device;
		 //memory
		 VulkanBufferMemory* MemoryHandle;

		 //Queue
		 VkQueue Queue;
		 struct
		 {
			 uint32_t graphics;
			 uint32_t compute;
			 uint32_t transfer;

		 } QueueTypeFlagBitIndex;

		 std::vector<VkQueueFamilyProperties> QueueFamilyProperties;
		 std::vector<VkDeviceQueueCreateInfo> QueueCI;


		 //Surface & Swapchain
		 VkSurfaceKHR Surface = VK_NULL_HANDLE;
		 uint32_t queueNodeIndex = UINT32_MAX;

		 VkFormat colorFormat;
		 VkColorSpaceKHR colorSpace;

		 bool SurfaceVsync = false;
		 uint32_t SurfaceHeight =0;
		 uint32_t SurfaceWidth = 0;
		 glm::mat4 projectionMatrix;
		 glm::mat4 viewMatrix;
		 glm::mat4 modelMatrix;
		 glm::vec3 CameraPosition;


		 VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
		 typedef struct _SwapChainBuffers {
			 VkImage image;
			 VkImageView imageview;
		 }SwapchainBuffer;
		 std::vector<SwapchainBuffer> SwapChainImageViewBufffer;
		 std::vector<VkImage> RetrievedImages;
		 uint32_t ImageCount= 0;

		 //Pipeline
		 VkPipelineCache PipelineCache;

		 //Shader
		 std::vector<VkShaderModule> ShaderModules;
		 VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;


		 //Submit
		 VkSubmitInfo Submitinfo;

		 //Renderpass
		 VkRenderPass Renderpass =VK_NULL_HANDLE;
		 //Depth
		 VkFormat DepthFormat;
		 struct {
			 VkImage image;
			 VkDeviceMemory memory;
			 VkImageView view;
		 } DepthStencil;

		 //Framebuffer
		 std::vector<VkFramebuffer> FrameBuffer;
		 //Semaphores
		 VkSemaphore DelayBeforeCommandExecution;
		 VkSemaphore CommandExecutionComplete;


		 //Fense
		 std::vector<VkFence> QueueFences;



		 //Cmd
		 VkCommandPool Commandpool = VK_NULL_HANDLE;
		 std::vector<VkCommandBuffer> CommandBuffers;
		 uint32_t ImageIndex = 0;

		 //PFN
		 PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = VK_NULL_HANDLE;
		 PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = VK_NULL_HANDLE;
		 PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = VK_NULL_HANDLE;
		 PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = VK_NULL_HANDLE;
		 PFN_vkQueuePresentKHR fpQueuePresentKHR = VK_NULL_HANDLE;
		 PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR = VK_NULL_HANDLE;
		 PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR = VK_NULL_HANDLE;
		 PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = VK_NULL_HANDLE;
		 PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR = VK_NULL_HANDLE;
		
	};


}