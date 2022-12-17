#include "VKDriver.hpp"

#include "Internal/VKInstance.hpp"
#include "Internal/VKDevice.hpp"
#include "Internal/VKPhysicalDevice.hpp"
#include "Internal/VKAppWindow.hpp"
#include "Internal/VKSurface.hpp"
#include "Internal/VKEnumMapper.hpp"
#include "Internal/VKMemAllocator.hpp"
#include "Internal/VKSwapChain.hpp"
#include "Internal/VKObjectManager.hpp"
#include "VKShaderResource.hpp"
#include "VKBuffer.hpp"
#include "VKShaderModule.hpp"
#include "VKContext.hpp"
#include "VKFence.hpp"

#include "VKFrameBuffer.hpp"
#include "VKRenderPass.hpp"
#include "VKDescriptorPool.hpp"
#include "VKSharedResource.hpp"

#include <spdlog/spdlog.h>
#if !_MSC_VER
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wnullability-completeness"
#pragma GCC diagnostic ignored "-Wswitch"
#endif
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

#if !_MSC_VER
#pragma GCC diagnostic pop
#endif
#if defined(_WIN32) || defined(_WIN64)
#undef CreateSemaphore
#endif

namespace Engine::Gfx
{
    // I can't use the MakeUnique function in Ptr.hpp in this translation unit on Window machine. Not sure why.
    // This is a temporary workaround
    template<class T, class ...Args>
    UniPtr<T> MakeUnique1(Args&&... args)
    {
        return UniPtr<T>(new T(std::forward<Args>(args)...));
    }

    VKDriver::VKDriver()
    {
        context = MakeUnique1<VKContext>();
        VKContext::context = context;
        appWindow = new VKAppWindow();
        instance = new VKInstance(appWindow->GetVkRequiredExtensions());
        surface = new VKSurface(*instance, appWindow);
        VKDevice::QueueRequest queueRequest[] = 
        {
            {
                VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT | VK_QUEUE_COMPUTE_BIT,
                true,
                1
            },
            {
                VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT,
                false,
                1
            }
        };
        device = new VKDevice(instance, surface, queueRequest, sizeof(queueRequest) / sizeof(VKDevice::QueueRequest));
        context->device = device;
        mainQueue = &device->GetQueue(0);
        graphics0queue = &device->GetQueue(1);
        context->mainQueue = mainQueue;
        gpu = &device->GetGPU();
        device_vk = device->GetHandle();
        objectManager = new VKObjectManager(device_vk);
        context->objManager = objectManager;
        swapChainImageProxy = MakeUnique1<VKSwapChainImageProxy>();

        // Create other objects
        memAllocator = new VKMemAllocator(instance->GetHandle(), device, gpu->GetHandle(), mainQueue->queueFamilyIndex);
        context->allocator = memAllocator;
        sharedResource = MakeUnique1<VKSharedResource>(context);
        context->sharedResource = sharedResource;
        swapchain = new VKSwapChain(mainQueue->queueFamilyIndex, gpu, *surface);
        context->swapchain = swapchain;

        // just set a placeholder image so that others can use the information from the Image before the first frame
        swapChainImageProxy->SetActiveSwapChainImage(swapchain->GetSwapChainImage(0), 0);

        // descriptor pool cache
        descriptorPoolCache = MakeUnique1<VKDescriptorPoolCache>(context);
        context->descriptorPoolCache = descriptorPoolCache;
    }

    VKDriver::~VKDriver()
    {
        vkDeviceWaitIdle(device_vk);

        sharedResource = nullptr;

        descriptorPoolCache = nullptr;
        objectManager->DestroyPendingResources();

        delete swapchain;
        delete memAllocator;
        delete objectManager;
        delete surface;
        delete appWindow;
        delete device;
        delete instance;
    }

    Extent2D VKDriver::GetWindowSize()
    {
        return appWindow->GetDefaultWindowSize();
    }

    void VKDriver::WaitForIdle()
    {
        vkDeviceWaitIdle(device->GetHandle());
    }

    Backend VKDriver::GetGfxBackendType()
    {
        return Backend::Vulkan;
    }

    SDL_Window* VKDriver::GetSDLWindow()
    {
        return appWindow->GetSDLWindow();
    }

    RefPtr<Image> VKDriver::GetSwapChainImageProxy()
    {
        return swapChainImageProxy.Get();
    }

    void VKDriver::ForceSyncResources()
    {
        return; // TODO: reimplementation needed
    }

    void VKDriver::ExecuteCommandBuffer(UniPtr<CommandBuffer>&& cmdBuf)
    {
        VKCommandBuffer* vkCmdBuf = static_cast<VKCommandBuffer*>(cmdBuf.Get());
        cmdBuf.Release();
        pendingCmdBufs.emplace_back(vkCmdBuf);
    }

    RefPtr<Semaphore> VKDriver::Present(RefPtr<Semaphore>* semaphores, uint32_t semaphoreCount)
    {
        assert(semaphoreCount <= 16);
        VkSemaphore vkSemaphores[16];
        for(int i = 0; i < semaphoreCount; ++i)
        {
            vkSemaphores[i] = static_cast<VKSemaphore*>(semaphores[i].Get())->GetHandle();
        }

        uint32_t activeIndex = swapChainImageProxy->GetActiveIndex();
        VkSwapchainKHR swapChainHandle = swapchain->GetHandle();
        VkPresentInfoKHR presentInfo = {
            VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            nullptr,
            semaphoreCount,
            vkSemaphores,
            1,
            &swapChainHandle,
            &activeIndex,
            nullptr
        };
        vkQueuePresentKHR(mainQueue->queue, &presentInfo);

        return inFlightFrame.imageAcquireSemaphore.Get();
    }

    void VKDriver::AcquireNextSwapChainImage(RefPtr<Semaphore> imageAcquireSemaphore)
    {
        swapchain->AcquireNextImage(swapChainImageProxy, GetHandle());
    }

    UniPtr<Semaphore> VKDriver::CreateSemaphore(const Semaphore::CreateInfo& createInfo)
    {
        return MakeUnique1<VKSemaphore>(createInfo.signaled);
    }

    UniPtr<Fence> VKDriver::CreateFence(const Fence::CreateInfo& createInfo)
    {
        return MakeUnique1<VKFence>(createInfo);
    }

    UniPtr<Buffer> VKDriver::CreateBuffer(const Buffer::CreateInfo& createInfo)
    {
        return MakeUnique1<VKBuffer>(createInfo.size, createInfo.usages, createInfo.visibleInCPU);
    }

    UniPtr<ShaderResource> VKDriver::CreateShaderResource(RefPtr<ShaderProgram> shader, ShaderResourceFrequency frequency)
    {
        return MakeUnique1<VKShaderResource>(shader, frequency);
    }

    UniPtr<RenderPass> VKDriver::CreateRenderPass()
    {
        return MakeUnique1<VKRenderPass>();
    }

    UniPtr<FrameBuffer> VKDriver::CreateFrameBuffer(RefPtr<RenderPass> renderPass)
    {
        return MakeUnique1<VKFrameBuffer>(renderPass);
    }

    UniPtr<CommandBuffer> VKDriver::CreateCommandBuffer()
    {
        return MakeUnique1<VKCommandBuffer>();
    }

    UniPtr<Image> VKDriver::CreateImage(const ImageDescription& description, ImageUsageFlags usages)
    {
        return MakeUnique1<VKImage>(description, usages);
    }
    UniPtr<ShaderProgram> VKDriver::CreateShaderProgram(const std::string& name, 
            const ShaderConfig* config,
            unsigned char* vert,
            uint32_t vertSize,
            unsigned char* frag,
            uint32_t fragSize)
    {
        return MakeUnique1<VKShaderProgram>(config, context, name, vert, vertSize, frag, fragSize);
    }

    RefPtr<CommandQueue> VKDriver::GetQueue(QueueType type)
    {
        switch (type) {
            case Engine::QueueType::Main: return const_cast<VKCommandQueue*>(mainQueue.Get());
            case Engine::QueueType::Graphics0: return const_cast<VKCommandQueue*>(graphics0queue.Get());
        }
    }

    void VKDriver::QueueSubmit(RefPtr<CommandQueue> queue,
            std::vector<RefPtr<CommandBuffer>>& cmdBufs,
            std::vector<RefPtr<Semaphore>>& waitSemaphores,
            std::vector<Gfx::PipelineStageFlags>& waitDstStageMasks,
            std::vector<RefPtr<Semaphore>>& signalSemaphroes,
            RefPtr<Fence> signalFence)
    {
        std::vector<VkSemaphore> vkWaitSemaphores;
        std::vector<VkSemaphore> vkSignalSemaphores;
        std::vector<VkPipelineStageFlags> vkPipelineStageFlags;
        std::vector<VkCommandBuffer> vkCmdBufs;

        for(auto w : waitSemaphores)
        {
            auto vkWaitSemaphore = static_cast<VKSemaphore*>(w.Get());
            vkWaitSemaphores.push_back(vkWaitSemaphore->GetHandle());
        }

        for(auto s : signalSemaphroes)
        {
            auto vkSignalSemaphore = static_cast<VKSemaphore*>(s.Get());
            vkSignalSemaphores.push_back(vkSignalSemaphore->GetHandle());
        }

        for(auto p : waitDstStageMasks)
        {
            vkPipelineStageFlags.push_back(MapPipelineStage(p));
        }

        for(auto c : cmdBufs)
        {
            vkCmdBufs.push_back(static_cast<VKCommandBuffer*>(c.Get())->GetHandle());
        }

        auto vkqueue = static_cast<VKCommandQueue*>(queue.Get());

        VkSubmitInfo submitInfo;
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.pNext = VK_NULL_HANDLE;
        submitInfo.waitSemaphoreCount = vkWaitSemaphores.size();
        submitInfo.pWaitSemaphores = vkWaitSemaphores.data();
        submitInfo.pWaitDstStageMask = vkPipelineStageFlags.data();
        submitInfo.commandBufferCount = vkCmdBufs.size();
        submitInfo.pCommandBuffers = vkCmdBufs.data();
        submitInfo.signalSemaphoreCount = vkSignalSemaphores.size();
        submitInfo.pSignalSemaphores = vkSignalSemaphores.data();

        VkFence fence = signalFence == nullptr ? VK_NULL_HANDLE : static_cast<VKFence*>(signalFence.Get())->GetHandle();

        vkQueueSubmit(vkqueue->queue, 1, &submitInfo, fence);
    }
}
