#include "EnvironmentBaker.hpp"
#include "Core/Component/MeshRenderer.hpp"
#include "GfxDriver/GfxDriver.hpp"
#include "Rendering/ImmediateGfx.hpp"
#include "Rendering/ShaderCompiler.hpp"
#include "ThirdParty/imgui/imgui.h"
#include <glm/glm.hpp>
#include <ktx.h>
#include <spdlog/spdlog.h>
namespace Engine::Editor
{
EnvironmentBaker::EnvironmentBaker() {}

void EnvironmentBaker::CreateRenderData(uint32_t width, uint32_t height)
{
    GetGfxDriver()->WaitForIdle();
    sceneImage = GetGfxDriver()->CreateImage(
        {
            .width = width,
            .height = height,
            .format = Gfx::ImageFormat::R8G8B8A8_SRGB,
            .multiSampling = Gfx::MultiSampling::Sample_Count_1,
            .mipLevels = 1,
            .isCubemap = false,
        },
        Gfx::ImageUsage::ColorAttachment | Gfx::ImageUsage::Texture | Gfx::ImageUsage::TransferDst
    );
}

static std::unique_ptr<Gfx::ShaderProgram> LoadShader(const char* path)
{
    auto shaderCompiler = ShaderCompiler();
    std::fstream f;
    f.open(path);
    if (f.is_open() && f.good())
    {
        std::string ss;
        f >> ss;
        shaderCompiler.Compile(ss, true);
        return GetGfxDriver()->CreateShaderProgram(
            "EnvironmentBaker",
            &shaderCompiler.GetConfig(),
            shaderCompiler.GetVertexSPV(),
            shaderCompiler.GetFragSPV()
        );
    }
    else
    {
        SPDLOG_ERROR("EnvironmentBaker: failed to open EnvironmentBaker.shad");
    }

    return nullptr;
}

void EnvironmentBaker::Bake(int size)
{
    uint32_t minimumSize = glm::pow(2, 5);
    if (size < minimumSize)
        return;

    auto cubemap = GetGfxDriver()->CreateImage(
        {
            .width = (uint32_t)size,
            .height = (uint32_t)size,
            .format = Gfx::ImageFormat::R16G16B16A16_SFloat,
            .multiSampling = Gfx::MultiSampling::Sample_Count_1,
            .mipLevels = 5,
            .isCubemap = true,
        },
        Gfx::ImageUsage::TransferSrc | Gfx::ImageUsage::ColorAttachment | Gfx::ImageUsage::Texture
    );

    // create baking shader if it's not created
    if (lightingBaker == nullptr)
    {
        lightingBaker = LoadShader("Assets/Shaders/Editor/EnvrionmentBaker/EnvironmentLightBaker.shad");
    }

    if (brdfBaker == nullptr)
    {
        brdfBaker = LoadShader("Assets/Shaders/Editor/EnvrionmentBaker/EnvironmentBRDFBaker.shad");
    }

    // create baking resource if it's not created
    if (bakingShaderResource == nullptr)
    {
        bakingShaderResource =
            GetGfxDriver()->CreateShaderResource(lightingBaker.get(), Gfx::ShaderResourceFrequency::Global);
    }
}

void EnvironmentBaker::BakeToCubeFace(Gfx::Image& cubemap, uint32_t face)
{
    ImmediateGfx::RenderToImage(
        cubemap,
        {Gfx::ImageAspect::Color, 0, 1, face, 1},
        *lightingBaker,
        lightingBaker->GetDefaultShaderConfig(),
        *bakingShaderResource,
        Gfx::ImageLayout::Transfer_Dst
    );
}

bool EnvironmentBaker::Tick()
{
    bool open = true;

    ImGui::Begin("Environment Baker", &open);
    // create scene color if it's null or if the window size is changed
    auto contentMax = ImGui::GetWindowContentRegionMax();
    auto contentMin = ImGui::GetWindowContentRegionMin();
    float width = contentMax.x - contentMin.x;
    float height = contentMax.y - contentMin.y;
    if (sceneImage == nullptr || sceneImage->GetDescription().width != (uint32_t)width ||
        sceneImage->GetDescription().height != (uint32_t)height)
    {
        // CreateRenderData(width, height);
    }

    ImGui::InputInt("resolution", &size);
    if (ImGui::Button("Bake"))
    {
        Bake(size);
    }

    ImGui::End();
    return open;
}

void EnvironmentBaker::Open() {}
void EnvironmentBaker::Close()
{
    sceneImage = nullptr;
}

} // namespace Engine::Editor