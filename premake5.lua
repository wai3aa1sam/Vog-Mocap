solution "Vog"
    architecture "x86_64"
    startproject "Vog-Mocap"
    
    configurations
    {
        "Debug",
        "Release",
        "Distribute"
    }

    flags
    {
        "MultiProcessorCompile"
    }

    --rtti ("Off")

outputDir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "dependencies.lua"

group "Dependencies"
    include "Vog/deps/glad"
    include "Vog/deps/ImGui"
group ""

group "Demo_projects"
    include "Vog-Mocap"
group ""

project "Vog"
    location "%{wks.location}/Vog"
    kind "StaticLib"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    --targetdir ("bin/" .. outputDir .. "/VogApp")       -- shd change to postbuildcommands later by using COPDIR
    targetdir ("bin/" .. outputDir .. "/%{prj.name}")
    objdir ("bin/int/" .. outputDir .. "/%{prj.name}")

    pchheader "vogpch.h"
	pchsource "%{prj.name}/src/vogpch.cpp"

    disablewarnings { "4819", }

    files
    {
        "%{wks.location}/%{prj.name}/src/**.h",
		"%{wks.location}/%{prj.name}/src/**.cpp",
		"%{wks.location}/%{prj.name}/deps/stb/include/**.h",
		"%{wks.location}/%{prj.name}/deps/stb/src/**.cpp",
    }

    includedirs
	{
		"%{wks.location}/%{prj.name}/src",
        "%{includeDirs.assimp}",
        "%{includeDirs.glad}",
        "%{includeDirs.glm}",
        "%{includeDirs.ImGui}",
		"%{wks.location}/%{prj.name}/deps/imgui_impl_win32-patch",
        "%{includeDirs.spdlog}",
        "%{includeDirs.stb}",
	}

    links
	{
		"opengl32.lib",
        "glad",
        "ImGui",
	}

    defines
	{
		"_CRT_SECURE_NO_WARNINGS",
		"IMGUI_IMPL_OPENGL_LOADER_CUSTOM",
        "GLM_FORCE_CTOR_INIT",
	}

    filter "system:windows"
        systemversion "latest"

        defines		-- preprocessor marcos
		{
			"VOG_PLATFORM_WINDOWS",
			"VOG_BUILD_DLL",
		}

    filter "configurations:Debug"
        defines "VOG_DEBUG"
        runtime "Debug"
        symbols "on"

	filter "configurations:Release"
		defines "VOG_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Distribute"
		defines "VOG_DISTRIBUTE"
		runtime "Release"
		optimize "on"