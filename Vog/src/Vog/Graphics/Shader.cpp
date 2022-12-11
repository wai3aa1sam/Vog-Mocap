#include "vogpch.h"
#include "Shader.h"

#include "Platform/OpenGL/OpenGLShader.h"

#include "Vog/Graphics/Renderer/Renderer.h"

namespace vog {
//#define debug_Shader

    RefPtr<Shader> Shader::create(const std::string& name_, const std::string& vertexSrc_, const std::string& fragSrc_)
    {
        switch (Renderer::getGraphicsAPI())
        {
            case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
            case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLShader>(name_, vertexSrc_, fragSrc_);
        }
        return nullptr;
    }
    RefPtr<Shader> Shader::create(const std::string& filepath_)
    {
        switch (Renderer::getGraphicsAPI())
        {
            case GraphicsAPI::API::None:    VOG_CORE_ASSERT(false, "GraphicsAPI::None is currently not supported!"); return nullptr;
            case GraphicsAPI::API::OpenGL:  return createRefPtr<OpenGLShader>(filepath_);
        }
        return nullptr;
    }

#pragma region ShaderLibrary
    void ShaderLibrary::destroy()
    {
        m_shaderMap.clear();
    }

    void ShaderLibrary::reload()
    {
        for (const auto& shader : m_shaderMap)
        {
            shader.second->reload();
        }
    }

    void ShaderLibrary::add(const std::string& name_, const RefPtr<Shader>& pShader_)
    {
        VOG_CORE_ASSERT(!isExist(name_), "The shader already exits!");
        m_shaderMap[name_] = pShader_;
    }

    void ShaderLibrary::add(const RefPtr<Shader>& pShader_)
    {
        add(pShader_->getName(), pShader_);
    }

    RefPtr<Shader>& ShaderLibrary::load(const std::string& filepath_)
    {
        auto pShader = Shader::create(filepath_);
        add(pShader);
        return m_shaderMap.at(pShader->getName());
    }

    RefPtr<Shader>& ShaderLibrary::load(const std::string& name_, const std::string& filepath_)
    {
        auto pShader = Shader::create(filepath_);
        add(name_, pShader);
        return m_shaderMap.at(pShader->getName());
    }

    void ShaderLibrary::endScene()
    {
        for (const auto& shader : m_shaderMap)
        {
            shader.second->setIsReloaded(false);
        }
    }
#pragma endregion
}