#pragma once

#include <glad/glad/glad.h>
#include <Jolt/Jolt.h>
#include <Jolt/Renderer/DebugRenderer.h>
#include <vector>

#include "Render/RenderObject.h"

//Jolt Debug renderer
namespace Engine::Physics {
    class JoltDebugRenderer : public JPH::DebugRenderer {
    public:
        JoltDebugRenderer() = default;
        virtual ~JoltDebugRenderer() = default;
        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override = 0;

        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow) override = 0;

        virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor, float inHeight) override = 0;



        virtual Batch CreateTriangleBatch(const Triangle* inTriangles, int inTriangleCount) override { return Batch(); }
        virtual Batch CreateTriangleBatch(const Vertex* inVertices, int inVertexCount, const uint32_t* inIndices, int inIndexCount) override { return Batch(); }
        virtual void DrawGeometry(JPH::RMat44Arg inModelMatrix, const JPH::AABox& inWorldSpaceBounds, float inLODScaleSq, JPH::ColorArg inModelColor, const GeometryRef& inGeometry, ECullMode inCullMode = ECullMode::CullBackFace, ECastShadow inCastShadow = ECastShadow::On, EDrawMode inDrawMode = EDrawMode::Solid) override {}
    };


    class JoltOpenGLDebugRenderer : public JoltDebugRenderer {
    public:
        JoltOpenGLDebugRenderer() {
            glGenVertexArrays(1, &lineVAO);
            glGenBuffers(1, &lineVBO);
            glBindVertexArray(lineVAO);
            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);

            glEnableVertexAttribArray(0); // Pos
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

            glEnableVertexAttribArray(1); // Color
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, r));


            glGenVertexArrays(1, &triangleVAO);
            glGenBuffers(1, &triangleVBO);
            glBindVertexArray(triangleVAO);
            glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);

            glEnableVertexAttribArray(0); // Pos
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, x));

            glEnableVertexAttribArray(1); // Color
            glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (void*)offsetof(Vertex, r));

            glBindVertexArray(0);

            Initialize();
        }

        ~JoltOpenGLDebugRenderer() {
            glDeleteVertexArrays(1, &lineVAO);
            glDeleteBuffers(1, &lineVBO);
            glDeleteVertexArrays(1, &triangleVAO);
            glDeleteBuffers(1, &triangleVBO);
        }

        virtual void DrawLine(JPH::RVec3Arg inFrom, JPH::RVec3Arg inTo, JPH::ColorArg inColor) override {
            Vertex vertices[2] = {
                { inFrom.GetX(), inFrom.GetY(), inFrom.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a },
                { inTo.GetX(), inTo.GetY(), inTo.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a }
            };

            glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

            glBindVertexArray(lineVAO);
            glDrawArrays(GL_LINES, 0, 2);
        }

        virtual void DrawTriangle(JPH::RVec3Arg inV1, JPH::RVec3Arg inV2, JPH::RVec3Arg inV3, JPH::ColorArg inColor, ECastShadow inCastShadow = ECastShadow::Off) override {
            Vertex vertices[3] = {
                { inV1.GetX(), inV1.GetY(), inV1.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a },
                { inV2.GetX(), inV2.GetY(), inV2.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a },
                { inV3.GetX(), inV3.GetY(), inV3.GetZ(), inColor.r, inColor.g, inColor.b, inColor.a }
            };

            glBindBuffer(GL_ARRAY_BUFFER, triangleVBO);
            glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STREAM_DRAW);

            glBindVertexArray(triangleVAO);
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }

        virtual void DrawText3D(JPH::RVec3Arg inPosition, const JPH::string_view& inString, JPH::ColorArg inColor = JPH::Color::sWhite, float inHeight = 0.5f) override {}

    private:
        GLuint lineVAO, lineVBO;
        GLuint triangleVAO, triangleVBO;

        // Structure interne pour envoyer les données à OpenGL
        struct Vertex {
            float x, y, z;
            uint8_t r, g, b, a; // Couleur fournie par Jolt
        };
    };


    inline std::unique_ptr<JoltDebugRenderer> JoltDebugRenderer_Factory() {
        Engine::Render::RendererAPI::API graphicAPI = Engine::Render::RendererAPI::GetAPI();

        std::unique_ptr<JoltDebugRenderer> joltRenderer = nullptr;
        switch (graphicAPI)
        {
        case Engine::Render::RendererAPI::API::None:
            std::cout << "RendererAPI is set on None" << std::endl;
            abort();
        case Engine::Render::RendererAPI::API::OpenGL:
            joltRenderer = std::make_unique<JoltOpenGLDebugRenderer>();
            break;
        case Engine::Render::RendererAPI::API::Vulkan:
            std::cout << "RendererAPI is set on Vulkan" << std::endl;
            abort();
        }
        return joltRenderer;
    }
}