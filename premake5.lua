workspace "RayTracer"
	architecture "x64"

	configurations
	{
		"Debug",
		"Release"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}"

project "RayTracer"
	location "RayTracer"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++20"
	systemversion "latest"
	staticruntime "On"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/libs/stb_image/**.cpp",
		"%{prj.name}/libs/stb_image/**.h",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/libs",
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	filter "system:windows"

		defines
		{
			"RT_WINDOWS"
		}
	
	filter "system:linux"

		defines
		{
			"RT_LINUX"
		}
	
	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"