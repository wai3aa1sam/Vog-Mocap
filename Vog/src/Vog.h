#pragma once
#include "vogpch.h"

#include "Vog/Core/Application.h"
#include "Vog/Core/Log.h"
#include "Vog/Core/Layer.h"
#include "Vog/Core/Timer.h"

#pragma region Input_And_KeyCodes
#include "Vog/Core/Input.h"
#include "Vog/Core/KeyCodes.h"
#pragma endregion

#pragma region Animation
//#include "Vog/Animation/Animation.h"
//#include "Vog/Animation/Animator.h"
#pragma endregion

#pragma region Asset
#include "Vog/Resources/AssetManager.h"
#pragma endregion


#pragma region Debug
#include "Vog/Debug/BenchmarkTimer.h"
#include "Vog/Debug/RendererDebug.h"
#include "Vog/Debug/Gizmos.h"
#pragma endregion

#pragma region Objects
#include "Vog/Objects/Camera.h"
#include "Vog/Objects/CameraController.h"
#pragma endregion

#pragma region Renderer
#include "Vog/Graphics/Buffer.h"
#include "Vog/Graphics/Framebuffer.h"
#include "Vog/Graphics/Mesh.h"
#include "Vog/Graphics/RenderCommand.h"
#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/Texture.h"
#include "Vog/Graphics/VertexArray.h"

#include "Vog/Graphics/Renderer/Renderer.h"
#include "Vog/Graphics/Renderer/DeferredRenderer.h"
#pragma endregion

#pragma region Scene
#include "Vog/Scene/Components.h"
#include "Vog/Scene/Entity.h"
#include "Vog/Scene/NativeScriptEntity.h"
#include "Vog/Scene/Scene.h"
#pragma endregion

#pragma region Math
#include "Vog/Math/MyMath.h"
#include "Vog/Math/MyRandom.h"
#pragma endregion

#pragma region Utilities
#include "Vog/Utilities/MyCommon.h"
#pragma endregion

#pragma region StartupEntry
//#include "Vog/Core/StartupEntry.h"
#pragma endregion
