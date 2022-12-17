#pragma once

#include "Code/Ptr.hpp"
#include "Code/EnumFlags.hpp"
#include "CommandBuffer.hpp"
#include "Image.hpp"
#include "CommandQueue.hpp"
#include "CommandPool.hpp"
#include "Buffer.hpp"
#include "Semaphore.hpp"
#include "Fence.hpp"

#include <vector>
#include <memory>
#include <vulkan/vulkan.h>
#include <unordered_map>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

#if defined(_WIN32) || defined(_WIN64)
#undef CreateSemaphore
#endif

namespace Engine::Gfx
{
    enum class Backend
    {
        Vulkan, OpenGL
    };

    class GfxDriver
    {
        public:
            static RefPtr<GfxDriver> Instance() { return gfxDriver; }
            static void CreateGfxDriver(Backend backend);
            static void DestroyGfxDriver() { gfxDriver = nullptr; }

            virtual ~GfxDriver(){};


            virtual RefPtr<Image>        GetSwapChainImageProxy() = 0;
            virtual RefPtr<CommandQueue> GetQueue(QueueType flags) = 0;
            virtual SDL_Window*          GetSDLWindow() = 0;
            virtual Backend              GetGfxBackendType() = 0;
            virtual Extent2D             GetWindowSize() = 0;

            virtual UniPtr<CommandPool>    CreateCommandPool(const CommandPool::CreateInfo& createInfo) = 0;
            virtual UniPtr<Buffer>      CreateBuffer(const Buffer::CreateInfo& createInfo) = 0;
            virtual UniPtr<CommandBuffer>  CreateCommandBuffer() = 0;
            virtual UniPtr<ShaderResource> CreateShaderResource(RefPtr<ShaderProgram> shader, ShaderResourceFrequency frequency) = 0;
            virtual UniPtr<RenderPass>     CreateRenderPass() = 0;
            virtual UniPtr<FrameBuffer>    CreateFrameBuffer(RefPtr<RenderPass> renderPass) = 0;
            virtual UniPtr<Image>          CreateImage(const ImageDescription& description, ImageUsageFlags usages) = 0;
            virtual UniPtr<ShaderProgram>  CreateShaderProgram(
                    const std::string& name, 
                    const ShaderConfig* config,
                    unsigned char* vert,
                    uint32_t vertSize,
                    unsigned char* frag,
                    uint32_t fragSize) = 0;
            virtual UniPtr<Semaphore>      CreateSemaphore(const Semaphore::CreateInfo& createInfo) = 0;
            virtual UniPtr<Fence>          CreateFence(const Fence::CreateInfo& createInfo) = 0;

            virtual void PrepareFrameResources(RefPtr<CommandQueue> queue) = 0;
            virtual void QueueSubmit(
                    RefPtr<CommandQueue> queue,
                    std::vector<RefPtr<CommandBuffer>>& cmdBufs,
                    std::vector<RefPtr<Semaphore>>& waitSemaphores,
                    std::vector<Gfx::PipelineStageFlags>& waitDstStageMasks,
                    std::vector<RefPtr<Semaphore>>& signalSemaphroes,
                    RefPtr<Fence> signalFence) = 0;
            virtual void ForceSyncResources() = 0;
            virtual void WaitForIdle() = 0;
            virtual RefPtr<Semaphore> Present(std::vector<RefPtr<Semaphore>> semaphores) = 0;
            virtual void AcquireNextSwapChainImage(RefPtr<Semaphore> imageAcquireSemaphore);
            virtual void ExecuteCommandBuffer(UniPtr<CommandBuffer>&& cmdBuf) = 0;

        private:
            static UniPtr<GfxDriver> gfxDriver;
    };
}

namespace Engine
{
    inline RefPtr<Gfx::GfxDriver> GetGfxDriver() { return Gfx::GfxDriver::Instance(); }
}
