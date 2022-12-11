#include "vogpch.h"
#include "Vog/Resources/AssetManager.h"

namespace vog {
	AssetManager::AssetData* AssetManager::s_pAssetData = new AssetManager::AssetData;

#pragma region Default_Asset
	AssetManager::AssetData::~AssetData()
	{
		destroy();
	}
	void AssetManager::AssetData::init()
	{
		{	// Load Shader
			shaderLibrary.load("assets/shaders/Default.glsl");
			shaderLibrary.load("assets/shaders/texture.glsl");
			shaderLibrary.load("assets/shaders/vertexPos.glsl");
			shaderLibrary.load("assets/shaders/Animation.glsl");
			shaderLibrary.load("assets/shaders/Simple_Model_Loading.glsl");
			shaderLibrary.load("assets/shaders/Skybox.glsl");

			// Debug
			shaderLibrary.load("assets/shaders/Debug/Debug_Line.glsl");
			shaderLibrary.load("assets/shaders/Debug/Debug_Circle.glsl");
			shaderLibrary.load("assets/shaders/Debug/Debug_Postprocess.glsl");

			// Deferred Shader
			shaderLibrary.load("assets/shaders/Deferred_Shader/GeometryPass.glsl");
			shaderLibrary.load("assets/shaders/Deferred_Shader/GeometryPass_Anim.glsl");
			shaderLibrary.load("assets/shaders/Deferred_Shader/Deferred_Lighting.glsl");
			shaderLibrary.load("assets/shaders/Deferred_Shader/Deferred_Postprocess.glsl");
			
			// Features
			shaderLibrary.load("assets/shaders/Blur_Pingpong.glsl");
			shaderLibrary.load("assets/shaders/Shadow_Mapping.glsl");
			shaderLibrary.load("assets/shaders/Shadow_Mapping_Anim.glsl");
			shaderLibrary.load("assets/shaders/Trail.glsl");
		}

		// Load defaule Textures
		pDefaultTexture = Texture2D::create("assets/textures/test.png");
		//pDefaultTexture = Texture2D::create("assets/textures/happymask.png");
		//pDefaultTexture = Texture2D::create("assets/textures/default_texture.png");
		//pDefaultCubemap = Cubemap::create("assets/skybox/sky");
		pDefaultCubemap = Cubemap::create("assets/skybox/space");

		// Load defaule Meshes
		{
			auto& meshPtrs = defaultMeshPtrs;
			auto& pShader = shaderLibrary.getShader("GeometryPass");
			//auto& pShader = shaderLibrary.getShader("Simple_Model_Loading");

			meshPtrs.reserve(static_cast<int>(PrimitiveMesh::Count));
			meshPtrs.emplace_back(createRefPtr<Model>("assets/meshes/primitives/cube.obj"));				//meshPtrs.back()->loadMaterial(pShader);
			meshPtrs.emplace_back(createRefPtr<Model>("assets/meshes/primitives/capsule.obj"));				//meshPtrs.back()->loadMaterial(pShader);
			meshPtrs.emplace_back(createRefPtr<Model>("assets/meshes/primitives/plane.obj"));				//meshPtrs.back()->loadMaterial(pShader);
			meshPtrs.emplace_back(createRefPtr<Model>("assets/meshes/primitives/sphere.obj"));				//meshPtrs.back()->loadMaterial(pShader);

			meshPtrs.emplace_back(createRefPtr<Model>("assets/meshes/primitives/postprocessQuad.obj"));		//meshPtrs.back()->loadMaterial(pShader);
		}

		// Load default Material
		{
			auto& materialPtrs = defaultMaterialPtrs;

			materialPtrs.reserve(10);
			//materialPtrs.emplace_back(createRefPtr<Material>());
		}
	}
	void AssetManager::AssetData::destroy()
	{
		pDefaultTexture.reset();
		defaultMeshPtrs.clear();
		defaultMaterialPtrs.clear();
	}
#pragma endregion

	void AssetManager::init()
	{
		s_pAssetData->init();
	}

	void AssetManager::shuntdown()
	{
		delete s_pAssetData;
		s_pAssetData = nullptr;
	}

	RefPtr<Model>& AssetManager::loadMesh(const std::string& filepath_)
	{
		size_t index = s_pAssetData->meshPtrs.size();
		s_pAssetData->meshPtrs.emplace_back(createRefPtr<Model>(filepath_));
		s_pAssetData->meshIndexMap[s_pAssetData->meshPtrs.back()->getName()] = index;
		return s_pAssetData->meshPtrs.back();
	}

	RefPtr<Shader>& AssetManager::loadShader(const std::string& filepath_) { return s_pAssetData->shaderLibrary.load(filepath_); }

	RefPtr<Shader>& AssetManager::getRendererShader() { return getShader("GeometryPass"); }
	RefPtr<Shader>& AssetManager::getRendererAnimatedShader() { return getShader("GeometryPass_Anim"); }

	void AssetManager::reloadShaders() { s_pAssetData->shaderLibrary.reload(); }

	void AssetManager::endScene() 	{ s_pAssetData->shaderLibrary.endScene(); }

	bool AssetManager::isAnimatedShader(const RefPtr<Shader>& pShader_) { return (pShader_ == getShader("GeometryPass_Anim") || pShader_ == getShader("Animation")); }
}