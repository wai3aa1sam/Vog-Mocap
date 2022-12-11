#include "vogpch.h"
#include "Vog/Graphics/Mesh.h"
#include "Vog/Math/MyMath.h"

#include "Vog/Core/Log.h"
#include "Vog/ImGui/ImGuiLibrary.h"

#include <imgui.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/matrix4x4.h>
#include <assimp/vector2.h>
#include <assimp/quaternion.h>

#include <filesystem>
#include <iostream>
#include <fstream>

//#define MESH_DEBUG

namespace vog {

	//struct MaterialData
	//{
	//	std::vector<RefPtr<Material>> materialPtrs;
	//	std::vector<RefPtr<Texture2D>> texturePtrs;
	//};
	
#ifdef MESH_DEBUG
	static std::ofstream s_myfile;
#endif // MESH_DEBUG

#if 0
	struct Converter
	{
		Matrix4f mat4 = Matrix4f(1.0f);

		void transformVertex(MeshAnimatedVertexLayout& vertex_)
		{
			vertex_.position = mat4 * Vector4f(vertex_.position, 1.0f);
			vertex_.normal = mat4 * Vector4f(vertex_.normal, 0.0f);
			vertex_.tangent = mat4 * Vector4f(vertex_.tangent, 0.0f);
		}
	};

	static Converter converter;
#endif // 0

	namespace Utils
	{
		static TextureType getTextureType(aiTextureType type_)
		{
			switch (type_)
				{
				case aiTextureType_DIFFUSE:				return TextureType::Albedo;
				case aiTextureType_SPECULAR:			return TextureType::Specular;
				//case aiTextureType_AMBIENT:				return TextureType::Albedo;
				//case aiTextureType_EMISSIVE:			return TextureType::Albedo;
				case aiTextureType_HEIGHT:				return TextureType::Normal;		// aiTextureType_NORMALS --- map_Kn ()		aiTextureType_HEIGHT --- map_Bump (blender)
				//case aiTextureType_NORMALS:				return TextureType::Albedo;
				//case aiTextureType_SHININESS:			return TextureType::Albedo;
				//case aiTextureType_OPACITY:				return TextureType::Albedo;
				//case aiTextureType_DISPLACEMENT:		return TextureType::Albedo;
				//case aiTextureType_LIGHTMAP:			return TextureType::Albedo;
				//case aiTextureType_REFLECTION:			return TextureType::Albedo;
				//case aiTextureType_BASE_COLOR:			return TextureType::Albedo;
				//case aiTextureType_NORMAL_CAMERA:		return TextureType::Albedo;
				//case aiTextureType_EMISSION_COLOR:		return TextureType::Albedo;
				//case aiTextureType_METALNESS:			return TextureType::Albedo;
				//case aiTextureType_DIFFUSE_ROUGHNESS:	return TextureType::Albedo;
				//case aiTextureType_AMBIENT_OCCLUSION:	return TextureType::Albedo;
				//case aiTextureType_SHEEN:				return TextureType::Albedo;
				//case aiTextureType_CLEARCOAT:			return TextureType::Albedo;
				//case aiTextureType_TRANSMISSION:		return TextureType::Albedo;
				//case aiTextureType_UNKNOWN:				return TextureType::Albedo;
				//case _aiTextureType_Force32Bit:			return TextureType::Albedo;
			}
			VOG_CORE_ASSERT(0, "");
			return TextureType::None;
		}
	}

#pragma region assimp_stuff_utils
	static Vector2f toVec2fFrom(const aiVector2D& aiVec2f_) { return Vector2f{ aiVec2f_.x, aiVec2f_.y }; }
	static Vector3f toVec3fFrom(const aiVector3D& aiVec3f_) { return Vector3f{ aiVec3f_.x, aiVec3f_.y, aiVec3f_.z }; }
	static Matrix4f toMat4fFrom(const aiMatrix4x4& aiMat4_)
	{
		Matrix4f to;
		to[0][0] = aiMat4_.a1; to[0][1] = aiMat4_.b1;  to[0][2] = aiMat4_.c1; to[0][3] = aiMat4_.d1;
		to[1][0] = aiMat4_.a2; to[1][1] = aiMat4_.b2;  to[1][2] = aiMat4_.c2; to[1][3] = aiMat4_.d2;
		to[2][0] = aiMat4_.a3; to[2][1] = aiMat4_.b3;  to[2][2] = aiMat4_.c3; to[2][3] = aiMat4_.d3;
		to[3][0] = aiMat4_.a4; to[3][1] = aiMat4_.b4;  to[3][2] = aiMat4_.c4; to[3][3] = aiMat4_.d4;
		return to;
	};
	static Quaternion toQuatFrom(const aiQuaternion& aiQuat_) { return { aiQuat_.w, aiQuat_.x, aiQuat_.y, aiQuat_.z }; }
#pragma endregion


#pragma region MeshLoader
	class VOG_API MeshLoader : public NonCopyable
	{
	public:
		bool loadFile(const std::string& filepath_, Model& model_)
		{
			//auto* pImporter = new Assimp::Importer();
			auto pImporter = std::make_unique<Assimp::Importer>();


			const aiScene* pScene = pImporter->ReadFile(filepath_, aiProcess_Triangulate | aiProcess_SortByPType |	// aiProcess_FlipUVs |
																	 aiProcess_GenUVCoords | aiProcess_GenNormals | aiProcess_CalcTangentSpace);

			if (!pScene || pScene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pScene->mRootNode)
			{
				VOG_CORE_LOG_ERROR("ERROR::ASSIMP::{0}", pImporter->GetErrorString());
				return false;
			}
			
			// texture path is same for model path
			std::string fileDir = std::filesystem::path(filepath_).parent_path().string();
			uint64_t fileDirSize = fileDir.size();
			std::string_view textureDirToken = "/";					// ${fileDir} '+' / <--- textureDirToken
			m_textureFilepath.reserve(fileDirSize + textureDirToken.size() + 1);
			m_textureFilepath.append(fileDir);
			m_textureFilepath.append(textureDirToken);

#if add_textures_token_to_texture_path
			aiString aiTexFilename;
			if (pScene->HasMaterials())
			{
				//pScene->mMaterials[0]->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexFilename) == aiReturn_SUCCESS
					std::string texFilename{ aiTexFilename.C_Str() };
				auto ret = texFilename.find("textures", 0);
				if (ret == std::string::npos)
				{
					std::string fileDir = std::filesystem::path(filepath_).parent_path().string();
					uint64_t fileDirSize = fileDir.size();
					std::string_view textureDirToken = "/textures/";					//${fileDir}/textures/

					m_textureFilepath.reserve(fileDirSize + textureDirToken.size() + 1);
					m_textureFilepath.append(fileDir);
					m_textureFilepath.append(textureDirToken);
				}
				else
				{
					m_textureFilepath = filepath_;
				}
			}
#endif // add_textures_token_to_texture_path

#pragma region process_meshes

			std::string name = std::filesystem::path(filepath_).stem().string();

			bool isAnimated = pScene->HasAnimations();

			uint32_t nMeshes = pScene->mNumMeshes;
			std::vector<SubMesh> subMeshes;
			//std::unordered_map<std::string, uint32_t> subMeshMap;

			std::vector<MeshVertexLayout> vertices;
			std::vector<MeshAnimatedVertexLayout> animatedVertices;
			std::vector<IndexType> indices;

			uint32_t startBaseVertexIndex = 0;
			uint32_t startBaseIndex = 0;

			uint32_t totalNVertices = 0;
			uint32_t totalNIndies = 0;

			std::unordered_map<std::string, BoneInfo> boneInfoMap;
			uint32_t boneCounter = 0;

			subMeshes.reserve(nMeshes);
			//subMeshMap.reserve(nMeshes);
			for (uint32_t i = 0; i < nMeshes; i++)
			{
				const aiMesh* pAiMesh = pScene->mMeshes[i];

				const uint32_t nVertices = pAiMesh->mNumVertices;
				const uint32_t nIndices = pAiMesh->mNumFaces * 3;	// triangle only

				subMeshes.emplace_back();
				auto& mesh = subMeshes[i];

				mesh.name = pAiMesh->mName.C_Str();

				mesh.baseVertexIndex = startBaseVertexIndex;
				mesh.nVertices = nVertices;

				mesh.baseIndex = startBaseIndex;
				mesh.nIndices = nIndices;

				mesh.materialIndex = pAiMesh->mMaterialIndex;

				//subMeshMap[mesh.name] = i;

				startBaseVertexIndex += nVertices;
				startBaseIndex += nIndices;
			}

			totalNVertices = startBaseVertexIndex;
			totalNIndies = startBaseIndex;

			// prepare to load vertex and index data
			if (isAnimated)
				animatedVertices.reserve(totalNVertices);
			else
				vertices.reserve(totalNVertices);

			indices.reserve(totalNIndies);

			// load vertices
			for (uint32_t iMeshes = 0; iMeshes < nMeshes; iMeshes++)
			{
				const aiMesh* pAiMesh = pScene->mMeshes[iMeshes];
				const uint32_t nVertices = pAiMesh->mNumVertices;
				const uint32_t nFaces = pAiMesh->mNumFaces;

				auto& mesh = subMeshes[iMeshes];

				mesh.materialIndex = pAiMesh->mMaterialIndex;			// sub model material index

				//VOG_LOG_INFO("mesh index: {0}, material index: {1}", iMeshes, mesh.m_materialIndex);

				if (isAnimated)
				{
					for (uint32_t iVertex = 0; iVertex < nVertices; iVertex++)
					{
						MeshAnimatedVertexLayout animatedvertexLayout;

						animatedvertexLayout.position = toVec3fFrom(pAiMesh->mVertices[iVertex]);
						if (pAiMesh->HasNormals())
							animatedvertexLayout.normal = toVec3fFrom(pAiMesh->mNormals[iVertex]);

						if (pAiMesh->HasTextureCoords(0))
							animatedvertexLayout.texCoord = { pAiMesh->mTextureCoords[0][iVertex].x, pAiMesh->mTextureCoords[0][iVertex].y };

						if (pAiMesh->HasTangentsAndBitangents())
						{
							animatedvertexLayout.tangent = toVec3fFrom(pAiMesh->mTangents[iVertex]);
							//vertex.bitangent = toAssimpVec3f(pAiMesh->mBitangents[i]);
						}

						animatedVertices.emplace_back(animatedvertexLayout);
					}

					// load bone data for mesh
					//auto& boneMapping = model_.m_boneMapping;
					
					for (uint32_t boneIndex = 0; boneIndex < pAiMesh->mNumBones; ++boneIndex)
					{
						int boneID = -1;
						std::string boneName = pAiMesh->mBones[boneIndex]->mName.C_Str();
						
						if (boneInfoMap.find(boneName) == boneInfoMap.end())
						{
							BoneInfo newBoneInfo;
							newBoneInfo.id = boneCounter;
							newBoneInfo.offset = toMat4fFrom(pAiMesh->mBones[boneIndex]->mOffsetMatrix);
							boneInfoMap[boneName] = newBoneInfo;
							boneID = boneCounter;

							boneCounter++;
						}
						else
						{
							boneID = boneInfoMap.at(boneName).id;
						}

						VOG_CORE_ASSERT(boneID != -1, "");
						const aiBone* pBone = pAiMesh->mBones[boneIndex];
						const auto& weights = pBone->mWeights;
						const uint32_t numWeights = pBone->mNumWeights;

						for (uint32_t weightIndex = 0; weightIndex < numWeights; ++weightIndex)
						{
							uint32_t vertexId = mesh.baseVertexIndex + weights[weightIndex].mVertexId;
							float weight = weights[weightIndex].mWeight;
							VOG_CORE_ASSERT(vertexId <= animatedVertices.size(), "Invalid vertex id");
							animatedVertices[vertexId].addVertexBoneData(boneID, weight);
						}
					}
				}
				else
				{
					for (uint32_t iVertex = 0; iVertex < nVertices; iVertex++)
					{
						MeshVertexLayout vertexLayout;

						vertexLayout.position = toVec3fFrom(pAiMesh->mVertices[iVertex]);
						if (pAiMesh->HasNormals())
							vertexLayout.normal = toVec3fFrom(pAiMesh->mNormals[iVertex]);

						if (pAiMesh->HasTextureCoords(0))
						{
							vertexLayout.texCoord = { pAiMesh->mTextureCoords[0][iVertex].x, pAiMesh->mTextureCoords[0][iVertex].y };
						}

						if (pAiMesh->HasTangentsAndBitangents())
						{
							vertexLayout.tangent = toVec3fFrom(pAiMesh->mTangents[iVertex]);
							//vertex.bitangent = toAssimpVec3f(pAiMesh->mBitangents[iVertex]);
						}
						vertices.emplace_back(vertexLayout);
					}
				}

				//IndexType indexType;
				for (uint32_t iIndex = 0; iIndex < nFaces; iIndex++)
				{
					const aiFace* face = &pAiMesh->mFaces[iIndex];
					if (face->mNumIndices != 3)
					{
						VOG_CORE_LOG_WARN("the face indics is not 3");
					}
					indices.emplace_back(face->mIndices[0]);
					indices.emplace_back(face->mIndices[1]);
					indices.emplace_back(face->mIndices[2]);
				}
			}
#pragma endregion

			// Animation

			// Animation end

			model_.m_name = std::move(name);
			model_.m_subMeshes = subMeshes;
			//model_.m_subMeshMap = subMeshMap;

			model_.m_inverseTransform = MyMath::inverse(toMat4fFrom(pScene->mRootNode->mTransformation));

			model_.m_isAnimation = isAnimated;
			model_.m_animatedVertices = animatedVertices;
			model_.m_vertices = vertices;
			model_.m_boneInfoMap = boneInfoMap;

			model_.m_indices = indices;

			model_.m_textureFilepath = m_textureFilepath;

			//model_.m_texturePtrs = texturePtrs;
			//model_.m_materialPtrs = materialPtrs;

			//model_.m_textureIndexMap = textureIndexMap;

			model_.m_pImporter.reset(pImporter.release());
			model_.m_pScene = pScene;

			_readHeirarchyData(model_.m_rootNode, pScene->mRootNode, Matrix4f(1.0f), 0);

			return true;
		}

	private:
		[[deprecated]]RefPtr<SubMesh> _processModelMesh(bool isAnimated_, uint32_t startBaseVertexIndex_, const aiMesh* pAiMesh_, const aiScene* pScene_)
		{
			// Load Mesh Vertices
			const uint32_t nVertices = pAiMesh_->mNumVertices;
			const uint32_t nIndices = pAiMesh_->mNumFaces;

			const uint32_t baseVertexIndex = startBaseVertexIndex_ + nVertices;

#ifdef MESH_DEBUG
#ifdef MESH_DEBUG
			s_myfile.open("mesh_debug.txt");
#endif // MESH_DEBUG
			s_myfile.close();
#endif // MESH_DEBUG
#ifdef MESH_DEBUG
			s_myfile << index << "---" << i << ".\tposition: { " << vertexLayout.position.x << ", " << vertexLayout.position.y << ", " << vertexLayout.position.z << " }\n";
			s_myfile << index << "---" << i << ".\tnormal: { " << vertexLayout.normal.x << ", " << vertexLayout.normal.y << ", " << vertexLayout.normal.z << " }\n";
			s_myfile << index << "---" << i << ".\tuv: { " << vertexLayout.texCoord.x << ", " << vertexLayout.texCoord.y << " }\n";
			s_myfile << index << "---" << i << ".\ttangent: { " << vertexLayout.tangent.x << ", " << vertexLayout.tangent.y << ", " << vertexLayout.tangent.z << " }\n";
#endif // DEBUG

#ifdef MESH_DEBUG
			s_myfile.flush();
			s_myfile << "\n\n\n\n\n\n\n\n\n";
#endif // DEBUG

#pragma region old_Load_Mesh_Materials

			// Load Mesh Materials

			//aiString aiTexFilename;
			//std::string texFilename;
			//texFilename.reserve(30);
			//const auto materialIndex = pAiMesh_->mMaterialIndex;

			//std::vector<RefPtr<Texture2D>> pTextures;
			//auto& textureMap = model_.m_textureMap;
			//if (materialIndex >= 0)
			//{
			//	auto& material = *pScene_->mMaterials[materialIndex];
			//	//if (AI_SUCCESS != material.Get(AI_MATKEY_SHININESS, myMaterial.shininess))
			//		//myMaterial.shininess = 20.0f;

			//	if (material.GetTexture(aiTextureType_DIFFUSE, 0, &aiTexFilename) == aiReturn_SUCCESS)		// Albedo map
			//	{
			//		texFilename = aiTexFilename.C_Str();
			//		// if not find in map
			//		if (textureMap.find(texFilename) == textureMap.end())
			//			textureMap[texFilename] = Texture2D::create(m_textureFilepath + texFilename);
			//		else
			//			pTextures.emplace_back(textureMap[texFilename]);

			//		//myMaterial.hasDiffuseMap = true;
			//		//std::cout << "Loaded mesh " << mesh->mName.C_Str() << " txture, path: " << mDirmDir << std::endl;
			//	}
			//	else
			//	{
			//		//if (AI_SUCCESS != material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(myMaterial.diffuseColor)))
			//		//	myMaterial.diffuseColor = MyVec3f(1.0f, 1.0f, 1.0f);
			//	}

			//	if (material.GetTexture(aiTextureType_SPECULAR, 0, &aiTexFilename) == aiReturn_SUCCESS)		// Specular map
			//	{
			//		texFilename = aiTexFilename.C_Str();
			//		// if not find in map
			//		if (textureMap.find(texFilename) == textureMap.end())
			//			textureMap[texFilename] = Texture2D::create(m_textureFilepath + texFilename);
			//		else
			//			pTextures.emplace_back(textureMap[texFilename]);

			//		//myMaterial.hasDiffuseMap = true;
			//		//std::cout << "Loaded mesh " << mesh->mName.C_Str() << " txture, path: " << mDirmDir << std::endl;
			//	}
			//	else
			//	{
			//		//if (AI_SUCCESS != material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(myMaterial.specularColor)))
			//		//	myMaterial.specularColor = MyVec3f(1.0f, 1.0f, 1.0f);
			//	}

			//	// aiTextureType_NORMALS --- map_Kn ()		aiTextureType_HEIGHT --- map_Bump (blender)
			//	if (material.GetTexture(aiTextureType_HEIGHT, 0, &aiTexFilename) == aiReturn_SUCCESS)		// normal map
			//	{
			//		texFilename = aiTexFilename.C_Str();
			//		// if not find in map
			//		if (textureMap.find(texFilename) == textureMap.end())
			//			textureMap[texFilename] = Texture2D::create(m_textureFilepath + texFilename);
			//		else
			//			pTextures.emplace_back(textureMap[texFilename]);

			//		//myMaterial.hasDiffuseMap = true;
			//		//std::cout << "Loaded mesh " << mesh->mName.C_Str() << " txture, path: " << mDirmDir << std::endl;
			//	}

			//}

			//// add back to the meshes ptr
			//if (isAnimated)
			//	return createRefPtr<ModelMesh>(animatedVertices, indices, pTextures);
			//else
			//	return createRefPtr<ModelMesh>(vertices, indices, pTextures);
#pragma endregion
		}

		void _readHeirarchyData(AssimpNodeData& dest_, const aiNode* pParentNode_, const Matrix4f& parentMatrix_, int level_ = 0)
		{
			dest_.name = pParentNode_->mName.C_Str();
			dest_.nChildren = pParentNode_->mNumChildren;
			dest_.children.reserve(dest_.nChildren);

			dest_.nMeshes = pParentNode_->mNumMeshes;
			dest_.pMeshesIndex = pParentNode_->mMeshes;

			dest_.localTransform = toMat4fFrom(pParentNode_->mTransformation);
			dest_.gobalTransform = parentMatrix_ * dest_.localTransform;

			//VOG_CORE_LOG_TRACE("{0} level: {1}, node name: {2}", IndentationLevel(level_), level_, dest_.name);
			//VOG_CORE_LOG_TRACE("level: {0}, node name: {1}, childern Count: {2}, \nlocal transform: {3}\n", level_, dest_.name, pParentNode_->mNumChildren, glm::to_string(dest_.localTransform));

			for (uint32_t i = 0; i < pParentNode_->mNumChildren; ++i)
			{
				AssimpNodeData node;
				_readHeirarchyData(node, pParentNode_->mChildren[i], dest_.gobalTransform, level_ + 1);
				dest_.children.emplace_back(node);
			}
		}

	private:
		std::string m_textureFilepath;
	};
#pragma endregion

	Model::Model(const std::string& filepath_)
		:
		m_name(std::filesystem::path(filepath_).stem().string())
	{
		MeshLoader meshLoader;
		meshLoader.loadFile(filepath_, *this);

		if (m_isAnimation)
		{
			// animation bone
			VOG_CORE_ASSERT(m_boneInfoMap.size() > 0, "Size of boneMap is 0!");

			m_boneTransforms.resize(m_boneInfoMap.size());

			VOG_CORE_ASSERT(m_animatedVertices.size() > 0, "Size of mesh vertices is 0!");
			VOG_CORE_ASSERT(m_indices.size() > 0, "Size of mesh vertices is 0!");

			m_pVertexBuffer = VertexBuffer::create(m_animatedVertices.data(), static_cast<uint32_t>(sizeof(m_animatedVertices[0]) * m_animatedVertices.size()));
			VertexLayout layout = {
				{VertexDataType::Float3, "a_position",	},
				{VertexDataType::Float3, "a_normal",	},
				{VertexDataType::Float2, "a_texCoord",	},
				{VertexDataType::Float3, "a_tangent",	},
				{VertexDataType::Int4,   "a_boneIDs",	},
				{VertexDataType::Float4, "a_weights",	},
			};
			m_pVertexBuffer->setVertexLayout(layout);

			m_pIndexBuffer = IndexBuffer::create((uint32_t*)m_indices.data(), static_cast<uint32_t>(m_indices.size()));

			m_pVertexArray = VertexArray::create(m_pVertexBuffer, m_pIndexBuffer);

			//m_pShader = Shader::create("assets/shaders/Animation.glsl");
		}
		else
		{
			VOG_CORE_ASSERT(m_vertices.size() > 0, "Size of mesh vertices is 0!");
			VOG_CORE_ASSERT(m_indices.size() > 0, "Size of mesh vertices is 0!");

			m_pVertexBuffer = VertexBuffer::create(m_vertices.data(), static_cast<uint32_t>(sizeof(m_vertices[0]) * m_vertices.size()));
			VertexLayout layout = {
				{VertexDataType::Float3, "a_position",	},
				{VertexDataType::Float3, "a_normal",	},
				{VertexDataType::Float2, "a_texCoord",	},
				{VertexDataType::Float3, "a_tangent",	},
			};
			m_pVertexBuffer->setVertexLayout(layout);

			m_pIndexBuffer = IndexBuffer::create((uint32_t*)m_indices.data(), static_cast<uint32_t>(m_indices.size()));

			m_pVertexArray = VertexArray::create(m_pVertexBuffer, m_pIndexBuffer);

			//m_pShader = Shader::create("assets/shaders/Simple_Model_Loading.glsl");
		}
	}

	Model::~Model()
	{

	}

	void Model::loadMaterial(const RefPtr<Shader> pShader_)
	{
		VOG_CORE_ASSERT(m_pScene, "");
		auto& pScene = m_pScene;

		std::vector<RefPtr<Texture2D>> texturePtrs;
		std::vector<RefPtr<Material>> materialPtrs;

		// local cache map to find textures by name
		std::unordered_map<std::string, uint32_t> textureIndexMap;

		// load textures only materials
		if (m_pScene->HasMaterials())
		{
			const uint32_t nMaterials = pScene->mNumMaterials;
			const uint32_t nTextures = pScene->mNumTextures;
			aiString aiTexFilename;
			std::string texFilename;
			texFilename.reserve(40);

			textureIndexMap.reserve(nTextures);

			uint32_t textureCounter = 0;

			materialPtrs.reserve(nMaterials);
			texturePtrs.reserve(nTextures);

			std::array<aiTextureType, 3> aiTextureTypes = { aiTextureType_DIFFUSE, aiTextureType_HEIGHT, aiTextureType_SPECULAR };

			for (uint32_t i = 0; i < nMaterials; i++)
			{
				//VOG_CORE_ASSERT(m_subMeshes[i].materialIndex < nMaterials, "invalid material index");

				const auto& aiMaterial = *pScene->mMaterials[i];

				materialPtrs.emplace_back(createRefPtr<Material>(pShader_));
				auto& material = *materialPtrs[i];

				for (const auto& aiType : aiTextureTypes)
				{
					if (aiMaterial.GetTexture(aiType, 0, &aiTexFilename) == aiReturn_SUCCESS)
					{
						texFilename = aiTexFilename.C_Str();
						std::string uniform_texture_name = Utils::textureTypeToShaderUniformName(Utils::getTextureType(aiType));

						// if uniform do not in shader
						if (!pShader_->getShaderLayout()->contains(uniform_texture_name))
							continue;

						// if not find in map
						if (textureIndexMap.find(texFilename) == textureIndexMap.end())
						{
							texturePtrs.emplace_back(Texture2D::create(m_textureFilepath + texFilename));
							textureIndexMap[texFilename] = textureCounter;
							material.setTexture2D(uniform_texture_name, texturePtrs.back());
							textureCounter++;
						}
						else
						{
							material.setTexture2D(uniform_texture_name, texturePtrs[textureIndexMap.at(texFilename)]);
						}
#ifdef MESH_DEBUG
						VOG_CORE_LOG_TRACE("Loaded material, Index: {0}, type: {1}, path: {2}", i, "Albedo", m_textureFilepath + texFilename);
#endif
					}
					else
					{
						//if (AI_SUCCESS != material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(myMaterial.diffuseColor)))
						//	myMaterial.diffuseColor = MyVec3f(1.0f, 1.0f, 1.0f);
					}
				}
			}
		}

		m_texturePtrs = texturePtrs;
		m_materialPtrs = materialPtrs;

		m_textureIndexMap = textureIndexMap;
	}

	void Model::onUpdateAnimation(float dt_)
	{
		m_currentTime += static_cast<float>(m_pScene->mAnimations[0]->mTicksPerSecond) * dt_;
		m_currentTime = fmod(m_currentTime, static_cast<float>(m_pScene->mAnimations[0]->mDuration));
		_calculateBoneTransform(m_rootNode, Matrix4f(1.0f));
	}

#pragma region Animation

	void Model::_calculateBoneTransform(const AssimpNodeData& node_, const Matrix4f& parentTransform_)
	{
		const std::string& nodeName = node_.name;
		Matrix4f nodeTransform;
		Matrix4f globalTransformation;

		const auto* aiNodeAnim = findAiNodeAnimBy(nodeName);
		if (aiNodeAnim && m_boneInfoMap.find(nodeName) != m_boneInfoMap.end())		// found bone
		{
			const auto& bone = m_boneInfoMap.at(nodeName);

			Vector3f translation = _interpolateTranslation(m_currentTime, aiNodeAnim);
			Quaternion rotation = _interpolateRotation(m_currentTime, aiNodeAnim);
			Vector3f scale = _interpolateScale(m_currentTime, aiNodeAnim);
			nodeTransform = MyMath::translate(translation) * MyMath::toMatrix4f(rotation) * MyMath::scale(scale);

			globalTransformation = m_inverseTransform * parentTransform_ * nodeTransform * node_.transform.getTransformation();
			m_boneTransforms[bone.id] = globalTransformation * bone.offset;
		}
		else
		{
			nodeTransform = node_.localTransform;
			globalTransformation = m_inverseTransform * parentTransform_ * nodeTransform;
		}

		for (uint32_t i = 0; i < node_.nChildren; i++)
		{
			_calculateBoneTransform(node_.children[i], globalTransformation);
		}
	}

	const aiNodeAnim* Model::findAiNodeAnimBy(const std::string& name_) const
	{
		
		for (uint32_t i = 0; i < m_pScene->mAnimations[0]->mNumChannels; i++)
		{
			const auto* node = m_pScene->mAnimations[0]->mChannels[i];
			if (name_._Equal(node->mNodeName.C_Str()))
				return node;
		}
		return nullptr;
	}

	double Model::_getAnimationRatio(double lastTimeStamp_, double nextTimeStamp_, double animationTime_) const
	{
		double midWayLength = animationTime_ - lastTimeStamp_;
		double frameDiff = nextTimeStamp_ - lastTimeStamp_;
		return midWayLength / frameDiff;
	}

	uint32_t Model::_getTranslationIndex(double animationTime_, const aiNodeAnim* pNodeAnim_) const
	{
		uint32_t nTranslation = pNodeAnim_->mNumPositionKeys;
		// TODO: optimized by binary search
		for (uint32_t i = 0; i < nTranslation - 1; i++)
		{
			if (animationTime_ < pNodeAnim_->mPositionKeys[i + 1].mTime)
				return i;
		}
		VOG_CORE_ASSERT(0, "no valid bone translation index!");
		return 0;
	}

	uint32_t Model::_getRotationIndex(double animationTime_, const aiNodeAnim* pNodeAnim_) const
	{
		uint32_t nRotation = pNodeAnim_->mNumRotationKeys;
		// TODO: optimized by binary search
		for (uint32_t i = 0; i < nRotation - 1; i++)
		{
			if (animationTime_ < pNodeAnim_->mRotationKeys[i + 1].mTime)
				return i;
		}
		VOG_CORE_ASSERT(0, "no valid bone translation index!");
		return 0;
	}

	uint32_t Model::_getScaleIndex(double animationTime_, const aiNodeAnim* pNodeAnim_) const
	{
		uint32_t nScale = pNodeAnim_->mNumScalingKeys;
		// TODO: optimized by binary search
		for (uint32_t i = 0; i < nScale - 1; i++)
		{
			if (animationTime_ < pNodeAnim_->mScalingKeys[i + 1].mTime)
				return i;
		}
		VOG_CORE_ASSERT(0, "no valid bone translation index!");
		return 0;
	}

	Vector3f Model::_interpolateTranslation(double animationTime_, const aiNodeAnim* pNodeAnim_) const
	{
		if (pNodeAnim_->mNumPositionKeys == 1)
			return toVec3fFrom(pNodeAnim_->mPositionKeys[0].mValue);

		uint32_t currentIndex = _getTranslationIndex(animationTime_, pNodeAnim_);

		const auto& lastFrame = pNodeAnim_->mPositionKeys[currentIndex];
		const auto& nextFrame = pNodeAnim_->mPositionKeys[currentIndex + 1];

		float ratio = static_cast<float>(_getAnimationRatio(lastFrame.mTime, nextFrame.mTime, animationTime_));
		if (ratio < 0.0f)
			ratio = 0.0f;

		Vector3f result = MyMath::lerp(toVec3fFrom(lastFrame.mValue), toVec3fFrom(nextFrame.mValue), ratio);

		return result;
	}

	Quaternion Model::_interpolateRotation(double animationTime_, const aiNodeAnim* pNodeAnim_) const
	{
		if (pNodeAnim_->mNumRotationKeys == 1)
			return MyMath::normalize(toQuatFrom(pNodeAnim_->mRotationKeys[0].mValue));

		uint32_t currentIndex = _getRotationIndex(animationTime_, pNodeAnim_);

		const auto& lastFrame = pNodeAnim_->mRotationKeys[currentIndex];
		const auto& nextFrame = pNodeAnim_->mRotationKeys[currentIndex + 1];

		float ratio = static_cast<float>(_getAnimationRatio(lastFrame.mTime, nextFrame.mTime, animationTime_));
		if (ratio < 0.0f)
			ratio = 0.0f;

		Quaternion result = MyMath::slerp(toQuatFrom(lastFrame.mValue), toQuatFrom(nextFrame.mValue), ratio);
		result = MyMath::normalize(result);

		return result;
	}

	Vector3f Model::_interpolateScale(double animationTime_, const aiNodeAnim* pNodeAnim_) const
	{
		if (pNodeAnim_->mNumScalingKeys == 1)
			return toVec3fFrom(pNodeAnim_->mScalingKeys[0].mValue);

		uint32_t currentIndex = _getScaleIndex(animationTime_, pNodeAnim_);

		const auto& lastFrame = pNodeAnim_->mScalingKeys[currentIndex];
		const auto& nextFrame = pNodeAnim_->mScalingKeys[currentIndex + 1];

		float ratio = static_cast<float>(_getAnimationRatio(lastFrame.mTime, nextFrame.mTime, animationTime_));
		if (ratio < 0.0f)
			ratio = 0.0f;

		Vector3f result = MyMath::lerp(toVec3fFrom(lastFrame.mValue), toVec3fFrom(nextFrame.mValue), ratio);

		return result;
	}

#pragma endregion


#pragma region ImGuiRender

	void Model::onImGuiRender()
	{
		uint32_t nodeIndexTracker = 0;

#if 0
		// TODO: the panel have some mistake on imgui
		if (ImGui::Begin(m_name.c_str()))
		{
			ImGui::Begin("Properties");

			_drawNode(nodeIndexTracker, m_rootNode);

			if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
				m_pSelectedNode = nullptr;

			if (m_pSelectedNode)
			{
				bool hasChangedValue = false;
				hasChangedValue |= ImGuiLibrary::drawVec3Control("Position", m_pSelectedNode->translation);
				Vector3f rotation = glm::degrees(m_pSelectedNode->rotation);
				hasChangedValue |= ImGuiLibrary::drawVec3Control("Rotation", rotation);
				m_pSelectedNode->rotation = glm::radians(rotation);
				hasChangedValue |= ImGuiLibrary::drawVec3Control("Scale", m_pSelectedNode->scale, 1.0f);

				if (hasChangedValue)
				{
					m_pSelectedNode->updateTransformation();
				}
			}
			ImGui::End();
		}
		ImGui::End();

#endif // 0

		_drawMaterial();
	}

	void Model::_drawNode(uint32_t& nodeIndexTracked_, AssimpNodeData& node_)
	{

		// build up flags for current node
		auto node_flags = ImGuiTreeNodeFlags_OpenOnArrow
							  | ((m_pSelectedNode == &node_) ? ImGuiTreeNodeFlags_Selected : 0)
							  | ((node_.nChildren == 0) ? ImGuiTreeNodeFlags_Leaf : 0);
		node_flags |= ImGuiTreeNodeFlags_SpanAvailWidth;

		// render this node
		const bool isOpened = ImGui::TreeNodeEx((void*)(intptr_t)nodeIndexTracked_, node_flags, node_.name.c_str());

		if (ImGui::IsItemClicked()/* && !ImGui::IsItemToggledOpen()*/)
			m_pSelectedNode = &node_;

		if (isOpened)
		{
			nodeIndexTracked_++;

			for (uint32_t i = 0; i < node_.nChildren; i++)
			{
				_drawNode(nodeIndexTracked_, node_.children[i]);
			}
			ImGui::TreePop();
		}
	}

	void Model::_drawMaterial()
	{
		for (size_t i = 0; i < m_materialPtrs.size(); i++)
		{
			auto& material = *m_materialPtrs[i];
			material.onImGuiRender();
		}
	}

#pragma endregion

}