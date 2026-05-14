#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

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
        static void RenderQuad(const glm::vec3& position, const std::shared_ptr<Texture>& texture);
        static void RenderCube(const glm::vec3& position);

        static void Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture>& texture, const glm::mat4& transformation, uint32_t indexCount = 0);
    private:

    };

}