#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"

#include <memory>

namespace GLib {


    class Render {
    public:
        static void Init();

        static void BeginFrame(Camera camera);
        static void EndFrame();

        static void SetClearColor(float r, float g, float b, float a);
        static void Clear();

        static void RenderQuad(const glm::vec3& position);
        static void RenderQuad(const glm::vec3& position, const Texture& texture);
        static void RenderCube(const glm::vec3& position);

        static void Submit(const VertexArray& vertexArray, const Texture& texture, const glm::mat4& transformation, uint32_t indexCount = 0);
        static void Submit(const Mesh& mesh, const glm::mat4& transform);
        static void Submit(Model& model, const glm::mat4& transform);
    private:

    };

}