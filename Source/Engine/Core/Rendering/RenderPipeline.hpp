#pragma once

#include "Code/Ptr.hpp"
#include "Core/GameScene/GameScene.hpp"
#include "Core/Component/Transform.hpp"
#include "GfxDriver/GfxDriver.hpp"
#include "Core/Graphics/FrameContext.hpp"
#include "Core/AssetDatabase/AssetDatabase.hpp"
namespace Engine
{
    class GameScene;
}
namespace Engine::Rendering
{
    class RenderPipeline
    {
        public:
            RenderPipeline(RefPtr<Gfx::GfxDriver> gfxDriver);
            void Init();
            ~RenderPipeline();

            void Render(RefPtr<GameScene> gameScene);
            inline void SetOffScreenOutput(bool offscreen) { offscreenOutput = offscreen; }
            RefPtr<Gfx::Image> GetOutputColor();
            RefPtr<Gfx::Image> GetOutputDepth();

        private:

            RefPtr<Gfx::GfxDriver> gfxDriver;

            void RenderObject(RefPtr<Transform> transform, UniPtr<CommandBuffer>& cmd);
            FrameContext* frameContext;

            UniPtr<Gfx::RenderPass> renderPass;
            UniPtr<Gfx::Image> colorImage;
            UniPtr<Gfx::Image> depthImage;
            UniPtr<Gfx::ShaderResource> globalResource;
            bool offscreenOutput;
            AssetDatabase::OnAssetReloadIterHandle assetReloadIterHandle;

            void ProcessLights(RefPtr<GameScene> gameScene, RefPtr<CommandBuffer> cmdBuf);


    };
}
