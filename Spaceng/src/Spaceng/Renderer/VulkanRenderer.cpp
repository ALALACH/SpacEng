#include "PCH.h"
#include "VulkanRenderer.h"

namespace Spaceng
{


	VulkanRenderer::~VulkanRenderer()
	{
		if (Swapchain != VK_NULL_HANDLE)
		{
			for (uint32_t i = 0; i < ImageCount; i++)
			{
				vkDestroyImageView(Device, SwapChainBuffers[i].imageview, nullptr);
			}
		}
		if (Surface != VK_NULL_HANDLE)
		{
			fpDestroySwapchainKHR(Device, Swapchain, nullptr);
			vkDestroySurfaceKHR(Instance, Surface, nullptr);
		}
		Surface = VK_NULL_HANDLE;
		Swapchain = VK_NULL_HANDLE;

		vkFreeCommandBuffers(Device, Commandpool, static_cast<uint32_t>(CommandBuffers.size()), CommandBuffers.data());
		vkDestroyCommandPool(Device, Commandpool, nullptr);

		for (uint32_t i = 0; i < FrameBuffer.size(); i++)
		{
			vkDestroyFramebuffer(Device, FrameBuffer[i], nullptr);
		}

		if (Renderpass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(Device, Renderpass, nullptr);
		}
		
		for (auto& shaderModule : ShaderModules)
		{
			vkDestroyShaderModule(Device, shaderModule, nullptr);
		}

		vkDestroyPipelineCache(Device, PipelineCache, nullptr);

		vkDestroySemaphore(Device, DelayBeforeCommandExecution, nullptr);
		vkDestroySemaphore(Device, CommandExecutionComplete, nullptr);
		for (auto& fence : QueueFences) {
			vkDestroyFence(Device, fence, nullptr);
		}

		vkDestroyDevice(Device, nullptr);
		vkDestroyInstance(Instance, nullptr);
	};


	void VulkanRenderer::InitRenderer(std::vector<const char*> EnabledInstanceextensions, std::vector<const char*> EnabledDeviceextensions,
		VkPhysicalDeviceFeatures EnableddeviceFeatures)
	{
		PushExtensionsandFeatures(EnabledInstanceextensions, EnabledDeviceextensions, EnableddeviceFeatures);
		CreateInstance();
		CreateDevice();
		CreatePipelineCache();
		CreateSemaphores();
		SetFuncPointer();
	};


	void VulkanRenderer::CreateInstance()
	{
		VkApplicationInfo appinfo = {};
		appinfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appinfo.pApplicationName = "SPACE ENGINE";
		appinfo.pEngineName = "SPACENG";
		appinfo.apiVersion = VK_API_VERSION_1_2;



#define VK_KHR_WIN32_SURFACE_EXTENSION_NAME "VK_KHR_win32_surface"
		std::vector<const char*> InstanceExtensions;
		InstanceExtensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);
		InstanceExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);


		//Validation-Debuging extensions
		if (Validation)
		{
			InstanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			InstanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			InstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
		}

		// Get list of supported extensions
		uint32_t extCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateInstanceExtensionProperties(nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				SE_LOG_DEBUG("----------------------")
				for (VkExtensionProperties extension : extensions)
				{
					supportedInstanceExtensions.emplace(extension.extensionName);
					SE_LOG_DEBUG("VKSupportedInstanceExtensions: {0}", extension.extensionName);
				}
				SE_LOG_DEBUG("----------------------")
			}
		}
		if (enabledInstanceExtensions.size() > 0)
		{
			for (const char* enabledExtension : enabledInstanceExtensions)
			{
				if (std::find(supportedInstanceExtensions.begin(), supportedInstanceExtensions.end(), enabledExtension) == supportedInstanceExtensions.end())
				{
					SE_ASSERT(enabledExtension, "Extension not supported");
				}
				InstanceExtensions.push_back(enabledExtension);
			}
		}

		VkInstanceCreateInfo instanceCreateInfo = {};
		instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instanceCreateInfo.pNext = NULL;
		instanceCreateInfo.pApplicationInfo = &appinfo;
		instanceCreateInfo.enabledExtensionCount = (uint32_t)InstanceExtensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = InstanceExtensions.data();

		if (Validation)
		{
			const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
			uint32_t instanceLayerCount;
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
			std::vector<VkLayerProperties> instanceLayerProperties(instanceLayerCount);
			vkEnumerateInstanceLayerProperties(&instanceLayerCount, instanceLayerProperties.data());
			bool validationLayerPresent = false;
			for (VkLayerProperties &layer : instanceLayerProperties) 
			{
				SE_LOG_DEBUG("InstanceLayers : {0}", layer.layerName);
				if (strcmp(layer.layerName, validationLayerName) == 0) {
					validationLayerPresent = true;
					break;
				}
			}
			SE_LOG_DEBUG("-------------------------")
			if (validationLayerPresent) {
				instanceCreateInfo.ppEnabledLayerNames = &validationLayerName;
				instanceCreateInfo.enabledLayerCount = 1;
			}
			else {
				SE_LOG_ERROR("Validation layer VK_LAYER_KHRONOS_validation not present, validation is disabled")
			}
		}
		VK_CHECK_RESULT(vkCreateInstance(&instanceCreateInfo, nullptr, &Instance));
	}


	void VulkanRenderer::CreateDevice()
	{
		//Physical Device
		uint32_t gpuCount = 0;
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(Instance, &gpuCount, nullptr));
		SE_ASSERT(gpuCount, "No Physical Device found!");

		std::vector<VkPhysicalDevice> physicalDevices(gpuCount);
		VK_CHECK_RESULT(vkEnumeratePhysicalDevices(Instance, &gpuCount, physicalDevices.data()));

		struct Properties
		{
			uint32_t apiVersion = 0;
			uint32_t driverVersion = 0;
			uint32_t VendorID = 0;
			uint32_t DeviceID = 0;
			const char* DeviceName = nullptr;
		}GPU_Properties;

		for (uint32_t i = 0; i < gpuCount; i++) {
			VkPhysicalDeviceProperties deviceProperties;
			vkGetPhysicalDeviceProperties(physicalDevices[i], &deviceProperties);
			GPU_Properties.DeviceName = deviceProperties.deviceName;
			GPU_Properties.DeviceID = deviceProperties.deviceID;
			GPU_Properties.apiVersion = deviceProperties.apiVersion;

			SE_LOG_WARN("----------- Device Nr {0} -----------",i)
			SE_LOG_INFO("Device: {0}", GPU_Properties.DeviceName);
			SE_LOG_INFO("DeviceID: {0}", GPU_Properties.DeviceID);
			std::string API = std::to_string((GPU_Properties.apiVersion >> 22)) +"."+ std::to_string(((GPU_Properties.apiVersion >> 12) & 0x3ff))+ "."
				+std::to_string((GPU_Properties.apiVersion & 0xfff)) + "\n";
			SE_LOG_INFO("VulkanAPI_Supported: {0}", API);
			SE_LOG_INFO("----------------------")
		}
		uint32_t selected = -1;
		while (selected<0  || selected>gpuCount-1)
		{ 
		SE_LOG_CRITICAL("Select a Device Nr : ...");
		std::cin >> selected;
		}
		PhysicalDevice = physicalDevices[selected];
		vkGetPhysicalDeviceProperties(PhysicalDevice, &deviceProperties);
		vkGetPhysicalDeviceFeatures(PhysicalDevice, &deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(PhysicalDevice, &deviceMemoryProperties);
		SE_LOG_WARN("Selected Device: {0}", deviceProperties.deviceName)
	
		//------------------------------------------------------------------------------------------------------------
		// Queue families
		// Desired queues need to be requested upon logical device creation
		// Due to differing queue family configurations of Vulkan implementations this can be a bit tricky, especially if the application
		// requests different queue types

		// Get queue family indices for the requested queue family types
		// Note that the indices may overlap depending on the implementation

		uint32_t QueueFamilyCount;
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, nullptr);
		SE_ASSERT(QueueFamilyCount, "No Device Queues Available");
		SE_LOG_DEBUG("----------------------")
		SE_LOG_DEBUG("VKQueueCount:{0}", QueueFamilyCount);
		SE_LOG_DEBUG("----------------------")

		QueueFamilyProperties.resize(QueueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &QueueFamilyCount, QueueFamilyProperties.data());
		

		const float defaultQueuePriority(0.0f);
		VkQueueFlags requestedQueueFlag = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT; 
		// Graphics queue
		if (requestedQueueFlag & VK_QUEUE_GRAPHICS_BIT)
		{
			QueueTypeFlagBitIndex.graphics = getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT);
			VkDeviceQueueCreateInfo queueInfo{};
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = QueueTypeFlagBitIndex.graphics;
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = &defaultQueuePriority;
			QueueCI.push_back(queueInfo);
		}
		else
		{
			QueueTypeFlagBitIndex.graphics = 0;
		}

		// Dedicated compute queue
		if (requestedQueueFlag & VK_QUEUE_COMPUTE_BIT)
		{
			QueueTypeFlagBitIndex.compute = getQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT);
			if (QueueTypeFlagBitIndex.compute != QueueTypeFlagBitIndex.graphics)
			{
				// If compute family index differs, we need an additional queue create info for the compute queue
				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = QueueTypeFlagBitIndex.compute;
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				QueueCI.push_back(queueInfo);
			}
		}
		else
		{
			// Else we use the same queue
			QueueTypeFlagBitIndex.compute = QueueTypeFlagBitIndex.graphics;
		}

		// Dedicated transfer queue
		if (requestedQueueFlag & VK_QUEUE_TRANSFER_BIT)
		{
			QueueTypeFlagBitIndex.transfer = getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT);
			if ((QueueTypeFlagBitIndex.transfer != QueueTypeFlagBitIndex.graphics) && (QueueTypeFlagBitIndex.transfer != QueueTypeFlagBitIndex.compute))
			{
				// If transfer family index differs, we need an additional queue create info for the transfer queue
				VkDeviceQueueCreateInfo queueInfo{};
				queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueInfo.queueFamilyIndex = QueueTypeFlagBitIndex.transfer;
				queueInfo.queueCount = 1;
				queueInfo.pQueuePriorities = &defaultQueuePriority;
				QueueCI.push_back(queueInfo);
			}
		}
		else
		{
			// Else we use the same queue
			QueueTypeFlagBitIndex.transfer = QueueTypeFlagBitIndex.graphics;
		}
		//------------------------------------------------------------------------------------------------------------
		// Get list of supported Device extensions
		std::vector<const char*> DeviceExtensions;

		uint32_t extCount = 0;
		vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extCount, nullptr);
		if (extCount > 0)
		{
			std::vector<VkExtensionProperties> extensions(extCount);
			if (vkEnumerateDeviceExtensionProperties(PhysicalDevice, nullptr, &extCount, &extensions.front()) == VK_SUCCESS)
			{
				for (auto& ext : extensions)
				{
					SupportedDeviceExtensions.emplace(ext.extensionName);
					SE_LOG_DEBUG("VKSupportedDeviceExtensions: {0}", ext.extensionName);
				}
				SE_LOG_DEBUG("----------------------")
			}
		}
		if (enabledDeviceExtensions.size() > 0)
		{
			for (const char* enabledExtension : enabledDeviceExtensions)
			{
				if (std::find(SupportedDeviceExtensions.begin(), SupportedDeviceExtensions.end(), enabledExtension) == SupportedDeviceExtensions.end())
				{
					SE_ASSERT(enabledExtension, "Extension not supported");
				}
				DeviceExtensions.push_back(enabledExtension);
			}
		}
		//------------------------------------------------------------------------------------------------------------
		//Device
		VkDeviceCreateInfo DeviceCreateInfo{};
		DeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		DeviceCreateInfo.pNext = NULL;
		DeviceCreateInfo.flags = 0;
		DeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(QueueCI.size());;
		DeviceCreateInfo.pQueueCreateInfos = QueueCI.data();
		DeviceCreateInfo.pEnabledFeatures = &deviceFeatures;
		if (DeviceExtensions.size() > 0)
		{
			DeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(DeviceExtensions.size());
			DeviceCreateInfo.ppEnabledExtensionNames = DeviceExtensions.data();
		}
		VK_CHECK_RESULT(vkCreateDevice(PhysicalDevice, &DeviceCreateInfo, nullptr, &Device));


		vkGetDeviceQueue(Device, QueueTypeFlagBitIndex.graphics,0, &Queue);
	}


	void VulkanRenderer::CreatePipelineCache()
	{
		VkPipelineCacheCreateInfo PipelineCacheCI = {};
		PipelineCacheCI.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
		PipelineCacheCI.pNext = NULL;
		VK_CHECK_RESULT(vkCreatePipelineCache(Device, &PipelineCacheCI, nullptr, &PipelineCache));
	}


	void VulkanRenderer::CreateSemaphores()
	{
		VkSemaphoreCreateInfo SemaphoreCI{};
		SemaphoreCI.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK_RESULT(vkCreateSemaphore(Device, &SemaphoreCI, nullptr, &DelayBeforeCommandExecution));
		VK_CHECK_RESULT(vkCreateSemaphore(Device, &SemaphoreCI, nullptr, &CommandExecutionComplete));
	}
	void VulkanRenderer::SetFuncPointer()
	{
		fpGetPhysicalDeviceSurfaceSupportKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(vkGetInstanceProcAddr(Instance, "vkGetPhysicalDeviceSurfaceSupportKHR"));
		fpGetPhysicalDeviceSurfaceCapabilitiesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR>(vkGetInstanceProcAddr(Instance, "vkGetPhysicalDeviceSurfaceCapabilitiesKHR"));
		fpGetPhysicalDeviceSurfaceFormatsKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceFormatsKHR>(vkGetInstanceProcAddr(Instance, "vkGetPhysicalDeviceSurfaceFormatsKHR"));
		fpGetPhysicalDeviceSurfacePresentModesKHR = reinterpret_cast<PFN_vkGetPhysicalDeviceSurfacePresentModesKHR>(vkGetInstanceProcAddr(Instance, "vkGetPhysicalDeviceSurfacePresentModesKHR"));
		fpCreateSwapchainKHR = reinterpret_cast<PFN_vkCreateSwapchainKHR>(vkGetDeviceProcAddr(Device, "vkCreateSwapchainKHR"));
		fpDestroySwapchainKHR = reinterpret_cast<PFN_vkDestroySwapchainKHR>(vkGetDeviceProcAddr(Device, "vkDestroySwapchainKHR"));
		fpGetSwapchainImagesKHR = reinterpret_cast<PFN_vkGetSwapchainImagesKHR>(vkGetDeviceProcAddr(Device, "vkGetSwapchainImagesKHR"));
		fpAcquireNextImageKHR = reinterpret_cast<PFN_vkAcquireNextImageKHR>(vkGetDeviceProcAddr(Device, "vkAcquireNextImageKHR"));
		fpQueuePresentKHR = reinterpret_cast<PFN_vkQueuePresentKHR>(vkGetDeviceProcAddr(Device, "vkQueuePresentKHR"));

	}

	void VulkanRenderer::SetupDepthBuffer()
	{
		//Img
		//Memalloc
		//Imgview
		//implement in Destructor
		//TODO : Implement Depthbuffers later for Framebuffers and Renderpass
	}


	void VulkanRenderer::CreateSurface(GLFWwindow* Window)
	{
		//------------------------------------------------------------------------------------------------------------
		//Init Surface

		//glfw Surface Binding
		glfwCreateWindowSurface(Instance, Window, nullptr, &Surface);

		// Get available queue family properties
		uint32_t queueCount;
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, NULL);

		std::vector<VkQueueFamilyProperties> QueueFamilyProperties(queueCount);
		vkGetPhysicalDeviceQueueFamilyProperties(PhysicalDevice, &queueCount, QueueFamilyProperties.data());

		// Iterate over each queue to learn whether it supports presenting:
		// Find a queue with present support
		// Will be used to present the swap chain images to the windowing system
		std::vector<VkBool32> QueueSupportsSurfacePresent(queueCount);
		for (uint32_t i = 0; i < queueCount; i++)
		{
			fpGetPhysicalDeviceSurfaceSupportKHR(PhysicalDevice, i, Surface, &QueueSupportsSurfacePresent[i]);
		}

		// Search for a graphics and a present queue in the array of queue
		// families, try to find one that supports both
		uint32_t graphicsQueueNodeIndex = UINT32_MAX;
		uint32_t presentQueueNodeIndex = UINT32_MAX;
		for (uint32_t i = 0; i < queueCount; i++)
		{
			if ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
			{
				if (graphicsQueueNodeIndex == UINT32_MAX)
				{
					graphicsQueueNodeIndex = i;
				}

				if (QueueSupportsSurfacePresent[i] == VK_TRUE)
				{
					graphicsQueueNodeIndex = i;
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		if (presentQueueNodeIndex == UINT32_MAX)
		{
			// If there's no queue that supports both present and graphics
			// try to find a separate present queue
			for (uint32_t i = 0; i < queueCount; ++i)
			{
				if (QueueSupportsSurfacePresent[i] == VK_TRUE)
				{
					presentQueueNodeIndex = i;
					break;
				}
			}
		}
		// Exit if either a graphics or a presenting queue hasn't been found
		SE_ASSERT(graphicsQueueNodeIndex != UINT32_MAX,"")
		SE_ASSERT(presentQueueNodeIndex != UINT32_MAX,"")
		
		queueNodeIndex = graphicsQueueNodeIndex;

		// Surface Formats & Space
		uint32_t formatCount;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &formatCount, NULL));
		assert(formatCount > 0);

		std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceFormatsKHR(PhysicalDevice, Surface, &formatCount, surfaceFormats.data()));


		if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
		{
			colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
			colorSpace = surfaceFormats[0].colorSpace;
		}
		else
		{
			bool found_B8G8R8A8_UNORM = false;
			for (auto&& surfaceFormat : surfaceFormats)
			{
				if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
				{
					colorFormat = surfaceFormat.format;
					colorSpace = surfaceFormat.colorSpace;
					found_B8G8R8A8_UNORM = true;
					break;
				}
			}
			if (!found_B8G8R8A8_UNORM)
			{
				colorFormat = surfaceFormats[0].format;
				colorSpace = surfaceFormats[0].colorSpace;
			}
		}

		//RenderPass : required by Pipeline, Requires Format , can't be refreshed
		VkAttachmentDescription colorAttachmentDescription{};
		colorAttachmentDescription.format = colorFormat;
		colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorReference = {};
		colorReference.attachment = 0;
		colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription = {};
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorReference;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;
		subpassDescription.pResolveAttachments = nullptr;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachmentDescription;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpassDescription;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK_RESULT(vkCreateRenderPass(Device, &renderPassInfo, nullptr, &Renderpass));
	}


	void VulkanRenderer::CreateSwapChain(uint32_t *width, uint32_t *height, bool vsync)
	{
		VkSwapchainKHR OldSwapChain = Swapchain;

		//capabilities : Format and Properties
		VkSurfaceCapabilitiesKHR SurfCaps;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfaceCapabilitiesKHR(PhysicalDevice, Surface, &SurfCaps));

		//Extent
		VkExtent2D SwapchainExtent = {};
		if (SurfCaps.currentExtent.width == (uint32_t)-1)
		{
			SwapchainExtent.height = *height;
			SwapchainExtent.width = *width;
		}
		else
		{
			SwapchainExtent = SurfCaps.currentExtent;
			*height = SurfCaps.currentExtent.height;
			*width = SurfCaps.currentExtent.width;
		}
		SurfaceWidth = *width;
		SurfaceHeight = *height;

		//Present Modes
		uint32_t presentmodecount;
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &presentmodecount, NULL));
		SE_ASSERT(presentmodecount > 0, "No present mode supported");
		std::vector<VkPresentModeKHR> Presentmodes(presentmodecount);
		VK_CHECK_RESULT(fpGetPhysicalDeviceSurfacePresentModesKHR(PhysicalDevice, Surface, &presentmodecount, Presentmodes.data()));
		VkPresentModeKHR PresentMode = VK_PRESENT_MODE_FIFO_KHR;
		SurfaceVsync = vsync;

		if (!vsync)
		{
			for (size_t i = 0; i < presentmodecount; i++)
			{
				if (Presentmodes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					PresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if ((PresentMode != VK_PRESENT_MODE_MAILBOX_KHR) && (Presentmodes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR))
				{
					PresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
				}
			}
		}
		//Number of Surface Images
		uint32_t NumberOfswapchainImages = SurfCaps.minImageCount + 1;
		if ((SurfCaps.maxImageCount > 0) && (NumberOfswapchainImages > SurfCaps.maxImageCount))
		{
			NumberOfswapchainImages = SurfCaps.maxImageCount;
		}

		//preTransform : mirroring and rotation
		VkSurfaceTransformFlagsKHR preTransform;
		if (SurfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
		{
			preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
		}
		else
		{
			preTransform = SurfCaps.currentTransform;
		}

		//Composite Alpha :indicating the alpha compositing mode to use when this surface is composited together with other surfaces on certain window systems.
		VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		
		std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
			VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
			VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
			VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
		};
		for (auto& compositeAlphaFlag : compositeAlphaFlags) {
			if (SurfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
				compositeAlpha = compositeAlphaFlag;
				break;
			};
		}

		//SwapchainCI
		VkSwapchainCreateInfoKHR SwapchainCI = {};
		SwapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		SwapchainCI.pNext = NULL;
		SwapchainCI.surface = Surface;
		SwapchainCI.minImageCount = NumberOfswapchainImages;
		SwapchainCI.imageFormat = colorFormat;
		SwapchainCI.imageColorSpace = colorSpace;
		SwapchainCI.imageExtent = { SwapchainExtent.width, SwapchainExtent.height };
		SwapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		SwapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
		SwapchainCI.imageArrayLayers = 1;
		SwapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		SwapchainCI.queueFamilyIndexCount = 0;
		SwapchainCI.pQueueFamilyIndices = NULL;
		SwapchainCI.presentMode = PresentMode;
		SwapchainCI.oldSwapchain = OldSwapChain;
		SwapchainCI.clipped = VK_TRUE; //discard rendering operations that affect regions of the surface that are not visible
		SwapchainCI.compositeAlpha = compositeAlpha;
		if (SurfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
			SwapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		}
		if (SurfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
			SwapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		// Create Swapchain
		VK_CHECK_RESULT(fpCreateSwapchainKHR(Device, &SwapchainCI, nullptr, &Swapchain));

		// Old swapchain
		if (OldSwapChain)
		{
			for (uint32_t i = 0; i < ImageCount; i++)
			{
				vkDestroyImageView(Device, SwapChainBuffers[i].imageview, nullptr);
			}
			fpDestroySwapchainKHR(Device, OldSwapChain, nullptr);
		}
		//retrieve Imagecount
		VK_CHECK_RESULT(fpGetSwapchainImagesKHR(Device, Swapchain, &ImageCount, NULL));

		//retrieve Images from Swapchain
		RetrievedImages.clear();
		RetrievedImages.resize(ImageCount);
		VK_CHECK_RESULT(fpGetSwapchainImagesKHR(Device, Swapchain, &ImageCount, RetrievedImages.data()));

		SwapChainBuffers.clear();
		SwapChainBuffers.resize(ImageCount);

		for (uint32_t i = 0; i < ImageCount; i++)
		{
			VkImageViewCreateInfo SwapChainImageViewCI = {};
			SwapChainImageViewCI.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			SwapChainImageViewCI.pNext = NULL;
			SwapChainImageViewCI.format = colorFormat;
			SwapChainImageViewCI.components = {
				VK_COMPONENT_SWIZZLE_R,
				VK_COMPONENT_SWIZZLE_G,
				VK_COMPONENT_SWIZZLE_B,
				VK_COMPONENT_SWIZZLE_A
			};

			SwapChainImageViewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
			SwapChainImageViewCI.flags = 0;

			SwapChainImageViewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			SwapChainImageViewCI.subresourceRange.baseMipLevel = 0;
			SwapChainImageViewCI.subresourceRange.levelCount = 1;
			SwapChainImageViewCI.subresourceRange.baseArrayLayer = 0;
			SwapChainImageViewCI.subresourceRange.layerCount = 1;


			SwapChainBuffers[i].image = RetrievedImages[i];
			SwapChainImageViewCI.image = SwapChainBuffers[i].image;

			VK_CHECK_RESULT(vkCreateImageView(Device, &SwapChainImageViewCI, nullptr, &SwapChainBuffers[i].imageview));
		}

		//Commandpool & CommandBuffers
		vkDestroyCommandPool(Device, Commandpool, nullptr);
		
		VkCommandPoolCreateInfo CommandPoolCI = {};
		CommandPoolCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		CommandPoolCI.queueFamilyIndex = queueNodeIndex;
		CommandPoolCI.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		VK_CHECK_RESULT(vkCreateCommandPool(Device, &CommandPoolCI, nullptr, &Commandpool));


		CommandBuffers.clear();
		CommandBuffers.resize(ImageCount);

		VkCommandBufferAllocateInfo CommandbufferAllocateInfo = {};
		CommandbufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		CommandbufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		CommandbufferAllocateInfo.commandBufferCount = 1;
		CommandbufferAllocateInfo.commandPool = Commandpool;

		for (VkCommandBuffer& commandBuffer : CommandBuffers)
		{
			VK_CHECK_RESULT(vkAllocateCommandBuffers(Device, &CommandbufferAllocateInfo, &commandBuffer));
		}

		//Framebuffer
		for (VkFramebuffer& framebuffer : FrameBuffer)
			vkDestroyFramebuffer(Device, framebuffer, nullptr);

		VkFramebufferCreateInfo FramebufferCI {};
		FramebufferCI.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		FramebufferCI.renderPass = Renderpass;
		FramebufferCI.attachmentCount = 1;
		FramebufferCI.width = *width;
		FramebufferCI.height = *height;
		FramebufferCI.layers = 1;

		FrameBuffer.clear();
		FrameBuffer.resize(ImageCount);

		for (uint32_t i = 0; i < FrameBuffer.size(); i++)
		{
			FramebufferCI.pAttachments = &SwapChainBuffers[i].imageview;
			VK_CHECK_RESULT(vkCreateFramebuffer(Device, &FramebufferCI, nullptr, &FrameBuffer[i]));
		}

		//Synchronisation premitives
		for (auto& fence : QueueFences)
			vkDestroyFence(Device, fence, nullptr);

		QueueFences.clear();
		QueueFences.resize(CommandBuffers.size());
		VkFenceCreateInfo FenceCI = {};
		FenceCI.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		FenceCI.flags = VK_FENCE_CREATE_SIGNALED_BIT; //fence object is created in the signaled state

		for (uint32_t i = 0; i < QueueFences.size(); i++)
			VK_CHECK_RESULT(vkCreateFence(Device, &FenceCI, nullptr, &QueueFences[i]));
	}


	void VulkanRenderer::prepareUniformBuffer(VkGLTFAsset* Asset,bool mapAccess, bool descriptorAcess)
	{
		VulkanBufferMemory::AllocateBufferMemory(Asset->UniformBuffer, Device, deviceMemoryProperties, descriptorAcess, mapAccess, nullptr);
		updateUniformBuffer(Asset);
	}


	void VulkanRenderer::updateUniformBuffer(VkGLTFAsset* Asset)
	{
		//writes to memory block not needed so no VK_MEMORY_PROPERTY_HOST_COHERENT_BIT 
		memcpy(Asset->UniformBuffer.mapped, &Asset->UBOMatrices, sizeof(Asset->UBOMatrices));
	}


	void VulkanRenderer::prepareDescriptorSet(VkGLTFAsset* Asset)
	{
		uint32_t type = Asset->getType();
		if (type == MeshType)
		{
			//descriptorSet Layout
			std::vector<VkDescriptorSetLayoutBinding> LayoutBindings {};
			
			VkDescriptorSetLayoutBinding Binding1{};
			Binding1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			Binding1.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			Binding1.binding = 0;
			Binding1.descriptorCount = 1;

			VkDescriptorSetLayoutBinding Binding2{};
			Binding2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			Binding2.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			Binding2.binding = 1;
			Binding2.descriptorCount = 1;

			LayoutBindings.push_back(Binding1);
			LayoutBindings.push_back(Binding2);

			VkDescriptorSetLayoutCreateInfo descriptorLayoutCI = {};
			descriptorLayoutCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorLayoutCI.pBindings = LayoutBindings.data();
			descriptorLayoutCI.bindingCount = 2;
			VK_CHECK_RESULT(vkCreateDescriptorSetLayout(Device, &descriptorLayoutCI, nullptr, &Asset->DescriptorSetLayout));

			//descriptor Pool
			std::vector<VkDescriptorPoolSize> DescriptorPoolSizes = {};

			VkDescriptorPoolSize PoolSize1 {};
			PoolSize1.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			PoolSize1.descriptorCount = 1;

			VkDescriptorPoolSize PoolSize2 {};
			PoolSize2.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			PoolSize2.descriptorCount = 1;

			DescriptorPoolSizes.push_back(PoolSize1);
			DescriptorPoolSizes.push_back(PoolSize2);

			VkDescriptorPoolCreateInfo DescriptorpoolCI {};
			DescriptorpoolCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			DescriptorpoolCI.poolSizeCount = 2;
			DescriptorpoolCI.maxSets = 1;
			DescriptorpoolCI.pPoolSizes = DescriptorPoolSizes.data();

			VK_CHECK_RESULT(vkCreateDescriptorPool(Device, &DescriptorpoolCI, nullptr, &Asset->DescriptorPool));

			//DescriptorSet (Alloc / Update)
			VkDescriptorSetAllocateInfo DescriptorAllocateInfo {};
			DescriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			DescriptorAllocateInfo.descriptorPool = Asset->DescriptorPool;
			DescriptorAllocateInfo.pSetLayouts = &Asset->DescriptorSetLayout;
			DescriptorAllocateInfo.descriptorSetCount = 1;

			VK_CHECK_RESULT(vkAllocateDescriptorSets(Device, &DescriptorAllocateInfo, &Asset->DescriptorSet));


			std::vector<VkWriteDescriptorSet> writeDescriptorSets;

			VkWriteDescriptorSet writeDescriptorSet1{};
			writeDescriptorSet1.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet1.dstSet = Asset->DescriptorSet;
			writeDescriptorSet1.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			writeDescriptorSet1.dstBinding = 0;
			writeDescriptorSet1.pBufferInfo = &Asset->UniformBuffer.BufferDescriptor;
			writeDescriptorSet1.descriptorCount = 1;

			VkWriteDescriptorSet writeDescriptorSet2{};
			writeDescriptorSet2.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSet2.dstSet = Asset->DescriptorSet;
			writeDescriptorSet2.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			writeDescriptorSet2.dstBinding = 1;
			writeDescriptorSet2.pImageInfo = &Asset->TextureDescriptor; //update once implemented and allocate memory for the texture object
			writeDescriptorSet2.descriptorCount = 1;

			writeDescriptorSets.push_back(writeDescriptorSet1);
			writeDescriptorSets.push_back(writeDescriptorSet2);

			//vkUpdateDescriptorSets(Device, writeDescriptorSets.size(), writeDescriptorSets.data(), 0, NULL);
		}
	}

	void VulkanRenderer::preparePipeline(VkGLTFAsset* Asset)
	{
		//PipelineLayout
		VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
		pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutCreateInfo.setLayoutCount = 1;
		pipelineLayoutCreateInfo.pSetLayouts = &Asset->DescriptorSetLayout;
		VK_CHECK_RESULT(vkCreatePipelineLayout(Device, &pipelineLayoutCreateInfo, nullptr, &Asset->PipelineLayout));

		//InputAssemblyState
		VkPipelineInputAssemblyStateCreateInfo InputAssemblyStateCI {};
		InputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		InputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		InputAssemblyStateCI.flags = 0;
		InputAssemblyStateCI.primitiveRestartEnable = VK_FALSE; // controls whether a special vertex index value is treated as restarting the assembly of primitives

		//RasterizationState
		VkPipelineRasterizationStateCreateInfo RasterizationStateCI {};
		RasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		RasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
		RasterizationStateCI.cullMode = VK_CULL_MODE_NONE; //can be experimented with
		RasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		RasterizationStateCI.flags = 0;
		RasterizationStateCI.depthClampEnable = VK_FALSE;
		RasterizationStateCI.lineWidth = 1.0f;

		//MultisampleState
		VkPipelineMultisampleStateCreateInfo MultiSampleCI {};
		MultiSampleCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		MultiSampleCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //read 1_bit image data and apply filtering and other transformations for the shader.
		MultiSampleCI.flags = 0;

		//ColorBlendState
		VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState {};
		pipelineColorBlendAttachmentState.colorWriteMask = 0xf;
		pipelineColorBlendAttachmentState.blendEnable = VK_FALSE;
		VkPipelineColorBlendStateCreateInfo ColorBlendStateCI {};
		ColorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		ColorBlendStateCI.pAttachments = &pipelineColorBlendAttachmentState;
		ColorBlendStateCI.attachmentCount = 1;

		//DepthStencil
		VkPipelineDepthStencilStateCreateInfo DepthStencilCI {};
		DepthStencilCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		DepthStencilCI.depthTestEnable = VK_TRUE;
		DepthStencilCI.depthWriteEnable = VK_TRUE;
		DepthStencilCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		DepthStencilCI.back.compareOp = VK_COMPARE_OP_ALWAYS;
		
		//ViewportState
		VkPipelineViewportStateCreateInfo pipelineViewportStateCI{};
		pipelineViewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		pipelineViewportStateCI.viewportCount = 1;
		pipelineViewportStateCI.scissorCount = 1;
		pipelineViewportStateCI.flags = 0;

		//DynamicState
		VkPipelineDynamicStateCreateInfo DynamicStateCI {};
		std::vector<VkDynamicState> EnabledDynamicState = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
		DynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		DynamicStateCI.pDynamicStates = EnabledDynamicState.data();
		DynamicStateCI.dynamicStateCount = static_cast<uint32_t>(EnabledDynamicState.size());
		DynamicStateCI.flags = 0;

		//ShaderStages
		std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};
		shaderStages[0] = LoadShader(Asset->VertexShaderFile, VK_SHADER_STAGE_VERTEX_BIT);
		shaderStages[1] = LoadShader(Asset->FragmentShaderFile, VK_SHADER_STAGE_FRAGMENT_BIT);

		//VertexInputState                        
		VkPipelineVertexInputStateCreateInfo VertexInputState {};
		VertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		// to do : (empty) to be implemented for further use



		//PipelineCreateInfo
		VkGraphicsPipelineCreateInfo PipelineCI {};
		PipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		PipelineCI.layout = Asset->PipelineLayout;
		PipelineCI.renderPass = Renderpass;
		PipelineCI.flags = 0;
		PipelineCI.basePipelineIndex = -1;
		PipelineCI.basePipelineHandle = VK_NULL_HANDLE;
		PipelineCI.pInputAssemblyState = &InputAssemblyStateCI;
		PipelineCI.pRasterizationState = &RasterizationStateCI;
		PipelineCI.pMultisampleState = &MultiSampleCI;
		PipelineCI.pColorBlendState = &ColorBlendStateCI;
		if (Asset->DepthStencilEnabled){ 
		PipelineCI.pDepthStencilState = &DepthStencilCI;}
		PipelineCI.pViewportState = &pipelineViewportStateCI;
		PipelineCI.pDynamicState = &DynamicStateCI;
		PipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
		PipelineCI.pStages = shaderStages.data();
		PipelineCI.pVertexInputState = &VertexInputState;

		VK_CHECK_RESULT(vkCreateGraphicsPipelines(Device, PipelineCache, 1, &PipelineCI, nullptr, &Asset->Pipeline));
	}



	void VulkanRenderer::cleanUpBuffer(VkDevice Device ,VkBuffer* buffer, VkDeviceMemory* memory)
	{
		VulkanBufferMemory::DeallocateBufferMemory(Device, buffer, memory);
	}

	void VulkanRenderer::PrepareAsset(VkGLTFAsset* Asset, AssetType Type , std::string filepath)
	{
		Asset->LoadFromFile(filepath);
		prepareUniformBuffer(Asset);
		prepareDescriptorSet(Asset);
		preparePipeline(Asset);
	}

	void VulkanRenderer::CleanUpAsset(VkGLTFAsset* Asset)
	{
		cleanUpBuffer(Device, &Asset->UniformBuffer.buffer, &Asset->UniformBuffer.memory); //Uniform Bufffer
		vkDestroyDescriptorPool(Device, Asset->DescriptorPool, nullptr);
		vkDestroyPipeline(Device, Asset->Pipeline, nullptr);
		vkDestroyPipelineLayout(Device, Asset->PipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(Device, Asset->DescriptorSetLayout, nullptr);
		
		//destroy Draw Buffers
		//destroy commandBuffers
	}


	VkPipelineShaderStageCreateInfo VulkanRenderer::LoadShader(std::string Filename, VkShaderStageFlagBits Stage)
	{
		const char* file = Filename.c_str();
		std::ifstream is(file, std::ios::binary | std::ios::in | std::ios::ate);
		SE_ASSERT(is.is_open(), "Could not open Shader File")

		size_t size = is.tellg();
		is.seekg(0, std::ios::beg);
		char* shaderCode = new char[size];
		is.read(shaderCode, size);
		is.close();

		assert(size > 0);

		VkShaderModule shaderModule;
		VkShaderModuleCreateInfo moduleCreateInfo{};
		moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		moduleCreateInfo.codeSize = size;
		moduleCreateInfo.pCode = (uint32_t*)shaderCode;

		VK_CHECK_RESULT(vkCreateShaderModule(Device, &moduleCreateInfo, NULL, &shaderModule));

		delete[] shaderCode;
		VkPipelineShaderStageCreateInfo shaderstage = {};
		shaderstage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderstage.stage = Stage;
		shaderstage.module = shaderModule;
		shaderstage.pName = "main";
		SE_ASSERT(shaderstage.module, "ShaderStage Module not Loaded");
		ShaderModules.push_back(shaderstage.module);
		
		return shaderstage;
	}


	void VulkanRenderer::Refresh(uint32_t* width, uint32_t* height, bool vsync)
	{
		vkDeviceWaitIdle(Device);
		CreateSwapChain(width, height, vsync);
		vkDeviceWaitIdle(Device);
		//todo: GUI Aspectratio
		//todo: Camera AspectRatio 
		//todo: RecordCommandBuffers
		//todo: update uniform Buffers (Dynamic)
		//set refresh bool to true
	}
	void VulkanRenderer::render()
	{
		Submitinfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		Submitinfo.pWaitDstStageMask = &submitPipelineStages;
		Submitinfo.waitSemaphoreCount = 1;
		Submitinfo.pWaitSemaphores = &DelayBeforeCommandExecution;
		Submitinfo.signalSemaphoreCount = 1;
		Submitinfo.pSignalSemaphores = &CommandExecutionComplete;

		VkResult result = fpAcquireNextImageKHR(Device, Swapchain, UINT64_MAX, DelayBeforeCommandExecution, (VkFence)nullptr, &ImageIndex);
		// the application must use semaphore and/or fence to ensure that the image layout and contents are not modified 
		// until the presentation engine reads have completed. the application must use semaphore and/or fence 
		// to ensure that the image layout and contents are not modified until the presentation engine reads have completed.
	
		if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) 
		// Check the Swapchain and Surface Compatability
		{
			if (result == VK_ERROR_OUT_OF_DATE_KHR){
			Refresh(&SurfaceWidth,&SurfaceHeight,SurfaceVsync);
			}
			return;
		}
		else {
			VK_CHECK_RESULT(result);
		}
		// Use a fence to wait until the command buffer has finished execution before using it again
		VK_CHECK_RESULT(vkWaitForFences(Device, 1, &QueueFences[ImageIndex], VK_TRUE, UINT64_MAX));
		VK_CHECK_RESULT(vkResetFences(Device, 1, &QueueFences[ImageIndex]));

		Submitinfo.commandBufferCount = 1;
		Submitinfo.pCommandBuffers = &CommandBuffers[ImageIndex]; // Command buffers(s) to execute in this batch (submission)

		VK_CHECK_RESULT(vkQueueSubmit(Queue, 1, &Submitinfo, QueueFences[ImageIndex]));
		//can use fence when Multiviewing /Multithreading: to insert a dependency from a queue to the host.

		VkPresentInfoKHR presentInfo = {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = NULL;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &Swapchain;
		presentInfo.pImageIndices = &ImageIndex;
		// pImageIndices is a pointer to an array of indices into the array of each swapchain’s presentable images, with swapchainCount entries.
		// Each entry in this array identifies the image to present on the corresponding entry in the pSwapchains array.
		
		// Check if a wait semaphore has been specified to wait for before presenting the image
		if (CommandExecutionComplete != VK_NULL_HANDLE)
		{
			presentInfo.pWaitSemaphores = &CommandExecutionComplete;
			presentInfo.waitSemaphoreCount = 1;
		}
		result = fpQueuePresentKHR(Queue, &presentInfo);
		if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) {
			Refresh(&SurfaceWidth, &SurfaceHeight, SurfaceVsync);
			if (result == VK_ERROR_OUT_OF_DATE_KHR) {
				return;
			}
		}
		else {
			VK_CHECK_RESULT(result);
		}
		VK_CHECK_RESULT(vkQueueWaitIdle(Queue));
	}



	void VulkanRenderer::PushExtensionsandFeatures(std::vector<const char*> EnabledInstanceextensions, std::vector<const char*> EnabledDeviceextensions,
		VkPhysicalDeviceFeatures EnabledFeatures)
	{
		for (const char* enabledExtension : EnabledInstanceextensions)
		{
			enabledInstanceExtensions.push_back(enabledExtension);
		}
		for (const char* enabledExtension : EnabledDeviceextensions)
		{
			enabledDeviceExtensions.push_back(enabledExtension);
		}
		deviceFeatures = EnabledFeatures;
	}


	bool VulkanRenderer::IsDeviceExtensionSupported(const std::string& DeviceextensionName) const
	{
		return SupportedDeviceExtensions.find(DeviceextensionName) != SupportedDeviceExtensions.end();
	}


	bool VulkanRenderer::IsInstanceExtensionSupported(const std::string& DeviceextensionName) const
	{
		return supportedInstanceExtensions.find(DeviceextensionName) != supportedInstanceExtensions.end();
	}


	uint32_t VulkanRenderer::getQueueFamilyIndex(VkQueueFlags queueFlags) const
	{
		// Dedicated queue for compute
		// Try to find a queue family index that supports compute but not graphics
		if ((queueFlags & VK_QUEUE_COMPUTE_BIT) == queueFlags)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(QueueFamilyProperties.size()); i++)
			{
				if ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) && ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
				{
					return i;
				}
			}
		}
		// Dedicated queue for transfer
		// Try to find a queue family index that supports transfer but not graphics and compute
		if ((queueFlags & VK_QUEUE_TRANSFER_BIT) == queueFlags)
		{
			for (uint32_t i = 0; i < static_cast<uint32_t>(QueueFamilyProperties.size()); i++)
			{
				if ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) && ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((QueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
				{
					return i;
				}
			}
		}
		// For other queue types or if no separate compute queue is present, return the first one to support the requested flags
		for (uint32_t i = 0; i < static_cast<uint32_t>(QueueFamilyProperties.size()); i++)
		{
			if ((QueueFamilyProperties[i].queueFlags & queueFlags) == queueFlags)
			{
				return i;
			}
		}
		SE_LOG_ERROR("Could not find a matching queue family index");
		__debugbreak();
		return UINT32_MAX;
	}


}
