#pragma once

#include <vector>
#include <array>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define MAX_SHADOW_CASTER 32

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

    //Shadow
    enum class Shadow_Quality : int {
        Low,
        Medium,
        Hight,
        VeryHight
    };

    inline float ShadowQuality_To_Value(Shadow_Quality shadowQuality) {
        switch (shadowQuality)
        {
        case Engine::Render::Shadow_Quality::Low:
            return 256;
        case Engine::Render::Shadow_Quality::Medium:
            return 512;
        case Engine::Render::Shadow_Quality::Hight:
            return 1024;
        case Engine::Render::Shadow_Quality::VeryHight:
            return 4096;
        default:
            return -1;
        }
    }

    struct ShadowCaster {
        Shadow_Quality quality = Shadow_Quality::Hight;
        bool castShadow = true;
    };

    // Handled the pointLight, Aligned in GPU memory
    struct ShadowMap_PointLight {
        ShadowMap_PointLight() = default;
        ShadowMap_PointLight(Shadow_Quality quality, glm::vec3& lightPos, float far_plane) {
            this->quality = quality;
            this->lightPos = lightPos;
            this->far_plane = far_plane;
            Generate_PointLight_ShadowProj();
        }

        alignas(16) Shadow_Quality quality;
        alignas(16) std::array<glm::mat4, 6> faces;
        glm::vec3 lightPos;
        float far_plane;
    private:
        void Generate_PointLight_ShadowProj() {
            float aspect = 1.0f;
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, far_plane);

            faces[0] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0));  //X+
            faces[1] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0, 0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)); //X-
            faces[2] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 0.0, 1.0));   //Y+
            faces[3] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, -1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)); //Y-
            faces[4] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, 1.0), glm::vec3(0.0, -1.0, 0.0));  //Z+
            faces[5] = shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.0, -1.0, 0.0)); //Z-
        }
    };
    // Handled the dirlight and the spotlight
    struct ShadowMap {
        ShadowMap() = default;
        ShadowMap(Shadow_Quality quality) {
            this->quality = quality;
        }
        glm::mat4 lightSpaceMatrice = glm::mat4(1.0);
        Shadow_Quality quality;
        float _pad[3];
        
        void Generate_SpotLight_ShadowProj(glm::vec3 position, glm::vec3 direction, float range, float outerCutOff) {
            float aspect = 1.0f;
            glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), aspect, 0.1f, range);

            glm::mat4 projection = glm::perspective(glm::radians(outerCutOff * 2.0f), aspect, 0.1f, range);
            glm::vec3 up = (glm::abs(direction.y) > 0.99f) ? glm::vec3(1, 0, 0) : glm::vec3(0, 1, 0);
            glm::mat4 lightView = glm::lookAt(position, position + direction, up);


            lightSpaceMatrice = projection * lightView;
        }
    };
}

#pragma region Padding_Structures
struct Padding_DirLight {  // To be identical to the memory alignment of structs in the shaders, SSBO
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
};
struct Padding_PointLight {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 color;
    float range = 0.0f;
    int shadowIndex = -1;
    int shadowQuality = -1;
};
struct Padding_SpotLight {
    alignas(16) glm::vec3 position;
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 color;
    float cutOff;
    float outerCutOff;
    float range;
    int shadowIndex = -1;
    int shadowQuality = -1;
};
struct All_Light {
    Padding_DirLight dirLight; // Only one DirLight
    std::vector<Padding_PointLight> pointLights;
    std::vector<Padding_SpotLight> spotLights;

    bool dirLightCastShadow = false;

    /////////// --- Shadow --- ///////////
    //Directional light
    Engine::Render::ShadowMap shadow_DirLight;

    //Point lights
    std::vector<Engine::Render::ShadowMap_PointLight> shadow_PointLights;

    //Spot lights
    std::vector<Engine::Render::ShadowMap> shadow_SpotLights;

    void Clear() {
        pointLights.clear();
        spotLights.clear();
        shadow_PointLights.clear();
        shadow_SpotLights.clear();
        dirLightCastShadow = false;
        dirLight = Padding_DirLight();
    }
};
#pragma endregion