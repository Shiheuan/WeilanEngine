#pragma once

#include "Core/GameScene/GameScene.hpp"
#include "GfxDriver/GfxDriver.hpp"
#include "Window/SceneTreeWindow.hpp"
#include "Window/InspectorWindow.hpp"
#include "Window/ProjectWindow.hpp"
#include "Window/AssetExplorer.hpp"
#include "Window/GameSceneWindow.hpp"
#include "Window/ProjectManagementWindow.hpp"
#include "Core/AssetDatabase/AssetDatabase.hpp"
#include "Core/GameObject.hpp"
#include "Core/GameScene/GameScene.hpp"
#include "Core/Rendering/RenderPipeline.hpp"
#include "ProjectManagement/ProjectManagement.hpp"
#include "ThirdParty/imgui/imgui.h"
#include <SDL2/SDL.h>
#include "Code/Ptr.hpp"
#include <memory>
namespace Engine::Editor
{
    class GameEditor
    {
        public:
            GameEditor(RefPtr<Gfx::GfxDriver> gfxDriver);
            ~GameEditor();

            void Init(RefPtr<Rendering::RenderPipeline> renderPipeline);
            void ProcessEvent(const SDL_Event& event);
            bool IsProjectInitialized();
            void ShowProjectsWindow();
            void Tick();
            void Render();
        private:
            /* Data */
            UniPtr<ProjectManagement> projectManagement;
            AssetDatabase::OnAssetReloadIterHandle assetReloadIterHandle;

            /* Windows */
            UniPtr<EditorContext> editorContext;
            UniPtr<SceneTreeWindow> sceneTreeWindow;
            UniPtr<ProjectWindow> projectWindow;
            UniPtr<AssetExplorer> assetExplorer;
            UniPtr<InspectorWindow> inspector;
            UniPtr<GameSceneWindow> gameSceneWindow;
            UniPtr<ProjectManagementWindow> projectManagementWindow;

            /* Rendering Structures */
            RefPtr<Gfx::GfxDriver> gfxDriver;
            UniPtr<Gfx::RenderPass> editorPass;
            RefPtr<Gfx::Image> gameColorImage;
            RefPtr<Gfx::Image> gameDepthImage;
            void DrawMainMenu();
            void RenderEditor(RefPtr<CommandBuffer> cmdBuf);
            void RenderSceneGUI(RefPtr<CommandBuffer> cmdBuf);
            void InitRendering();

            struct ImGuiData
            {
                UniPtr<Gfx::GfxBuffer> vertexBuffer; // use single buffer to capture all the vertices
                UniPtr<Gfx::GfxBuffer> indexBuffer;
                UniPtr<Gfx::Image> fontTex;
                UniPtr<Gfx::Image> editorRT;
                UniPtr<Gfx::ShaderResource> generalShaderRes;
                RefPtr<Gfx::ShaderProgram> shaderProgram;
                Gfx::ShaderConfig shaderConfig;

                RefPtr<Gfx::ShaderResource> GetImageResource();
                void ResetImageResource() { currCacheIndex = -1; }
                void ClearImageResource();

                private:
                int currCacheIndex = -1;
                std::vector<UniPtr<Gfx::ShaderResource>> imageResourcesCache;
            };

            ImGuiData imGuiData;

            UniPtr<Gfx::ShaderResource> res;
            UniPtr<Gfx::RenderPass> renderPass;
    };
}
