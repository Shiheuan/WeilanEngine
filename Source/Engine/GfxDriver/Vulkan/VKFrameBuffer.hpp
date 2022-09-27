#pragma once
#include "../FrameBuffer.hpp"
#include "Code/Ptr.hpp"
#include <vulkan/vulkan.h>
namespace Engine::Gfx
{
    class VKRenderPass;
    class VKImage;
    struct VKContext;
    class VKFrameBuffer : public FrameBuffer
    {
        public:
            VKFrameBuffer(RefPtr<VKContext> context, RefPtr<RenderPass> baseRenderPass);
            VKFrameBuffer(const VKFrameBuffer& other) = delete;
            ~VKFrameBuffer() override;

            void SetAttachments(const std::vector<RefPtr<Image>>& attachments) override;

            std::vector<RefPtr<VKImage>>& GetAttachments() { return attachments; };
            uint32_t GetWidth() {return width;}
            uint32_t GetHeight() {return height;}
            VkFramebuffer GetHandle();
        private:

            uint32_t width;
            uint32_t height;
            VkFramebuffer frameBuffer = VK_NULL_HANDLE;
            RefPtr<VKRenderPass> baseRenderPass;
            RefPtr<VKContext> context;
            
            std::vector<RefPtr<VKImage>> attachments;
            void CreateFrameBuffer();
    };
}
