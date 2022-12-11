#pragma once
#include "Vog/Core/Core.h"

#include "Vog/Utilities/MyCommon.h"

#include "Vog/Graphics/Shader.h"

#include <glad/glad.h>


namespace vog {

	class VOG_API OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& name_, const std::string& vertexSrc_, const std::string& fragSrc_);
		OpenGLShader(const std::string& filepath_);

		virtual ~OpenGLShader();
		virtual void bind() override;
		virtual void unbind() override;

		virtual void reload(const std::string& filepath_ = std::string{}) override;

		virtual void setIsReloaded(bool isReloaded_) override;
		virtual bool getIsReloaded() override;
		//virtual void setMaterialSameShaderReload(const MaterialSameShaderReloadCallBackFn& fp_) override;

		//ShaderDataList reflect() override;

		virtual void setInt(const std::string& name_, int value_) override;
		virtual void setBool(const std::string& name_, bool value_) override;

		virtual void setFloat(const std::string& name_, float value_) override;
		virtual void setFloat2(const std::string& name_, const Vector2f& value_) override;
		virtual void setFloat3(const std::string& name_, const Vector3f& value_) override;
		virtual void setFloat4(const std::string& name_, const Vector4f& value_) override;

		virtual void setMat3(const std::string& name_, const Matrix3f& value_) override;
		virtual void setMat4(const std::string& name_, const Matrix4f& value_) override;

		virtual void setIntArray(const std::string& name_, const int* values_, uint32_t count_) override;
		virtual void setBoolArray(const std::string& name_, const bool* values_, uint32_t count_) override;
		virtual void setFloatArray(const std::string& name_, const float* values_, uint32_t count_) override;
		virtual void setFloat2Array(const std::string& name_, const Vector2f* values_, uint32_t count_) override;
		virtual void setFloat3Array(const std::string& name_, const Vector3f* values_, uint32_t count_) override;
		virtual void setFloat4Array(const std::string& name_, const Vector4f* values_, uint32_t count_) override;
		virtual void setMat3Array(const std::string& name_, const Matrix3f* values_, uint32_t count_) override;
		virtual void setMat4Array(const std::string& name_, const Matrix4f* values_, uint32_t count_) override;

		inline virtual int getUniformLocation(const std::string& name_) const override
		{
			// TODO: should check whether uniform exist
			if (m_locationCacheMap.find(name_) == m_locationCacheMap.end())
			{
				GLint loc = glGetUniformLocation(m_rendererId, name_.c_str());
				if (loc < 0)
				{
					if (m_isPrintError)
					{
						VOG_CORE_LOG_ERROR("cannot find uniform variable: {0}, file name = {1}", name_.c_str(), m_name.c_str());
						m_isPrintError = false;
					}
					return -1;
				}
				m_locationCacheMap[name_] = glGetUniformLocation(m_rendererId, name_.c_str());
			}
			return m_locationCacheMap.at(name_);
		}
		virtual int getBindingLocation(const std::string& name_) const override;

		inline virtual const std::string& getName() const override { return m_name; }
		inline virtual uint32_t getID() const override { return m_rendererId; }
		inline virtual RefPtr<ShaderLayout> getShaderLayout() override { return m_pShaderLayout; };

	private:
		std::unordered_map<GLenum, std::string> _preprocessShaderFile(const std::string& sourceCode_);
		bool _compileAndLinkShader(const std::unordered_map<GLenum, std::string>& shaderSourceMap_);

		void _getShaderInfoLog(GLuint shader_, std::string& outMsg_);
		void _getProgramInfoLog(GLuint program_, std::string& outMsg_);

		std::unordered_map<std::string, OpenGLShaderData> OpenGLShader::_reflect();

	private:
		uint32_t m_rendererId = 0;
		std::string m_name;
		std::string m_filepath;
		mutable std::unordered_map<std::string, GLint> m_locationCacheMap;
		mutable std::unordered_map<std::string, GLint> m_bindingCacheMap;

		std::unordered_map<GLenum, std::string> m_shaderSourceCodeMap;

		bool m_isReloaded = false;
		RefPtr<ShaderLayout> m_pShaderLayout;

		//MaterialSameShaderReloadCallBackFn m_fpMaterialSameShaderReload;

		mutable bool m_isPrintError = true;
	};

	struct OpenGLShaderData
	{
		GLint count; // count of the variable
		GLenum type; // type of the variable (float, vec3 or mat4, etc)

		static const GLsizei bufSize = 40; // maximum name length
		GLchar name[bufSize];
		//std::string name; // variable name in GLSL
		GLsizei nameLength; // name length
	};
}