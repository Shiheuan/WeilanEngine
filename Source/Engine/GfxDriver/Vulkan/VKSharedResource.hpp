#pragma once

#include "Code/Ptr.hpp"
#include <vulkan/vulkan.h>

namespace Engine::Gfx
{
    class VKContext;
    class VKImage;
    class VKSharedResource
    {
        public:
            VKSharedResource(RefPtr<VKContext> context);
            ~VKSharedResource();
            VkSampler GetDefaultSampler() {return defaultSampler; }
            RefPtr<VKImage> GetDefaultTexture() {return defaultTexture; }


        private:
            RefPtr<VKContext> context;

            VkSampler defaultSampler;
            UniPtr<VKImage> defaultTexture;
    };
}
