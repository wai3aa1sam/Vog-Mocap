#include "vogpch.h"
#include "Vog/Core/Core.h"

#include <glad/glad.h>
#include "backends/imgui_impl_opengl3.cpp"

#ifdef imgui_default_docking	// default dokcing branch in ImGui seems has bug on OpenGL3 with win32, imgui window will be black if outside
#include "backends/imgui_impl_win32.cpp"
#else
#include "imgui_impl_win32-patch.cpp"
#endif // 0

