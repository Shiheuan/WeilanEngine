#include "WeilanEngine.hpp"
#include "Core/AssetDatabase/AssetDatabase.hpp"
#include "Core/AssetDatabase/Importers/GLBImporter2.hpp"
#include "Core/AssetDatabase/Importers/GeneralImporter.hpp"
#include "Core/AssetDatabase/Importers/ShaderImporter.hpp"
#include "Core/AssetDatabase/Importers/TextureImporter.hpp"
#include "Core/AssetDatabase/Importers/glbImporter.hpp"
#include "Core/GameObject.hpp"
#include "Core/Rendering/RenderPipeline.hpp"
#include "GfxDriver/GfxDriver.hpp"
#include "Libs/FileSystem/FileSystem.hpp"
#include "Rendering/GfxResourceTransfer.hpp"
#include "Script/LuaBackend.hpp"
#include <chrono>
#include <glm/gtc/matrix_transform.hpp>
#include <spdlog/spdlog.h>

#undef CreateSemaphore

namespace Engine
{
void WeilanEngine::Launch(std::filesystem::path path)
{
    // drivers
    Gfx::GfxDriver::CreateInfo gfxDriverCreateInfo{
        .windowSize = {1920, 1080},
    };

    Gfx::GfxDriver::CreateGfxDriver(Gfx::Backend::Vulkan, gfxDriverCreateInfo);
    gfxDriver = Gfx::GfxDriver::Instance();

    spdlog::set_level(spdlog::level::info);

    projectManagement = MakeUnique<Editor::ProjectManagement>();
    Editor::ProjectManagement::instance = projectManagement;

    std::filesystem::current_path(path);

    if (std::filesystem::exists(path) && std::filesystem::is_empty(path))
    {
        projectManagement->CreateNewProject(path);
        SPDLOG_INFO("Creating project in {}", path.string());
    }
    else
    {
        SPDLOG_INFO("Loading project");
        projectManagement->LoadProject(path);
    }

    LuaBackend::Instance()->LoadLuaInFolder(path / "Assets");
    AssetDatabase::InitSingleton(path);
    RegisterAssetImporters();
    AssetDatabase::Instance()->LoadInternalAssets();
    AssetDatabase::Instance()->LoadAllAssets();

    // recover last active scene
    UUID lastActiveSceneUUID = projectManagement->GetLastActiveScene();
    if (!lastActiveSceneUUID.IsEmpty())
        GameSceneManager::Instance()->SetActiveGameScene(
            AssetDatabase::Instance()->GetAssetObject(lastActiveSceneUUID));

    // modules
    renderPipeline = MakeUnique<Rendering::RenderPipeline>(gfxDriver.Get());
#if GAME_EDITOR
    gameEditor = MakeUnique<Editor::GameEditor>(gfxDriver.Get(), projectManagement);
    gameEditor->Init();
    renderPipeline->Init(gameEditor->GetGameEditorRenderer());
#else
    renderPipeline->Init(nullptr);
#endif

    auto mainQueue = gfxDriver->GetQueue(QueueType::Main);

    // main loop
    SDL_Event sdlEvent;
    bool shouldBreak = false;
    while (!shouldBreak)
    {

        while (SDL_PollEvent(&sdlEvent))
        {
#if GAME_EDITOR
            gameEditor->ProcessEvent(sdlEvent);
#endif
            switch (sdlEvent.type)
            {
                case SDL_KEYDOWN:
                    if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_Q)
                    {
                        shouldBreak = true;
                    }
            }
        }

        // update
        auto activeGameScene = GameSceneManager::Instance()->GetActiveGameScene();
        if (activeGameScene)
            activeGameScene->Tick();
#if GAME_EDITOR
        gameEditor->Tick();
#endif

        // rendering
        renderPipeline->Render(activeGameScene);

        AssetDatabase::Instance()->EndOfFrameUpdate();
    }

    gfxDriver->WaitForIdle();

    gameEditor->Deinit();
}

WeilanEngine::~WeilanEngine()
{
    gameEditor = nullptr;
    renderPipeline = nullptr;
    AssetDatabase::Deinit();
    Internal::GfxResourceTransfer::DestroyGfxResourceTransfer();
    Gfx::GfxDriver::DestroyGfxDriver();
}

void WeilanEngine::RegisterAssetImporters()
{
    RefPtr<AssetDatabase> assetDb = AssetDatabase::Instance();
    assetDb->RegisterImporter<Internal::GLBImporter2>("glb");
    assetDb->RegisterImporter<Internal::GeneralImporter<Material>>("mat");
    assetDb->RegisterImporter<Internal::GeneralImporter<Rendering::RenderPipelineAsset>>("rp");
    assetDb->RegisterImporter<Internal::GeneralImporter<GameScene>>("game");
    assetDb->RegisterImporter<Internal::ShaderImporter>("shad");
    assetDb->RegisterImporter<Internal::TextureImporter>("png");
    assetDb->RegisterImporter<Internal::TextureImporter>("jpeg");
    assetDb->RegisterImporter<Internal::TextureImporter>("jpg");
    assetDb->RegisterImporter<Internal::TextureImporter>("ktx");
    assetDb->RegisterImporter<Internal::TextureImporter>("ktx2");
}
} // namespace Engine

