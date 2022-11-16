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
		void InitRenderer(std::vector<const char*> EnabledInstanceextensions, std::vector<const char*> EnabledDeviceextensions,
			VkPhysicalDeviceFeatures EnableddeviceFeatures);

		void CreateInstance();
		void CreateDevice();
		void SetupDepthBuffer();
		void CreatePipelineCache();
		void CreateSemaphores();
		void SetFuncPointer();



		void CreateSurface(GLFWwindow* Window);
		void CreateSwapChain(uint32_t* width, uint32_t* height, bool vsync);

		void prepareUniformBuffer(VkGLTFAsset* Asset, bool mapAccess = true, bool descriptorAcess = true);
		void updateUniformBuffer(VkGLTFAsset* Asset);
		void prepareDescriptorSet(VkGLTFAsset* Asset);
		void preparePipeline(VkGLTFAsset* Asset);



		void PrepareAsset(VkGLTFAsset* Asset ,AssetType Type , std::string filename);
		void CleanUpAsset(VkGLTFAsset* Asset);


		void Refresh(uint32_t* width, uint32_t* height, bool vsync);
		void render();




		//helpers
		void cleanUpBuffer(VkDevice Device, VkBuffer* buffer, VkDeviceMemory* memory);
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

		 VkSwapchainKHR Swapchain = VK_NULL_HANDLE;
		 typedef struct _SwapChainBuffers {
			 VkImage image;
			 VkImageView imageview;
		 }SCBuffer;
		 std::vector<SCBuffer> SwapChainBuffers;
		 std::vector<VkImage> RetrievedImages;
		 uint32_t ImageCount= 0;

		 //Shader
		 std::vector<VkShaderModule> ShaderModules;

		 //Pipeline
		 VkPipelineCache PipelineCache;

		 //Submit
		 VkSubmitInfo Submitinfo;
		 VkPipelineStageFlags submitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		 //Renderpass
		 VkRenderPass Renderpass =VK_NULL_HANDLE;

		 //Framebuffer
		 std::vector<VkFramebuffer> FrameBuffer;

		 //Fense
		 std::vector<VkFence> QueueFences;

		 //Semaphores
		 VkSemaphore DelayBeforeCommandExecution;
		 VkSemaphore CommandExecutionComplete;


		 //Cmd
		 VkCommandPool Commandpool = VK_NULL_HANDLE;
		 std::vector<VkCommandBuffer> CommandBuffers;
		 uint32_t ImageIndex = 0;

		 //PFN
		 PFN_vkGetPhysicalDeviceSurfaceSupportKHR fpGetPhysicalDeviceSurfaceSupportKHR = VK_NULL_HANDLE;
		 PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR fpGetPhysicalDeviceSurfaceCapabilitiesKHR = VK_NULL_HANDLE;
		 PFN_vkGetPhysicalDeviceSurfaceFormatsKHR fpGetPhysicalDeviceSurfaceFormatsKHR = VK_NULL_HANDLE;
		 PFN_vkGetPhysicalDeviceSurfacePresentModesKHR fpGetPhysicalDeviceSurfacePresentModesKHR = VK_NULL_HANDLE;
		 PFN_vkCreateSwapchainKHR fpCreateSwapchainKHR = VK_NULL_HANDLE;
		 PFN_vkDestroySwapchainKHR fpDestroySwapchainKHR = VK_NULL_HANDLE;
		 PFN_vkGetSwapchainImagesKHR fpGetSwapchainImagesKHR = VK_NULL_HANDLE;
		 PFN_vkAcquireNextImageKHR fpAcquireNextImageKHR = VK_NULL_HANDLE;
		 PFN_vkQueuePresentKHR fpQueuePresentKHR = VK_NULL_HANDLE;
	};


}