#include "Render.h"

#include <iostream>

#include "Texture.h"
#include "VertexArray.h"
#include "Window/Window.h"
#include "Scene/Entity.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GLib {

    struct RenderData 
    {
        std::shared_ptr<Scene> ActiveScene;
        
        std::chrono::steady_clock::time_point BeginFrameTime;
        double DeltaTime;

        std::shared_ptr<Shader> ColorShader;
        std::shared_ptr<Shader> TextureShader;
        
        std::shared_ptr<VertexArray> QuadVA;
        std::shared_ptr<VertexArray> CubeVA;
        
        std::vector<float> LineData;
        int MaxLineVertices;
        std::shared_ptr<VertexArray> LineVA;
        
        std::shared_ptr<Texture> WhiteTexture;

        glm::mat4 ViewProjection = glm::mat4(1.0f);
        glm::vec3 CamPosition = glm::vec3(0.0f);
    };

    static RenderData s_Data;

    void Render::Init() {
        InitImGui();
        SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        //glDisable(GL_CULL_FACE);
        glEnable(GL_CULL_FACE);

        glLineWidth(1);

        s_Data.DeltaTime = 0.016;

        {
            float quadVertices[] = {
                // position          // normal         // texture coord   // color                  
                1.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,   1.0f, 1.0f,   1.0f, 1.0f, 1.0f, 1.0f,
                1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f,   1.0f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f,
                -1.0f, -1.0f, 0.0f,  0.0f, 0.0f, -1.0f,   0.0f, 0.0f,   1.0f, 1.0f, 1.0f, 1.0f,
                -1.0f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,   0.0f, 1.0f,   1.0f, 1.0f, 1.0f, 1.0f,
            };  

            uint32_t quadIndices[] = {3,1,0, 3,2,1};

            s_Data.QuadVA = std::make_shared<VertexArray>();
            s_Data.QuadVA->Bind();
            std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
            
            vb->SetLayout({
                { "a_Position", GL_FLOAT, 3 },
                { "a_Normal", GL_FLOAT, 3 },
                { "a_TexCoord", GL_FLOAT, 2 },
                { "a_Color", GL_FLOAT, 4 },
            });
            s_Data.QuadVA->AddVertexBuffer(vb);


            std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(quadIndices, 6);
            s_Data.QuadVA->SetIndexBuffer(ib);
        }

        {
            // CUBE
            float cubeVertices[] =
            {
                1.f,  1.f,  1.f,   1.f, 1.f, 1.f, 1.f,     1.0f, 1.0f,	//0
                -1.f,  1.f,  1.f,   1.f, 1.f, 1.f, 1.f,	    1.0f, 0.0f, //1
                -1.f,  1.f, -1.f,   1.f, 1.f, 1.f, 1.f,	    0.0f, 0.0f, //2
                1.f,  1.f, -1.f,   1.f, 1.f, 1.f, 1.f,	    0.0f, 1.0f, //3
                1.f, -1.f,  1.f,   1.f, 1.f, 1.f, 1.f,	    1.0f, 1.0f, //4
                -1.f, -1.f,  1.f,   1.f, 1.f, 1.f, 1.f,	    1.0f, 0.0f, //5
                -1.f, -1.f, -1.f,   1.f, 1.f, 1.f, 1.f,	    0.0f, 0.0f, //6
                1.f, -1.f, -1.f,   1.f, 1.f, 1.f, 1.f,	    0.0f, 1.0f  //7
            };
            GLuint cubeIndices[] = 
            {
                0, 1, 3, //top 1
                3, 1, 2, //top 2
                2, 6, 7, //front 1
                7, 3, 2, //front 2
                7, 6, 5, //bottom 1
                5, 4, 7, //bottom 2
                5, 1, 4, //back 1
                4, 1, 0, //back 2
                4, 3, 7, //right 1
                3, 4, 0, //right 2
                5, 6, 2, //left 1
                5, 1, 2  //left 2
            };

            s_Data.CubeVA = std::make_shared<VertexArray>();
            s_Data.CubeVA->Bind();

            std::shared_ptr<VertexBuffer> cubevb = std::make_shared<VertexBuffer>(cubeVertices, sizeof(cubeVertices));
            cubevb->SetLayout({
                { "a_Position", GL_FLOAT, 3 },
                { "a_Normal", GL_FLOAT, 3 },
                { "a_TexCoord", GL_FLOAT, 2 },
                { "a_Color", GL_FLOAT, 4 },
            });
            s_Data.CubeVA->AddVertexBuffer(cubevb);


            std::shared_ptr<IndexBuffer> cubeib = std::make_shared<IndexBuffer>(cubeIndices, 36);
            s_Data.CubeVA->SetIndexBuffer(cubeib);
            s_Data.CubeVA->Unbind();
        }

        {
            s_Data.MaxLineVertices = 1000;
            s_Data.LineVA = std::make_shared<VertexArray>();
            s_Data.LineVA->Bind();

            auto linevb = std::make_shared<VertexBuffer>(s_Data.MaxLineVertices * 6 * sizeof(float));
            linevb->SetLayout({
                { "a_Position", GL_FLOAT, 3 },
                { "a_Color", GL_FLOAT, 3}
            });
            s_Data.LineVA->AddVertexBuffer(linevb);
        }

        s_Data.WhiteTexture = std::make_shared<Texture>(TextureSpecification());
        
        s_Data.ColorShader = std::make_shared<Shader>("assets/shaders/ColorShader.glsl");
        s_Data.TextureShader = std::make_shared<Shader>("assets/shaders/Texture.glsl");

        s_Data.TextureShader->Bind();
    }

    void Render::BeginFrame() {
        Entity cam = s_Data.ActiveScene->GetPrimaryCamera();
        TransformComponent transform = cam.Get<TransformComponent>(); //s_Data.ActiveScene->GetRegistry().get<TransformComponent>(cam);
        CameraComponent camera = cam.Get<CameraComponent>(); //s_Data.ActiveScene->GetRegistry().get<CameraComponent>(cam);

        BeginFrame(camera.m_Camera, transform.GetTransform(), transform.m_Translation);
    }

    void Render::BeginFrame(Camera camera, glm::mat4 transform, glm::vec3 camPos) {
        BeginFrame(camera.GetProjection() * glm::inverse(transform), camPos);
    }

    void Render::BeginFrame(glm::mat4 viewProjection, glm::vec3 camPos) {
        s_Data.BeginFrameTime = std::chrono::steady_clock::now();
        Clear();
        s_Data.ViewProjection = viewProjection;
        s_Data.CamPosition = camPos;
        ImGuiNewFrame();
    }

    void Render::EndFrame() {
        ImGuiEndFrame();
        Window::Update();
        
        std::chrono::steady_clock::time_point time = std::chrono::steady_clock::now();
        std::chrono::duration<double> dt = time - s_Data.BeginFrameTime;
        s_Data.DeltaTime = dt.count();
    }

    void Render::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void Render::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render::OnViewportResize(uint32_t width, uint32_t height) {
        s_Data.ActiveScene->OnViewportResize(width, height);
    }

    std::shared_ptr<VertexArray> Render::GetQuadVertexArray() {
        return s_Data.QuadVA;
    }

    std::shared_ptr<VertexArray> Render::GetCubeVertexArray() {
        return s_Data.CubeVA;
    }

    double Render::GetDeltaTime() {
        return s_Data.DeltaTime;
    }

    void Render::DrawLine(const glm::vec3 &start, const glm::vec3 &end, const glm::vec3 &color) {
        s_Data.LineData.push_back(start.x);
        s_Data.LineData.push_back(start.y);
        s_Data.LineData.push_back(start.z);
        
        s_Data.LineData.push_back(color.x);
        s_Data.LineData.push_back(color.y);
        s_Data.LineData.push_back(color.z);
        
        s_Data.LineData.push_back(end.x);
        s_Data.LineData.push_back(end.y);
        s_Data.LineData.push_back(end.z);
        
        s_Data.LineData.push_back(color.x);
        s_Data.LineData.push_back(color.y);
        s_Data.LineData.push_back(color.z);
    }

    void Render::DrawLineFlush()
    {
        if (s_Data.LineData.empty())
            return;
        
        s_Data.LineVA->Bind();
        s_Data.LineVA->GetVertexBuffers()[0]->SetData(s_Data.LineData.data(), s_Data.LineData.size() * sizeof(float));

        // 6 floats make up a vertex (3 position 3 color)
        int count = s_Data.LineData.size() / 6;

        s_Data.ColorShader->Bind();
        s_Data.ColorShader->UploadUniformMat4("u_Transform", s_Data.ViewProjection);
        
        glDisable(GL_DEPTH_TEST);
        glDrawArrays(GL_LINES, 0, count);
        glEnable(GL_DEPTH_TEST);

        s_Data.LineData.clear();
    }

    void Render::RenderQuad(const glm::vec3& position, std::shared_ptr<Texture> texture) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians((float)glfwGetTime() * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans, glm::vec3(1.0f));
        Submit(s_Data.QuadVA, texture, trans);
    }

    void Render::RenderCube(const glm::vec3& position) {
        Submit(s_Data.CubeVA, s_Data.WhiteTexture, glm::translate(glm::mat4(1.0f), position));
    }

    void Render::Submit(std::shared_ptr<VertexArray> vertexArray, std::shared_ptr<Texture> texture, const glm::mat4 &transform, uint32_t indexCount)
    {
        texture->Bind(0);
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->UploadUniformMat4("u_ViewProjection", s_Data.ViewProjection);
        s_Data.TextureShader->UploadUniformMat4("u_Model", transform);
        s_Data.TextureShader->UploadUniformFloat3("u_ViewPos", s_Data.CamPosition);
        s_Data.TextureShader->UploadUniformFloat3("u_Light.position", glm::vec3(0.5f));
        s_Data.TextureShader->UploadUniformFloat3("u_Light.ambient", glm::vec3(0.8f, 0.5f, 0.3f));
        s_Data.TextureShader->UploadUniformFloat3("u_Light.diffuse", glm::vec3(0.8f, 0.5f, 0.3f));
        s_Data.TextureShader->UploadUniformFloat3("u_Light.specular", glm::vec3(0.8f, 0.3f, 0.6f));
        s_Data.TextureShader->UploadUniformFloat("u_MaterialShininess", 32.0f);
        vertexArray->Bind();

        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void Render::Submit(const Mesh &mesh, const glm::mat4 &transform)
    {
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->UploadUniformMat4("u_ViewProjection", s_Data.ViewProjection);
        s_Data.TextureShader->UploadUniformMat4("u_Model", transform);
        s_Data.TextureShader->UploadUniformFloat3("u_ViewPos", s_Data.CamPosition);
        s_Data.TextureShader->UploadUniformFloat3("u_Light.position", glm::vec3(20.0f));
        s_Data.TextureShader->UploadUniformFloat3("u_Light.ambient", glm::vec3(0.8f, 0.5f, 0.3f));
        s_Data.TextureShader->UploadUniformFloat3("u_Light.diffuse", glm::vec3(0.8f, 0.5f, 0.3f));
        s_Data.TextureShader->UploadUniformFloat3("u_Light.specular", glm::vec3(0.8f, 0.5f, 0.3f));
        s_Data.TextureShader->UploadUniformFloat("u_MaterialShininess", 32.0f);
        
        if (mesh.GetTextures().size() == 0){
            s_Data.WhiteTexture->Bind(0);
        }
        else {
            for (uint32_t i = 0; i < mesh.GetTextures().size(); i++){
                mesh.GetTextures()[i]->Bind(i);
            }
        }
        
        mesh.Bind(*s_Data.TextureShader);

        glDrawElements(GL_TRIANGLES, mesh.GetVertexArray().GetIndexBuffer()->GetCount(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
    }

    void Render::Submit(Model &model, const glm::mat4& transform)
    {
        for (std::shared_ptr<Mesh> mesh : model.GetMeshes()) {
            Submit(*mesh, transform);
        }
    }
    
    void Render::InitImGui()
    {
        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoTaskBarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;

        // ImGuiViewport* viewport = ImGui::GetWindowViewport();
        // viewport->Flags |= ImGuiViewportFlags_TopMost;
        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(Window::GetWindowID(), true);
        ImGui_ImplOpenGL3_Init();
    }

    void Render::ImGuiNewFrame()
    {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    void Render::ImGuiEndFrame()
    {
        ImGuiIO& io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)Window::GetWidth(), (float)Window::GetHeight());

        // Rendering
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }
    }


    std::shared_ptr<Scene> CreateScene() {
        return s_Data.ActiveScene = std::make_shared<Scene>();
    }

    void SetActiveScene(std::shared_ptr<Scene> scene) {
        s_Data.ActiveScene = scene;
    }

    std::shared_ptr<Scene> GetActiveScene() {
        return s_Data.ActiveScene;
    }
}