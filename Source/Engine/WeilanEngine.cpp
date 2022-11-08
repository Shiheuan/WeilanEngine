#include "WeilanEngine.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>
#include <spdlog/spdlog.h>
#include "Core/GameObject.hpp"
#include "GfxDriver/GfxDriver.hpp"
#include "Core/AssetDatabase/AssetDatabase.hpp"
#include "Core/Rendering/RenderPipeline.hpp"
#include "Core/AssetDatabase/Importers/glbImporter.hpp" 
#include "Core/AssetDatabase/Importers/GeneralImporter.hpp"
#include "Core/AssetDatabase/Importers/ShaderImporter.hpp"
#include "Core/AssetDatabase/Importers/TextureImporter.hpp"
#include "Core/Global/Global.hpp"

namespace Engine
{
    void WeilanEngine::Launch()
    {
        spdlog::set_level(spdlog::level::info);

        Global::InitSingleton();
        AssetDatabase::InitSingleton();
        // register importers
        RegisterAssetImporters();
        // drivers
        Gfx::GfxDriver::CreateGfxDriver(Gfx::Backend::Vulkan);
        gfxDriver = Gfx::GfxDriver::Instance();

        // modules
        renderPipeline = MakeUnique<Rendering::RenderPipeline>(gfxDriver.Get());
#if GAME_EDITOR
        gameEditor = MakeUnique<Editor::GameEditor>(gfxDriver.Get());
#endif

        ConfigureProjectPath();
        // init
#if GAME_EDITOR
        AssetDatabase::Instance()->LoadInternalAssets();
        AssetDatabase::Instance()->LoadAllAssets();
        gameEditor->LoadCurrentProject();
#endif
        renderPipeline->Init();
        gameEditor->Init(renderPipeline);

        // main loop
        SDL_Event sdlEvent;
        bool shouldBreak = false;
        while (!shouldBreak)
        {

            while (SDL_PollEvent(&sdlEvent)) {
#if GAME_EDITOR
                gameEditor->ProcessEvent(sdlEvent);
#endif
                switch (sdlEvent.type) {
                    case SDL_KEYDOWN:
                    if (sdlEvent.key.keysym.scancode == SDL_SCANCODE_Q)
                    {
                        shouldBreak = true;
                    }
                }
            }

            // update
            auto activeGameScene = GameSceneManager::Instance()->GetActiveGameScene();
#if GAME_EDITOR
            gameEditor->Tick();
#endif

            // rendering
            if (activeGameScene) renderPipeline->Render(activeGameScene);
#if GAME_EDITOR
            gameEditor->Render();
#endif
            gfxDriver->DispatchGPUWork();

            AssetDatabase::Instance()->EndOfFrameUpdate();
        }
    }

    WeilanEngine::~WeilanEngine()
    {
        gameEditor = nullptr;
        renderPipeline = nullptr;
        AssetDatabase::Deinit();
        Gfx::GfxDriver::DestroyGfxDriver();
    }

    void WeilanEngine::RegisterAssetImporters()
    {
        AssetImporter::RegisterImporter("glb", []() { return MakeUnique<Internal::glbImporter>(); });
        AssetImporter::RegisterImporter("mat", []() { return MakeUnique<Internal::GeneralImporter<Material>>(); });
        AssetImporter::RegisterImporter("game", []() { return MakeUnique<Internal::GeneralImporter<GameScene>>(); });
        AssetImporter::RegisterImporter("shad", []() { return MakeUnique<Internal::ShaderImporter>(); });
        AssetImporter::RegisterImporter("png", []() { return MakeUnique<Internal::TextureImporter>(); });
        AssetImporter::RegisterImporter("jpeg", []() { return MakeUnique<Internal::TextureImporter>(); });
        AssetImporter::RegisterImporter("jpg", []() { return MakeUnique<Internal::TextureImporter>(); });
    }

    void WeilanEngine::ConfigureProjectPath()
    {
#if GAME_EDITOR
        gameEditor->ConfigEditorPath();
#endif
    }
}
