#include "vogpch.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL/OpenGLDefines.h"
#include "Platform/OpenGL/OpenGLUtils.h"

#include <filesystem>

namespace vog {

	namespace Utils
	{
		std::filesystem::path getDefaultShaderPath()
		{
			return "assets/shaders/Default.glsl";
		}
	}

	OpenGLShader::OpenGLShader(const std::string& name_, const std::string& vertexSrc_, const std::string& fragSrc_)
		:
		m_name(name_)
	{
		m_shaderSourceCodeMap[GL_VERTEX_SHADER] = vertexSrc_;
		m_shaderSourceCodeMap[GL_FRAGMENT_SHADER] = fragSrc_;
		_compileAndLinkShader(m_shaderSourceCodeMap);

		m_pShaderLayout = createRefPtr<ShaderLayout>(_reflect());
	}

	OpenGLShader::OpenGLShader(const std::string& filepath_)
		:
		m_name(std::filesystem::path(filepath_).stem().string()), m_filepath(filepath_)
	{
		reload(m_filepath);

		m_pShaderLayout = createRefPtr<ShaderLayout>(_reflect());
	}

	OpenGLShader::~OpenGLShader()
	{
		VOG_CORE_ASSERT(m_rendererId, "OpenGLShader delete Failed, no valid rendererId");

		glDebugCall(glDeleteProgram(m_rendererId));
	}

	void OpenGLShader::bind()
	{
		glDebugCall(glUseProgram(m_rendererId));
	}

	void OpenGLShader::unbind()
	{
		glDebugCall(glUseProgram(0));
	}

	void OpenGLShader::reload(const std::string& filepath_)
	{
		if (m_rendererId)
		{
			glDebugCall(glDeleteProgram(m_rendererId));
			m_rendererId = 0;

			m_locationCacheMap.clear();
			m_shaderSourceCodeMap.clear();
		}
		std::string source;

		if (m_filepath.empty())
			my_readFile(source, filepath_);
		else
			my_readFile(source, m_filepath);

		m_shaderSourceCodeMap = _preprocessShaderFile(source);
		bool isSuccess = _compileAndLinkShader(m_shaderSourceCodeMap);

		if (m_pShaderLayout && isSuccess)
		{
			m_pShaderLayout->reload(_reflect());
			setIsReloaded(true);

			s_is_reset_error = true;		// TODO: remove
		}
		else if (!isSuccess)
		{
			if (m_rendererId)
			{
				glDebugCall(glDeleteProgram(m_rendererId));
				m_rendererId = 0;

				m_locationCacheMap.clear();
				m_shaderSourceCodeMap.clear();
			}
			my_readFile(source, Utils::getDefaultShaderPath().string());
			m_shaderSourceCodeMap = _preprocessShaderFile(source);
			_compileAndLinkShader(m_shaderSourceCodeMap);
		}
	}

	void OpenGLShader::setIsReloaded(bool isReloaded_)
	{
		m_isReloaded = isReloaded_;
	}

	bool OpenGLShader::getIsReloaded()
	{
		return m_isReloaded;
	}

	std::unordered_map<std::string, OpenGLShaderData> OpenGLShader::_reflect()
	{
		GLint i;
		GLint nUniforms;

		glGetProgramiv(m_rendererId, GL_ACTIVE_UNIFORMS, &nUniforms);
#ifdef debug_shader
		VOG_CORE_LOG_INFO("Shader: {0}", m_name);
		VOG_CORE_LOG_TRACE("Active Uniforms: {0}", nUniforms);
#endif // ShaderDeug

		//---------
		uint32_t offset = 0;
		std::unordered_map<std::string, OpenGLShaderData> openglShaderMap;

		//dataMap.reserve(nUniforms);
		for (i = 0; i < nUniforms; i++)
		{
			OpenGLShaderData openglShaderData;

			glGetActiveUniform(m_rendererId, (GLuint)i, 40 /*name buffer size*/,
				&openglShaderData.nameLength, &openglShaderData.count, &openglShaderData.type, openglShaderData.name);

#ifdef debug_shader
			VOG_CORE_LOG_TRACE("Uniform #{0} Type: {1} Name: {2}, Count: {3}", i, openglShaderData.type, openglShaderData.name, openglShaderData.count);
#endif // ShaderDeug

			openglShaderMap[openglShaderData.name] = openglShaderData;
		}

		return openglShaderMap;
	}

	int OpenGLShader::getBindingLocation(const std::string& name_) const
	{
		int location = getUniformLocation(name_);

		if (m_bindingCacheMap.find(name_) == m_bindingCacheMap.end())
		{
			GLint binding = -1;
			glGetUniformiv(m_rendererId, location, &binding);
			if (binding < 0 || location < 0)
			{
				if (m_isPrintError)
				{
					VOG_CORE_LOG_ERROR("cannot find binding, variable: {0}, file name = {1}", name_.c_str(), m_name.c_str());
					m_isPrintError = false;
				}
				return -1;
			}
			m_bindingCacheMap[name_] = binding;
		}
		return m_bindingCacheMap.at(name_);
	}

#pragma region setUniform
	void OpenGLShader::setInt(const std::string& name_, int value_)
	{
		glDebugCall(glUniform1i(getUniformLocation(name_), value_));
	}

	void OpenGLShader::setBool(const std::string& name_, bool value_)
	{
		glDebugCall(glUniform1i(getUniformLocation(name_), value_));
	}

	void OpenGLShader::setFloat(const std::string& name_, float value_)
	{
		glDebugCall(glUniform1f(getUniformLocation(name_), value_));
	}

	void OpenGLShader::setFloat2(const std::string& name_, const Vector2f& value_)
	{
		glDebugCall(glUniform2fv(getUniformLocation(name_), 1, glm::value_ptr(value_)));
	}

	void OpenGLShader::setFloat3(const std::string& name_, const Vector3f& value_)
	{
		glDebugCall(glUniform3fv(getUniformLocation(name_), 1, glm::value_ptr(value_)));
	}

	void OpenGLShader::setFloat4(const std::string& name_, const Vector4f& value_)
	{
		glDebugCall(glUniform4fv(getUniformLocation(name_), 1, glm::value_ptr(value_)));
	}

	void OpenGLShader::setMat3(const std::string& name_, const Matrix3f& value_)
	{
		glDebugCall(glUniformMatrix3fv(getUniformLocation(name_), 1, GL_FALSE, glm::value_ptr(value_)));
	}

	void OpenGLShader::setMat4(const std::string& name_, const Matrix4f& value_)
	{
		glDebugCall(glUniformMatrix4fv(getUniformLocation(name_), 1, GL_FALSE, glm::value_ptr(value_)));
	}

	void OpenGLShader::setIntArray(const std::string& name_, const int* values_, uint32_t count_)
	{
		glDebugCall(glUniform1iv(getUniformLocation(name_), count_, values_));
	}

	void OpenGLShader::setBoolArray(const std::string& name_, const bool* values_, uint32_t count_)
	{
		glDebugCall(glUniform1iv(getUniformLocation(name_), count_, (int*)values_));
	}

	void OpenGLShader::setFloatArray(const std::string& name_, const float* values_, uint32_t count_)
	{
		glDebugCall(glUniform1fv(getUniformLocation(name_), count_, values_));
	}

	void OpenGLShader::setFloat2Array(const std::string& name_, const Vector2f* values_, uint32_t count_)
	{
		glDebugCall(glUniform2fv(getUniformLocation(name_), count_, glm::value_ptr(*values_)));
	}

	void OpenGLShader::setFloat3Array(const std::string& name_, const Vector3f* values_, uint32_t count_)
	{
		glDebugCall(glUniform3fv(getUniformLocation(name_), count_, glm::value_ptr(*values_)));
	}

	void OpenGLShader::setFloat4Array(const std::string& name_, const Vector4f* values_, uint32_t count_)
	{
		glDebugCall(glUniform4fv(getUniformLocation(name_), count_, glm::value_ptr(*values_)));
	}

	void OpenGLShader::setMat3Array(const std::string& name_, const Matrix3f* values_, uint32_t count_)
	{
		glDebugCall(glUniformMatrix3fv(getUniformLocation(name_), count_, GL_FALSE, glm::value_ptr(*values_)));
	}

	void OpenGLShader::setMat4Array(const std::string& name_, const Matrix4f* values_, uint32_t count_)
	{
		glDebugCall(glUniformMatrix4fv(getUniformLocation(name_), count_, GL_FALSE, glm::value_ptr(*values_)));
	}
#pragma endregion

#pragma region Shader_and_program_compile_link_info

	std::unordered_map<GLenum, std::string> OpenGLShader::_preprocessShaderFile(const std::string& sourceCode_)
	{
		std::unordered_map<GLenum, std::string> shaderSourceMap;
		std::string shaderTypeToken;

		// #type vertex
		std::string_view typeToken = "#type";

		size_t pos = sourceCode_.find(typeToken, 0);

		while (pos != std::string::npos)
		{
			pos += typeToken.length();

			size_t newLine = my_getToken(shaderTypeToken, sourceCode_, pos, "\r\n");

			pos = sourceCode_.find(typeToken, pos);

			std::string code = (pos == std::string::npos) ?
				sourceCode_.substr(newLine, sourceCode_.size() - newLine) : sourceCode_.substr(newLine, pos - newLine);

			//VOG_CORE_LOG_TRACE("shader type: {0}", shaderTypeToken);
			//VOG_CORE_LOG_TRACE("source code: {0}", code);

			shaderSourceMap[Utils::getShaderType(shaderTypeToken)] = code;
		}
		return shaderSourceMap;
	}

	bool OpenGLShader::_compileAndLinkShader(const std::unordered_map<GLenum, std::string>& shaderSourceMap_)
	{
		std::array<GLuint, 5> shaderIds;
		GLuint programId = glCreateProgram();
		//m_rendererId = 
		uint32_t index = 0;

		for (auto& shaderSource : shaderSourceMap_)
		{
			shaderIds[index] = glCreateShader(shaderSource.first);
			const char* srcData = shaderSource.second.c_str();
			GLint srcSize = static_cast<GLint>(shaderSource.second.size());
			glShaderSource(shaderIds[index], 1, &srcData, &srcSize);

			glDebugCall(glCompileShader(shaderIds[index]));
			GLint compiled = 0;
			glGetShaderiv(shaderIds[index], GL_COMPILE_STATUS, &compiled);
			if (!compiled)
			{
				std::string info;
				_getShaderInfoLog(shaderIds[index], info);

				std::string tmp = "Error compile shader filename: ";
				tmp += m_name;
				tmp += "\n";
				tmp += info;
				VOG_CORE_LOG_ERROR(tmp);
			}
			glDebugCall(glAttachShader(programId, shaderIds[index]));
			index++;
		}

		glDebugCall(glLinkProgram(programId));
		GLint linked = 0;
		glGetProgramiv(programId, GL_LINK_STATUS, &linked);
		if (!linked)
		{
			std::string info;
			_getProgramInfoLog(programId, info);

			std::string tmp = "Error compile shader filename: ";
			tmp += m_name;
			tmp += "\n";
			tmp += info;
			VOG_CORE_LOG_ERROR(tmp);
		}

		glDebugCall(glValidateProgram(programId));
		GLint validated;
		glGetProgramiv(programId, GL_VALIDATE_STATUS, &validated);
		if (!validated)
		{
			std::string info;
			_getProgramInfoLog(programId, info);

			std::string tmp = "Error validate shader filename = ";
			tmp += m_name;
			tmp += "\n";
			tmp += info;
			VOG_CORE_LOG_ERROR(tmp);
		}

		// delete shader after linking
		for (uint32_t i = 0; i < index; i++)
		{
			uint32_t id = shaderIds[i];
			glDebugCall(glDetachShader(programId, id));
			glDebugCall(glDeleteShader(id));
		}

		// assign to member rendererId if no error
		m_rendererId = programId;
		return linked;
	}

	void OpenGLShader::_getShaderInfoLog(GLuint shader_, std::string& outMsg_)
	{
		outMsg_.clear();
		if (!shader_) return;

		GLint bufLen = 0;
		GLint outLen = 0;

		glGetShaderiv(shader_, GL_INFO_LOG_LENGTH, &bufLen);
		outMsg_.resize(bufLen + 1);

		glGetShaderInfoLog(shader_, bufLen, &outLen, &*outMsg_.begin());
		outMsg_.resize(outLen);
	}

	void OpenGLShader::_getProgramInfoLog(GLuint program_, std::string& outMsg_)
	{
		outMsg_.clear();
		if (!program_) return;

		GLint bufLen = 0;
		GLint outLen = 0;

		glGetProgramiv(program_, GL_INFO_LOG_LENGTH, &bufLen);
		outMsg_.resize(bufLen + 1);

		glGetProgramInfoLog(program_, bufLen, &outLen, &*outMsg_.begin());
		outMsg_.resize(outLen);
	}
#pragma endregion
}