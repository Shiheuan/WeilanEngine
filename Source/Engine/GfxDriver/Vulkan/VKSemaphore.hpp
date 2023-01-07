#pragma once
#include "../Semaphore.hpp"
#include "VKContext.hpp"
#include <vulkan/vulkan.h>

namespace Engine::Gfx
{
    class VKSemaphore : public Semaphore
    {
        public:
            VKSemaphore(bool signaled)
            {
                VkSemaphoreCreateInfo createInfo;
                createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
                createInfo.pNext = VK_NULL_HANDLE;
                createInfo.flags = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

                vkCreateSemaphore(GetDevice()->GetHandle(), &createInfo, VK_NULL_HANDLE, &vkSemaphore);
            }
            ~VKSemaphore() override
            {
                vkDestroySemaphore(GetDevice()->GetHandle(), vkSemaphore, VK_NULL_HANDLE);
            };

            inline VkSemaphore GetHandle() { return vkSemaphore; }

        private:
            VkSemaphore vkSemaphore;
    };
}