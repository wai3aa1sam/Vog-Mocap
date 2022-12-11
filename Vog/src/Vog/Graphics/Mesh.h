#pragma once
#include "Vog/Core/Core.h"
#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Texture.h"
#include "Vog/Graphics/Buffer.h"
#include "Vog/Graphics/Shader.h"
#include "Vog/Graphics/VertexArray.h"
#include "Vog/Graphics/Material.h"

#include <unordered_map>
#include <map>

// Animation References:
// [LearnOpenGL - Skeletal Animation](https://learnopengl.com/Guest-Articles/2020/Skeletal-Animation)

struct aiNodeAnim;
struct aiScene;
namespace Assimp
{
	class Importer;
}

namespace vog {

	static constexpr int s_max_n_bone_influenece = 4;
	static constexpr int s_max_n_bone = 100;

	struct MeshVertexLayout
	{
		Vector3f position;
		Vector3f normal;
		Vector2f texCoord;
		Vector3f tangent;
	};

	struct MeshAnimatedVertexLayout
	{
		Vector3f position;
		Vector3f normal;
		Vector2f texCoord;
		Vector3f tangent;

		int boneIDs[4];
		float weights[4];

		MeshAnimatedVertexLayout()
		{
			setToDefault();
		}

		void setToDefault()
		{
			for (int i = 0; i < s_max_n_bone_influenece; i++)
			{
				boneIDs[i] = -1;
				weights[i] = 0.0f;
			}
		}

		void addVertexBoneData(int boneID_, float weight_)
		{
			for (int i = 0; i < s_max_n_bone_influenece; ++i)
			{
				if (boneIDs[i] == -1)
				{
					boneIDs[i] = boneID_;
					weights[i] = weight_;
					break;
				}
			}

			//VOG_CORE_LOG_WARN("more than 4 bones in 1 vertex!");
		}
	};

	struct BoneInfo
	{
		/*id is index in finalBoneMatrices*/
		uint32_t id;

		/*offset matrix transforms vertex from model space to bone space*/
		Matrix4f offset;
	};

	struct IndexType
	{ 
		uint32_t index;
		IndexType(uint32_t index_)
			:
			index(index_)
		{

		}
		operator uint32_t() const { return index; }
	};

	struct Transform		// TODO: remove 
	{
		const Matrix4f& getTransformation() const { return transformation; }

		void updateTransformation() { transformation = MyMath::translate(translation) * MyMath::toMatrix4f(Quaternion(rotation)) * MyMath::scale(scale) *
														MyMath::translate(ex_translation) * MyMath::toMatrix4f(Quaternion(ex_rotation)) * MyMath::scale(ex_scale); }

		Matrix4f transformation = Matrix4f(1.0f);

		Vector3f translation = { 0.0f, 0.0f, 0.0f };
		Vector3f rotation = { 0.0f, 0.0f, 0.0f };
		Vector3f scale = { 1.0f, 1.0f, 1.0f };

		Vector3f ex_translation = { 0.0f, 0.0f, 0.0f };
		Vector3f ex_rotation = { 0.0f, 0.0f, 0.0f };
		Vector3f ex_scale = { 1.0f, 1.0f, 1.0f };
	};

	struct AssimpNodeData
	{
		std::string name;
		uint32_t nChildren = 0;
		std::vector<AssimpNodeData> children;

		uint32_t nMeshes = 0;
		uint32_t* pMeshesIndex = nullptr;

		Matrix4f localTransform = Matrix4f(1.0f);

		//--------
		Matrix4f gobalTransform = Matrix4f(1.0f);

		Transform transform;
	};

	struct VOG_API SubMesh
	{
		friend class MeshLoader;
		friend class Model;
		friend class Renderer;
	public:
		std::string name;

		uint32_t baseVertexIndex = 0;
		uint32_t nVertices = 0;

		uint32_t baseIndex = 0;
		uint32_t nIndices = 0;

		uint32_t materialIndex = 0;
	};

	class VOG_API Model : public NonCopyable
	{
		friend class Renderer;
		friend class MeshLoader;
		friend class Animation;

		friend class Scene;
		friend struct MeshComponent;
	public:
		Model(const std::string& filepath_);
		~Model();		// resolve the unique_ptr for Assimp::Importer deleter problem

		void loadMaterial(const RefPtr<Shader> pShader_);

		void onUpdateAnimation(float dt_);

		void onImGuiRender();			// TODO: Replace by ECS draw UI stuff

		const RefPtr<Material>& getMaterial(uint32_t index_ = 0) { VOG_CORE_ASSERT(m_materialPtrs.size() > 0, ""); return m_materialPtrs[index_]; }
		const std::vector<RefPtr<Material>>& getMaterials() { return m_materialPtrs; }

		//inline RefPtr<Shader>& getShader() { return m_pShader; }
		inline const std::vector<SubMesh>& getMeshes() const{ return m_subMeshes; }

		inline AssimpNodeData& getRootNode() { return m_rootNode; }
		inline const AssimpNodeData& getRootNode() const { return m_rootNode; }

		inline const auto& getBoneInfoMap() const { return m_boneInfoMap; }
		inline size_t getBoneCount() const { return m_boneInfoMap.size(); }

		inline const Matrix4f& getInverseMatrix() const { return m_inverseTransform; }

		inline const std::vector<Matrix4f>& getBoneTransforms() const { return m_boneTransforms; }

		inline bool isAnimation() const { return m_isAnimation; }

		inline RefPtr<VertexBuffer>& getpVertexBuffer() { return m_pVertexBuffer; }
		inline RefPtr<IndexBuffer>& getpIndexBuffer() { return m_pIndexBuffer; }

		inline std::string& getName() { return m_name; }

		//void dumpBoneMapping() const
		//{
		//	for (auto& bone : m_boneMapping)
		//	{
		//		VOG_CORE_LOG_INFO("boneMapping, name: {0}, index: {1}", bone.first, bone.second);
		//	}
		//}

		void dumpBoneInfoMap() const
		{
			for (auto& bone : m_boneInfoMap)
			{
				VOG_CORE_LOG_INFO("boneInfoMap, name: {0}, index: {1}, \noffset Matrix: {2}\n", bone.first, bone.second.id, glm::to_string(bone.second.offset));
			}
		}

	protected:

		// TODO: move to another class and support multi Animation
		double _getAnimationRatio(double lastTimeStamp_, double nextTimeStamp_, double animationTime_) const;
		const aiNodeAnim* findAiNodeAnimBy(const std::string& name_) const;

		uint32_t _getTranslationIndex(double animationTime_, const aiNodeAnim* pNodeAnim_) const;
		uint32_t _getRotationIndex(double animationTime_, const aiNodeAnim* pNodeAnim_) const;
		uint32_t _getScaleIndex(double animationTime_, const aiNodeAnim* pNodeAnim_) const;

		Vector3f _interpolateTranslation(double animationTime_, const aiNodeAnim* pNodeAnim_) const;
		Quaternion _interpolateRotation(double animationTime_, const aiNodeAnim* pNodeAnim_) const;
		Vector3f _interpolateScale(double animationTime_, const aiNodeAnim* pNodeAnim_) const;

		void _calculateBoneTransform(const AssimpNodeData& node_, const Matrix4f& parentTransform_);
		
		// TODO: Replace by ECS draw UI stuff
		void _drawNode(uint32_t& nodeIndexTracked_, AssimpNodeData& node_);

		void _drawMaterial();

	protected:
		std::string m_name;
		bool m_isAnimation = false;

		//Assimp::Importer* m_pImporter;
		std::unique_ptr<Assimp::Importer> m_pImporter;
		const aiScene* m_pScene = nullptr;

		std::vector<SubMesh> m_subMeshes;
		//std::unordered_map<std::string, uint32_t> m_subMeshMap;

		std::vector<MeshVertexLayout> m_vertices;
		std::vector<MeshAnimatedVertexLayout> m_animatedVertices;
		std::vector<IndexType> m_indices;

		std::vector<RefPtr<Material>> m_materialPtrs;
		std::vector<RefPtr<Texture2D>> m_texturePtrs;

		std::string m_textureFilepath;
		std::unordered_map<std::string, uint32_t> m_textureIndexMap;

		AssimpNodeData m_rootNode;

		Matrix4f m_inverseTransform;

		std::unordered_map<std::string, BoneInfo> m_boneInfoMap; //
		RefPtr<VertexArray> m_pVertexArray;
		RefPtr<VertexBuffer> m_pVertexBuffer;
		RefPtr<IndexBuffer> m_pIndexBuffer;

		//RefPtr<Shader> m_pShader;

		//--------- Animation
		float m_duration = 0.0f;
		float m_ticksPerSecond = 0.0f;
		float m_currentTime = 0.0f;
		float m_deltaTime = 0.0f;

		std::vector<Matrix4f> m_boneTransforms;

		// Temp: ImGui Window
		AssimpNodeData* m_pSelectedNode = nullptr;

	};
}