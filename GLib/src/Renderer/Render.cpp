#include "Render.h"

#include <iostream>

#include "Texture.h"

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace GLib {

    struct RenderData 
    {
        std::shared_ptr<Shader> ColorShader;
        std::shared_ptr<Shader> TextureShader;
        
        std::shared_ptr<VertexArray> QuadVA;
        std::shared_ptr<VertexArray> CubeVA;
        
        std::shared_ptr<Texture> WhiteTexture;

        glm::mat4 s_ViewProjection = glm::mat4(1.0f);
    };

    static RenderData s_Data;

    void Render::Init() {
        
        SetClearColor(0.2f, 0.3f, 0.3f, 1.0f);

        float quadVertices[] = {
            // position           // color                  // texture coord
             1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   1.0f, 1.0f,
             1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,   1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f, 1.0f,   0.0f, 1.0f
        };  

        uint32_t quadIndices[] = {0,1,3, 1,2,3};

        s_Data.QuadVA = std::make_shared<VertexArray>();
        s_Data.QuadVA->Bind();
        std::shared_ptr<VertexBuffer> vb = std::make_shared<VertexBuffer>(quadVertices, sizeof(quadVertices));
        
        vb->SetLayout({
            { "a_Position", GL_FLOAT, 3 },
            { "a_Color", GL_FLOAT, 4 },
            { "a_TexCoord", GL_FLOAT, 2 }
        });
        s_Data.QuadVA->AddVertexBuffer(vb);


        std::shared_ptr<IndexBuffer> ib = std::make_shared<IndexBuffer>(quadIndices, 6);
        s_Data.QuadVA->SetIndexBuffer(ib);

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
            { "a_Color", GL_FLOAT, 4 },
            { "a_TexCoord", GL_FLOAT, 2 }
        });
        s_Data.CubeVA->AddVertexBuffer(cubevb);


        std::shared_ptr<IndexBuffer> cubeib = std::make_shared<IndexBuffer>(cubeIndices, 36);
        s_Data.CubeVA->SetIndexBuffer(cubeib);

        s_Data.WhiteTexture = std::make_shared<Texture>(TextureSpecification());
        uint32_t white = 0xffffffff;
        s_Data.WhiteTexture->SetData(&white, sizeof(uint32_t));
        
        s_Data.ColorShader = std::make_shared<Shader>("assets/shaders/ColorShader.glsl");
        s_Data.TextureShader = std::make_shared<Shader>("assets/shaders/Texture.glsl");

        s_Data.TextureShader->Bind();
        s_Data.TextureShader->UploadUniformInt("u_Texture", 0);
    }

    void Render::BeginFrame(Camera camera)
    {
        Clear();
        s_Data.s_ViewProjection = camera.GetViewProjection();
    }

    void Render::EndFrame()
    {
    }

    void Render::SetClearColor(float r, float g, float b, float a) {
        glClearColor(r, g, b, a);
    }

    void Render::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void Render::RenderQuad(const glm::vec3& position) {
        RenderQuad(position, s_Data.WhiteTexture);
    }

    void Render::RenderQuad(const glm::vec3& position, const std::shared_ptr<Texture>& texture) {
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians((float)glfwGetTime() * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        trans = glm::scale(trans, glm::vec3(1.0f));
        Submit(s_Data.QuadVA, texture, trans);
    }

    void Render::RenderCube(const glm::vec3& position) {
        Submit(s_Data.CubeVA, s_Data.WhiteTexture, glm::translate(glm::mat4(1.0f), position));
    }

    void Render::Submit(const std::shared_ptr<VertexArray>& vertexArray, const std::shared_ptr<Texture>& texture, const glm::mat4& transformation, uint32_t indexCount)
    {
        texture->Bind();
        s_Data.TextureShader->Bind();
        s_Data.TextureShader->UploadUniformMat4("u_Transform", s_Data.s_ViewProjection * transformation);
        vertexArray->Bind();

        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }
}