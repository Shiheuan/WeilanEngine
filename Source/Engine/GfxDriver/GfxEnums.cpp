#include "GfxEnums.hpp"

namespace Engine::Gfx
{
bool HasWriteAccessMask(AccessMaskFlags flags)
{
    if (HasFlag(flags, AccessMask::Shader_Write) || HasFlag(flags, AccessMask::Color_Attachment_Write) ||
        HasFlag(flags, AccessMask::Transfer_Write) || HasFlag(flags, AccessMask::Host_Write) ||
        HasFlag(flags, AccessMask::Memory_Write))
        return true;

    return false;
}

bool HasReadAccessMask(AccessMaskFlags flags)
{
    if (HasFlag(flags, AccessMask::Indirect_Command_Read) || HasFlag(flags, AccessMask::Index_Read) ||
        HasFlag(flags, AccessMask::Vertex_Attribute_Read) || HasFlag(flags, AccessMask::Uniform_Read) ||
        HasFlag(flags, AccessMask::Input_Attachment_Read) || HasFlag(flags, AccessMask::Shader_Read) ||
        HasFlag(flags, AccessMask::Color_Attachment_Read) ||
        HasFlag(flags, AccessMask::Depth_Stencil_Attachment_Read) || HasFlag(flags, AccessMask::Transfer_Read) ||
        HasFlag(flags, AccessMask::Host_Read) || HasFlag(flags, AccessMask::Memory_Read))
        return true;

    return false;
}
} // namespace Engine::Gfx

namespace Engine::Gfx::Utils
{

// https://registry.khronos.org/vulkan/specs/1.3-khr-extensions/html/chap40.html#formats-definition
uint32_t MapImageFormatToByteSize(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat::BC7_UNorm_Block: return 16;
        case ImageFormat::BC7_SRGB_UNorm_Block: return 16;
        case ImageFormat::BC3_Unorm_Block: return 16;
        case ImageFormat::BC3_SRGB_Block: return 16;
        case ImageFormat::R16G16B16A16_SFloat: return 8;
        case ImageFormat::R32G32B32A32_SFloat: return 16;
        case ImageFormat::R16G16B16A16_UNorm: return 8;
        case ImageFormat::R8G8B8A8_UNorm:
        case ImageFormat::B8G8R8A8_UNorm:
        case ImageFormat::B8G8R8A8_SRGB: return 4;
        case ImageFormat::R8G8B8A8_SRGB: return 4;
        case ImageFormat::R8G8B8_SRGB: return 3;
        case ImageFormat::R8G8_SRGB: return 2;
        case ImageFormat::R8_SRGB: return 1;
        case ImageFormat::D16_UNorm: return 2;
        case ImageFormat::D16_UNorm_S8_UInt: return 3;
        case ImageFormat::D24_UNorm_S8_UInt: return 4;
        default: assert(0 && "Not implemented");
    }

    return 64;
};

ImageFormat MapStringToImageFormat(std::string_view name)
{
    if (name == "R16G16B16A16_SFloat")
        return ImageFormat::R16G16B16A16_SFloat;
    else if (name == "R32G32B32A32_SFloat")
        return ImageFormat::R32G32B32A32_SFloat;
    else if (name == "R16G16B16A16_UNorm")
        return ImageFormat::R16G16B16A16_UNorm;
    else if (name == "R8G8B8A8_UNorm")
        return ImageFormat::R8G8B8A8_UNorm;
    else if (name == "B8G8R8A8_UNorm")
        return ImageFormat::B8G8R8A8_UNorm;
    else if (name == "B8G8R8A8_SRGB")
        return ImageFormat::B8G8R8A8_SRGB;
    else if (name == "R8G8B8A8_SRGB")
        return ImageFormat::R8G8B8A8_SRGB;
    else if (name == "R8G8B8_SRGB")
        return ImageFormat::R8G8B8_SRGB;
    else if (name == "R8G8_SRGB")
        return ImageFormat::R8G8_SRGB;
    else if (name == "R8_SRGB")
        return ImageFormat::R8_SRGB;
    else if (name == "D16_UNorm")
        return ImageFormat::D16_UNorm;
    else if (name == "D16_UNorm_S8_UInt")
        return ImageFormat::D16_UNorm_S8_UInt;
    else if (name == "D32_SFLOAT_S8_UInt")
        return ImageFormat::D32_SFLOAT_S8_UInt;
    else if (name == "D24_UNorm_S8_UInt")
        return ImageFormat::D24_UNorm_S8_UInt;
    else if (name == "BC7_UNorm_Block")
        return ImageFormat::BC7_UNorm_Block;
    else if (name == "BC7_SRGB_UNorm_Block")
        return ImageFormat::BC7_SRGB_UNorm_Block;
    else if (name == "BC3_Unorm_Block")
        return ImageFormat::BC3_Unorm_Block;
    else if (name == "BC3_SRGB_Block")
        return ImageFormat::BC3_SRGB_Block;

    return ImageFormat::Invalid;
}
} // namespace Engine::Gfx::Utils
