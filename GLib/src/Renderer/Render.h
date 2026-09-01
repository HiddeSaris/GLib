#pragma once

#include "VertexArray.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
#include "Model.h"
#include "Mesh.h"
#include "Scene/Scene.h"

#include <memory>

namespace GLib {

    std::shared_ptr<Scene> CreateScene();
    void SetActiveScene(std::shared_ptr<Scene> scene);
    std::shared_ptr<Scene> GetActiveScene();

    class Render {
    public:
        static void Init();

        static void BeginFrame();
        static void BeginFrame(Camera camera, glm::mat4 transform, glm::vec3 camPos);
        static void BeginFrame(glm::mat4 view, glm::mat4 projection, glm::vec3 camPoss);
        static void EndFrame();

        static void SetClearColor(float r, float g, float b, float a);
        static void Clear();

        static void OnViewportResize(uint32_t width, uint32_t height);

        static std::shared_ptr<VertexArray> GetQuadVertexArray();
        static std::shared_ptr<VertexArray> GetCubeVertexArray();
        static double GetDeltaTime();

        static void DrawLine(const glm::vec3& start, const glm::vec3& end, const glm::vec3& color = glm::vec3(1.0f));
        static void DrawLineFlush();

        static void RenderQuad(const glm::vec3& position, std::shared_ptr<Texture> texture);
        static void RenderCube(const glm::vec3& position);

        static void SubmitPointLight(glm::vec3 pos, glm::vec3 color, float intensity);
        static void SubmitDirLight(glm::vec3 dir, glm::vec3 color, float intensity);
        static void SubmitSpotLight(glm::vec3 pos, glm::vec3 dir, glm::vec3 color, float intensity, float cutoff, float outerCutoff);

        static void Submit(std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<Texture> texture, const glm::mat4 &transform, uint32_t indexCount = 0);
        static void Submit(std::shared_ptr<Mesh> mesh, const glm::mat4& transform);
        static void Submit(Model& model, const glm::mat4& transform);
    private:
        static void UploadPBRUniforms(const glm::mat4& transform);
        static void InitImGui();
        static void ImGuiNewFrame();
        static void ImGuiEndFrame();
    };

}