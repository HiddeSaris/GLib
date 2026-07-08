#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Scene/Scene.h"

#include <memory>

namespace GLib {

    std::shared_ptr<Scene> CreateScene();

    class Render {
    public:
        static void Init();

        static void BeginFrame(Camera camera, glm::mat4 transform, glm::vec3 camPos);
        static void BeginFrame(glm::mat4 viewProjection, glm::vec3 camPos);
        static void EndFrame();

        static void SetClearColor(float r, float g, float b, float a);
        static void Clear();

        static void OnViewportResize(uint32_t width, uint32_t height);

        static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f));
        static void DrawLineFlush();

        static void Submit(const VertexArray& vertexArray, const Texture& texture, const glm::mat4& transformation, uint32_t indexCount = 0);
        static void Submit(const Mesh& mesh, const glm::mat4& transform);
        static void Submit(Model& model, const glm::mat4& transform);
    private:

    };

}