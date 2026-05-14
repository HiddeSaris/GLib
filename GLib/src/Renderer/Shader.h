#pragma once

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

typedef unsigned int GLenum;

namespace GLib {

    class Shader {
    public:
        Shader(const std::string& filepath);
        Shader(const std::string& vertexSrc, const std::string& fragmentSrc);
        ~Shader();

        void Bind() const;
        void Unbind() const;

        // const std::string& GetName() const { return m_Name; }
        
        // void SetInt(const std::string& name, int value);
        // void SetIntArray(const std::string& name, int* values, uint32_t count);
        // void SetFloat(const std::string& name, float value);
        // void SetFloat2(const std::string& name, const glm::vec2& value);
        // void SetFloat3(const std::string& name, const glm::vec3& value);
        // void SetFloat4(const std::string& name, const glm::vec4& value);
        // void SetMat4(const std::string& name, const glm::mat4& value);

        void UploadUniformInt(const std::string& name, int value);
        // void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);
        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        void UploadUniformFloat4(const std::string& name, const glm::vec4& value);
        void UploadUniformMat3(const std::string& name, const glm::mat3 matrix);
        void UploadUniformMat4(const std::string& name, const glm::mat4 matrix);
    private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);
    private:
        uint32_t m_RendererID;
        std::string m_FilePath;
        // std::string m_Name;
    };

}