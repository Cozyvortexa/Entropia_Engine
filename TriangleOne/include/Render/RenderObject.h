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

    class VertexBuffer {
    public:
        virtual ~VertexBuffer() = default;
        virtual void Bind() const = 0;
        virtual void Unbind() const = 0;
    };

    enum class Format
    {
        //Float
        Float1,
        Float2,
        Float3,
        Float4,
        //Int
        UInt1,
        UInt2,
        UInt3,
        UInt4
    };

    struct VertexAttribute
    {
        uint32_t location;
        Format format;
        uint32_t offset;
    };

    struct VertexLayout
    {
        uint32_t stride;
        std::vector<VertexAttribute> attributes;
    };
}