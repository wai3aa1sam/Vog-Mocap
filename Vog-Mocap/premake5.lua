include "dependencies.lua"

project "Vog-Mocap"
    location "%{wks.location}/Vog-Mocap"
    kind "ConsoleApp"
    language "C++"
    cppdialect "C++17"
    staticruntime "on"

    targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/int/" .. outputDir .. "/%{prj.name}")

    files
	{
		"src/**.h",
		"src/**.cpp",
        
        "%{wks.location}/%{prj.name}/deps/JointSmooth/**.h",
        "%{wks.location}/%{prj.name}/deps/JointSmooth/**.cpp",

        "%{wks.location}/%{prj.name}/deps/SerialPort/src/**.h",
        "%{wks.location}/%{prj.name}/deps/SerialPort/src/**.cpp",
	}

    includedirs
	{
		"%{wks.location}/Vog/src",
		"%{wks.location}/Vog/deps",
        "%{includeDirs.spdlog}",
        "%{includeDirs.glm}",

        "%{wks.location}/%{prj.name}/deps/SerialPort/src",

        "%{wks.location}/%{prj.name}/deps/JointSmooth",
        --"$(KINECTSDK20_DIR)/inc",
	}

    links
	{
		"Vog",
        --"$(KINECTSDK20_DIR)/Lib/x64/kinect20.lib",
	}

    filter "system:windows"
        systemversion "latest"

        defines
        {
            "VOG_PLATFORM_WINDOWS",
            "GLM_FORCE_CTOR_INIT",
            --"VOG_ENABLE_KINECT",
        }

        postbuildcommands 
		{
            '{COPY} "%{wks.location}/%{prj.name}/imgui.ini" "%{cfg.targetdir}"',
		}

    filter "configurations:Debug"
        defines "VOG_DEBUG"
        runtime "Debug"
        symbols "on"

        links
        {
            "%{libraryDirs.assimp_Debug_Lib}",
        }

        postbuildcommands 
		{
            '{COPY} "%{binaryDirs.assimp_Debug_Bin}" "%{cfg.targetdir}"',
		}

    filter "configurations:Release"
        defines "VOG_RELEASE"
        runtime "Release"
        optimize "on"

        links
        {
            "%{libraryDirs.assimp_Release_Lib}",
        }

        postbuildcommands 
		{
            '{COPY} "%{binaryDirs.assimp_Release_Bin}" "%{cfg.targetdir}"',
		}

    filter "configurations:Distribute"
        defines "VOG_DISTRIBUTE"
        runtime "Release"
        optimize "on"
        
        links
        {
            "%{libraryDirs.assimp_Release_Lib}",

        }

        postbuildcommands 
		{
            '{COPY} "%{binaryDirs.assimp_Release_Bin}" "%{cfg.targetdir}"',
		}