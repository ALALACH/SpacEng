include "./vendor/premake_customization/solution_items.lua"

workspace "Spaceng"
	architecture "x64"
	targetdir "build"
	startproject "App"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}
	solution_items
	{
		".editorconfig"
	}
	filter "language:C++ or language:C"
		architecture "x86_64"
	filter ""


outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
VULKAN_SDK = os.getenv("VULKAN_SDK")

--includeDir init
IncludeDir = {}
IncludeDir["glfw"] = "Spaceng/Dependency/glfw"
IncludeDir["imgui"] = "Spaceng/Dependency/imgui"
IncludeDir["spdlog"] = "Spaceng/Dependency/spdlog"
IncludeDir["VulkanSDK"] = "%{VULKAN_SDK}/Include"
IncludeDir["optick"] = "Spaceng/Dependency/optick"
IncludeDir["Tinygltf"] = "Spaceng/Dependency/Tinygltf"
IncludeDir["glm"] = "Spaceng/Dependency/glm"
IncludeDir["asio"] = "Spaceng/Dependency/asio/include"


LibraryDir = {}
LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}
Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

group "Dependencies"
include "Spaceng/Dependency/glfw"
include "Spaceng/Dependency/imgui"
--requires premake file
--include "Spaceng/Dependency/Tinygltf" (corrupted Premake)
--include "Spaceng/Dependency/spdlog"
--include "Spaceng/Dependency/optick"
--include "Spaceng/Dependency/glm"

group "Engine"
project "Spaceng"
	location "Spaceng"
	kind "StaticLib"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "PCH.h"
	pchsource "Spaceng/src/PCH.cpp"

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/Dependency",
		"%{IncludeDir.glfw}",
		"%{IncludeDir.glm}",
		"%{IncludeDir.imgui}",
		"%{IncludeDir.spdlog}",
		"%{IncludeDir.optick}",
		"%{IncludeDir.asio}",
		"%{IncludeDir.Tinygltf}",
		"%{IncludeDir.ktx}",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.VulkanSDK}",
		"%{IncludeDir.VulkanSDK_LocalInclude}",
	}

	files 
	{ 
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp"
	}

	links 
	{ 
		"glfw",
		"imgui",
		"%{Library.Vulkan}",
		"%{Library.VulkanUtils}",
	}

	filter "system:windows"
	systemversion "latest"
	
	defines 
	{ 
		"_NO_DEBUG_HEAP=1",
		"SE_PLATFORM_WIN",
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "On"
		links
		{
	
		}
				
	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "On"
		links
		{
		
		}

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "On"





group "Client"
project "App"
	location "App"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	staticruntime "on"
	
	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	links 
	{ 
		"Spaceng"
	}
	
	includedirs
	{
		"%{prj.name}/src",
		"Spaceng/src",
		"Spaceng/Dependency",
		"%{IncludeDir.Vulkan}",
		"%{IncludeDir.VulkanSDK}"
	}
	
	files 
	{ 
		"%{prj.name}/src/**.h", 
		"%{prj.name}/src/**.c", 
		"%{prj.name}/src/**.hpp", 
		"%{prj.name}/src/**.cpp" 
	}

	filter "system:windows"
		systemversion "latest"
				
		defines 
		{ 
			"_NO_DEBUG_HEAP=1",
			"SE_PLATFORM_WIN",
			"_CRT_SECURE_NO_WARNINGS"
		}
	
	filter "configurations:Debug"
		defines "SE_DEBUG"
		symbols "on"

	filter "configurations:Release"
		defines "SE_RELEASE"
		optimize "on"

	filter "configurations:Dist"
		defines "SE_DIST"
		optimize "on"