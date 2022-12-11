

VULKAN_SDK = os.getenv("VULKAN_SDK")

dirs = {}
dirs["assimp"] = "%{wks.location}/Vog/deps/assimp"
dirs["VulkanSDK"] = "%{VULKAN_SDK}"

includeDirs = {}
includeDirs["assimp"]       = "%{wks.location}/Vog/deps/assimp/include"
includeDirs["glad"]         = "%{wks.location}/Vog/deps/glad/include"
includeDirs["glm"]          = "%{wks.location}/Vog/deps/glm"
includeDirs["ImGui"]        = "%{wks.location}/Vog/deps/ImGui"
includeDirs["spdlog"]       = "%{wks.location}/Vog/deps/spdlog/include"
includeDirs["stb"]          = "%{wks.location}/Vog/deps/stb/include"
includeDirs["VulkanSDK"]    = "%{VULKAN_SDK}/Include"

libraryDirs = {}
libraryDirs["assimp"] = "%{dirs.assimp}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
libraryDirs["assimp_Debug_Lib"] = "%{libraryDirs.assimp}/assimp-vc143-mtd.lib"
libraryDirs["assimp_Release_Lib"] = "%{libraryDirs.assimp}/assimp-vc143-mt.lib"

--libraryDirs["VulkanSDK"] = "%{VULKAN_SDK}/Lib"
--libraryDirs["Vulkan_Lib"] = "%{libraryDirs.VulkanSDK}/vulkan-1.lib"
--libraryDirs["VulkanUtils_Lib"] = "%{libraryDirs.VulkanSDK}/VkLayer_utils.lib"

--libraryDirs["ShaderC_Debug_Lib"] = "%{libraryDirs.VulkanSDK}/shaderc_sharedd.lib"
--libraryDirs["SPIRV_Cross_Debug_Lib"] = "%{libraryDirs.VulkanSDK}/spirv-cross-cored.lib"
--libraryDirs["SPIRV_Cross_GLSL_Debug_Lib"] = "%{libraryDirs.VulkanSDK}/spirv-cross-glsld.lib"
--libraryDirs["SPIRV_Tools_Debug_Lib"] = "%{libraryDirs.VulkanSDK}/SPIRV-Toolsd.lib"

--libraryDirs["ShaderC_Release_Lib"] = "%{libraryDirs.VulkanSDK}/shaderc_shared.lib"
--libraryDirs["SPIRV_Cross_Release_Lib"] = "%{libraryDirs.VulkanSDK}/spirv-cross-core.lib"
--libraryDirs["SPIRV_Cross_GLSL_Release_Lib"] = "%{libraryDirs.VulkanSDK}/spirv-cross-glsl.lib"

binaryDirs = {}
binaryDirs["assimp"] = "%{dirs.assimp}/bin/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
binaryDirs["assimp_Debug_Bin"] = "%{binaryDirs.assimp}/assimp-vc143-mtd.dll"
binaryDirs["assimp_Release_Bin"] = "%{binaryDirs.assimp}/assimp-vc143-mt.dll"

--binaryDirs["VulkanSDK"] = "%{VULKAN_SDK}/Bin"

--binaryDirs["ShaderC_Debug_Bin"]             = "%{binaryDirs.VulkanSDK}/shaderc_sharedd.dll"
--binaryDirs["SPIRV_Cross_Debug_Bin"]         = "%{binaryDirs.VulkanSDK}/spirv-cross-c-sharedd.dll"
--binaryDirs["SPIRV_Cross_GLSL_Debug_Bin"]    = "%{binaryDirs.VulkanSDK}/spirv-cross-glsld.dll"
--binaryDirs["SPIRV_Tools_Debug_Bin"]         = "%{binaryDirs.VulkanSDK}/SPIRV-Tools-sharedd.dll"

--binaryDirs["ShaderC_Release_Bin"]           = "%{binaryDirs.VulkanSDK}/shaderc_shared.dll"
--binaryDirs["SPIRV_Cross_Release_Bin"]       = "%{binaryDirs.VulkanSDK}/spirv-cross-c-shared.dll"
--binaryDirs["SPIRV_Cross_GLSL_Release_Bin"]  = "%{binaryDirs.VulkanSDK}/spirv-cross-glsl.dll"