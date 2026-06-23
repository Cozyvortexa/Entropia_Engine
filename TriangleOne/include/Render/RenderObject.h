#pragma once

#include <vector>

namespace Engine::Render {
    class RendererAPI {
    public:
        enum class API { None, OpenGL, Vulkan };
        static API GetAPI() { return currentAPI; };
    private:
        const static API currentAPI = API::OpenGL;
    };
}

namespace Engine::Render {

    enum class BufferUsage {
        Static,   // GPU_ONLY, upload once
        Dynamic,  // frequently updated
        Stream    // every frame
    };


    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
    };

    enum class Format{
        //Float
        Float1,
        Float2,
        Float3,
        Float4,
        //Int
        UInt1,
        UInt2,
        UInt3,
        UInt4,
        //Matrix
        Mat3,
        Mat4
    };

    enum class BufferClearTarget {
        Color,
        Depth
    };

    struct VertexAttribute
    {
        uint32_t location;
        Format format;  // Value type
        uint32_t offset;
    };

    struct VertexLayout
    {
        uint32_t stride;
        std::vector<VertexAttribute> attributes;
    };

    struct Buffer {

        virtual void Bind() = 0;
        virtual void UnBind() = 0;
        virtual void ClearBuffer(BufferClearTarget target) = 0;
    };

    struct OpenGlBuffer : public Buffer {

        void ClearBuffer(BufferClearTarget target) override {
            if (target == Engine::Render::BufferClearTarget::Color) {
                glClear(GL_COLOR_BUFFER_BIT);
            }
            else if (target == Engine::Render::BufferClearTarget::Depth){
                glClear(GL_DEPTH_BUFFER_BIT);
            }

        }
    };

    enum class Filter{
        Nearest,
        Linear
    };

    enum class WrapMode{
        Repeat,
        ClampEdge
    };

    struct SamplerDesc{
        Filter minFilter;
        Filter magFilter;

        WrapMode wrapU;
        WrapMode wrapV;
    };
}