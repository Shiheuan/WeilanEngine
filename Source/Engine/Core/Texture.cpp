#include "Texture.hpp"
#include "GfxDriver/GfxEnums.hpp"
#include "GfxDriver/Vulkan/Internal/VKEnumMapper.hpp"
#include "Rendering/GfxResourceTransfer.hpp"
#include "ThirdParty/stb/stb_image.h"
#include <ktxvulkan.h>

namespace Engine
{
Texture::Texture(TextureDescription texDesc, const UUID& uuid) : AssetObject(uuid), desc(texDesc)
{
    image =
        Gfx::GfxDriver::Instance()->CreateImage(texDesc.img, Gfx::ImageUsage::Texture | Gfx::ImageUsage::TransferDst);
    image->SetName(uuid.ToString());
    Internal::GfxResourceTransfer::ImageTransferRequest request{
        .data = texDesc.data,
        .size = Gfx::Utils::MapImageFormatToByteSize(texDesc.img.format) * texDesc.img.width * texDesc.img.height,
        .keepData = texDesc.keepData
        // use default subresource layer
    };
    Internal::GetGfxResourceTransfer()->Transfer(image, request);
}

bool IsKTX2File(ktx_uint8_t* imageData)
{
    return imageData[0] == 0xAB && imageData[1] == 0x4B && imageData[2] == 0x54 && imageData[3] == 0x58 &&
           imageData[4] == 0x20 && imageData[5] == 0x32 && imageData[6] == 0x30 && imageData[7] == 0xBB &&
           imageData[8] == 0x0D && imageData[9] == 0x0A && imageData[10] == 0x1A && imageData[11] == 0x0A;
}

bool IsKTX1File(ktx_uint8_t* imageData)
{
    return imageData[0] == 0xAB && imageData[1] == 0x4B && imageData[2] == 0x54 && imageData[3] == 0x58 &&
           imageData[4] == 0x20 && imageData[5] == 0x31 && imageData[6] == 0x31 && imageData[7] == 0xBB &&
           imageData[8] == 0x0D && imageData[9] == 0x0A && imageData[10] == 0x1A && imageData[110] == 0x0A;
}

Texture::Texture(KtxTexture texDesc, const UUID& uuid) : AssetObject(uuid)
{
    ktx_uint8_t* imageData = texDesc.imageData;
    uint32_t imageByteSize = texDesc.byteSize;
    if (IsKTX1File(imageData) || IsKTX2File(imageData))
    {
        ktxTexture* texture;
        if (ktxTexture_CreateFromMemory((uint8_t*)imageData,
                                        imageByteSize,
                                        KTX_TEXTURE_CREATE_LOAD_IMAGE_DATA_BIT,
                                        &texture) != KTX_SUCCESS)
        {
            throw std::runtime_error("Texture-Failed to create texture from memory");
        }

        if (ktxTexture_NeedsTranscoding(texture))
        {
            char* writerScParams;
            uint32_t valueLen;
            enum class CompressionMode
            {
                UASTC,
                ETC1S
            } compressionMode;
            if (KTX_SUCCESS == ktxHashList_FindValue(&texture->kvDataHead,
                                                     KTX_WRITER_SCPARAMS_KEY,
                                                     &valueLen,
                                                     (void**)&writerScParams))
            {
                if (std::strstr(writerScParams, "--uastc"))
                    compressionMode = CompressionMode::UASTC;
                else
                    compressionMode = CompressionMode::ETC1S;
            }
            else
            {
                throw std::runtime_error("Texture-failed to create ktx texture");
            }

            ktx_texture_transcode_fmt_e tf;
            auto& gpuFeatures = GetGfxDriver()->GetGPUFeatures();
            switch (compressionMode)
            {
                case CompressionMode::ETC1S:
                    {
                        if (gpuFeatures.textureCompressionETC2)
                            tf = KTX_TTF_ETC2_RGBA;
                        else if (gpuFeatures.textureCompressionBC)
                            tf = KTX_TTF_BC3_RGBA;
                        else
                        {
                            std::string message =
                                "Vulkan implementation does not support any available transcode target.";
                            throw std::runtime_error(message);
                        }
                        break;
                    }
                case CompressionMode::UASTC:
                    {
                        if (gpuFeatures.textureCompressionASTC4x4)
                            tf = KTX_TTF_ASTC_4x4_RGBA;
                        else if (gpuFeatures.textureCompressionBC)
                            tf = KTX_TTF_BC7_RGBA;
                        else
                        {
                            std::string message =
                                "Vulkan implementation does not support any available transcode target.";
                            throw std::runtime_error(message);
                        }
                        break;
                    }
            }

            if (ktxTexture2_TranscodeBasis((ktxTexture2*)texture, tf, 0) != 0)
            {
                throw std::runtime_error("failed to transcode ktx texture");
            }
        }

        TextureDescription texDesc;
        texDesc.img.isCubemap = texture->isCubemap;
        texDesc.img.width = texture->baseWidth;
        texDesc.img.height = texture->baseHeight;
        texDesc.img.mipLevels = texture->numLevels;
        texDesc.img.format = Gfx::MapVKFormat(ktxTexture_GetVkFormat(texture));
        texDesc.data = nullptr;

        image = Gfx::GfxDriver::Instance()->CreateImage(texDesc.img,
                                                        Gfx::ImageUsage::Texture | Gfx::ImageUsage::TransferDst);

        ktx_uint8_t* data = ktxTexture_GetData(texture);
        if (texture->numDimensions == 1)
        {
            throw std::runtime_error("Texture-numDimensions not implemented");
        }
        else if (texture->numDimensions == 2)
        {
            for (uint32_t level = 0; level < texture->numLevels; ++level)
            {
                for (uint32_t layer = 0; layer < texture->numLayers; ++layer)
                {
                    for (uint32_t face = 0; face < texture->numFaces; ++face)
                    {
                        // Retrieve a pointer to the image for a specific mip level, array layer
                        // & face or depth slice.
                        ktx_size_t offset = 0;
                        if (ktxTexture_GetImageOffset(texture, level, layer, face, &offset) != KTX_SUCCESS)
                            throw std::runtime_error("Texture-failed to get image offset");
                        uint32_t size = ktxTexture_GetImageSize(texture, level);

                        Gfx::ImageSubresourceLayers layers{
                            .aspectMask = Gfx::ImageAspectFlags::Color,
                            .mipLevel = level,
                            .baseArrayLayer = layer + face, // a multi-face texture (cubemap) can only have one layer
                            .layerCount = 1};
                        Internal::GfxResourceTransfer::ImageTransferRequest request{.data = data + offset,
                                                                                    .size = size,
                                                                                    .keepData = true,
                                                                                    .subresourceLayers = layers};
                        Internal::GetGfxResourceTransfer()->Transfer(image, request);
                    }
                }
            }
        }
        else
        {
            std::runtime_error("Texture-numDimensions not implemented");
        }
        // ...
        // Do something with the texture image.
        // ...
        ktxTexture_Destroy(texture); // https://github.khronos.org/KTX-Software/libktx/index.html#readktx

        this->desc = texDesc;
    }
}

void Texture::Reload(AssetObject&& loaded)
{
    Texture* newTex = static_cast<Texture*>(&loaded);
    image = std::move(newTex->image);
    AssetObject::Reload(std::move(loaded));
}

UniPtr<Engine::Texture> LoadTextureFromBinary(unsigned char* imageData, std::size_t byteSize)
{
    int width, height, channels, desiredChannels;
    stbi_info_from_memory(imageData, byteSize, &width, &height, &desiredChannels);
    if (desiredChannels == 3) // 3 channel srgb texture is not supported on PC
        desiredChannels = 4;

    stbi_uc* loaded = stbi_load_from_memory(imageData, (int)byteSize, &width, &height, &channels, desiredChannels);

    bool is16Bit = stbi_is_16_bit_from_memory(imageData, byteSize);
    bool isHDR = stbi_is_hdr_from_memory(imageData, byteSize);
    if (is16Bit && isHDR)
    {
        SPDLOG_ERROR("16 bits and hdr texture Not Implemented");
    }

    Engine::TextureDescription desc;
    desc.img.width = width;
    desc.img.height = height;
    desc.data = loaded;

    if (desiredChannels == 4)
    {
        desc.img.format = Engine::Gfx::ImageFormat::R8G8B8A8_SRGB;
    }
    if (desiredChannels == 3)
    {
        desc.img.format = Engine::Gfx::ImageFormat::R8G8B8_SRGB;
    }
    if (desiredChannels == 2)
    {
        desc.img.format = Engine::Gfx::ImageFormat::R8G8_SRGB;
    }
    if (desiredChannels == 1)
    {
        desc.img.format = Engine::Gfx::ImageFormat::R8_SRGB;
    }
    return MakeUnique<Engine::Texture>(desc);
}
} // namespace Engine
