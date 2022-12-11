#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Mesh.h"
#include "Vog/Graphics/Texture.h"

namespace vog {

	enum class PrimitiveMesh
	{
		Box = 0,
		Capsule,
		Plane,
		Sphere,




		PostprocessQuad,
		Count
	};

	class VOG_API AssetManager : public NonCopyable
	{
	public:
		static void init();
		static void shuntdown();

		static RefPtr<Model>& loadMesh(const std::string& filepath_);
		static RefPtr<Model>& getLoadedMesh(const std::string& name_);

		static RefPtr<Shader>& loadShader(const std::string& filepath_);
		static RefPtr<Shader>& getRendererShader();
		static RefPtr<Shader>& getRendererAnimatedShader();

		static RefPtr<Shader>& getShader(const std::string& name_);

		static const RefPtr<Texture2D>& getDefaultTexture();
		static const RefPtr<Cubemap>& getDefaultCubemap();
		static const RefPtr<Model>& getMesh(PrimitiveMesh primitive_);

		static void reloadShaders();
		static void endScene();

		static bool isAnimatedShader(const RefPtr<Shader>& pShader_);

		template<typename Func>
		static void foreach_shader(Func func_);

		template<typename Func>
		static void foreach_mesh(Func func_);

	private:
		struct AssetData
		{
			RefPtr<Texture2D> pDefaultTexture;
			RefPtr<Cubemap> pDefaultCubemap;
			std::vector<RefPtr<Model>> defaultMeshPtrs;
			std::vector<RefPtr<Material>> defaultMaterialPtrs;

			std::vector<RefPtr<Model>> meshPtrs;
			std::unordered_map<std::string, size_t> meshIndexMap;

			ShaderLibrary shaderLibrary;

			~AssetData();
			void init();
			void destroy();
		};
		static AssetData* s_pAssetData;
	};

	inline RefPtr<Model>& AssetManager::getLoadedMesh(const std::string& name_) { return s_pAssetData->meshPtrs[s_pAssetData->meshIndexMap.at(name_)]; }

	inline const RefPtr<Cubemap>&	AssetManager::getDefaultCubemap() { return s_pAssetData->pDefaultCubemap; }
	inline const RefPtr<Texture2D>& AssetManager::getDefaultTexture() { return s_pAssetData->pDefaultTexture; }
	inline const RefPtr<Model>&		AssetManager::getMesh(PrimitiveMesh primitive_) { return s_pAssetData->defaultMeshPtrs[static_cast<int>(primitive_)]; }
	inline RefPtr<Shader>&			AssetManager::getShader(const std::string& name_) { return s_pAssetData->shaderLibrary.getShader(name_); }

	template<typename Func>
	inline void AssetManager::foreach_shader(Func func_)
	{
		for (auto& element : s_pAssetData->shaderLibrary.m_shaderMap)
		{
			if (element.second->getID() == AssetManager::getShader("Skybox")->getID())		// TODO: rectify
				continue;

			func_(element.second);
		}
	}
	template<typename Func>
	inline void AssetManager::foreach_mesh(Func func_)
	{
		for (size_t i = 0; i < s_pAssetData->defaultMeshPtrs.size(); i++)
		{
			func_(s_pAssetData->defaultMeshPtrs[i]);
		}
	}
}