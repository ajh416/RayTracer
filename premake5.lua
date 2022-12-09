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

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

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
		"%{prj.name}/src/stb_image/**.cpp",
		"%{prj.name}/src/stb_image/**.h",
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/src/stb_image",
	}

	defines
	{
	}

	filter "configurations:Debug"
		symbols "On"

	filter "configurations:Release"
		optimize "On"