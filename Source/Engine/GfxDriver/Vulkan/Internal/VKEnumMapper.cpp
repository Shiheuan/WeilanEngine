#include "VKEnumMapper.hpp"
#include <spdlog/spdlog.h>

namespace Engine::Gfx
{
    VkFormat MapFormat(ImageFormat format)
    {
        switch(format)
        {
            case ImageFormat::R16G16B16A16_SFloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
            case ImageFormat::D16_UNorm: return VK_FORMAT_D16_UNORM;
            case ImageFormat::D16_UNorm_S8_UInt: return VK_FORMAT_D16_UNORM_S8_UINT;
            case ImageFormat::D24_UNorm_S8_UInt: return VK_FORMAT_D24_UNORM_S8_UINT;
            case ImageFormat::B8G8R8A8_UNorm: return VK_FORMAT_B8G8R8A8_UNORM;
            case ImageFormat::B8G8R8A8_SRGB: return VK_FORMAT_B8G8R8A8_SRGB;
            case ImageFormat::R8G8B8A8_UNorm: return VK_FORMAT_R8G8B8A8_UNORM;
            case ImageFormat::R8G8B8A8_SRGB: return VK_FORMAT_R8G8B8A8_SRGB;
            case ImageFormat::R8G8B8_SRGB: return VK_FORMAT_R8G8B8_SRGB;
            case ImageFormat::R8G8_SRGB: return VK_FORMAT_R8G8_SRGB;
            case ImageFormat::R8_SRGB: return VK_FORMAT_R8_SRGB;
            default: assert(0 && "Format map failed");
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    }

    ImageFormat MapVKFormat(VkFormat format)
    {
        switch(format)
        {
            case VK_FORMAT_R16G16B16A16_SFLOAT: return ImageFormat::R16G16B16A16_SFloat;
            case VK_FORMAT_D16_UNORM: return ImageFormat::D16_UNorm;
            case VK_FORMAT_D16_UNORM_S8_UINT: return ImageFormat::D16_UNorm_S8_UInt;
            case VK_FORMAT_D24_UNORM_S8_UINT : return ImageFormat::D24_UNorm_S8_UInt;
            case VK_FORMAT_B8G8R8A8_UNORM: return ImageFormat::B8G8R8A8_UNorm;
            case VK_FORMAT_B8G8R8A8_SRGB: return ImageFormat::B8G8R8A8_SRGB;
            case VK_FORMAT_R8G8B8A8_UNORM: return ImageFormat::R8G8B8A8_UNorm;
            case VK_FORMAT_R8G8B8A8_SRGB: return ImageFormat::R8G8B8A8_SRGB;
            case VK_FORMAT_R8G8B8_SRGB: return ImageFormat::R8G8B8_SRGB;
            case VK_FORMAT_R8G8_SRGB: return ImageFormat::R8G8_SRGB;
            case VK_FORMAT_R8_SRGB: return ImageFormat::R8_SRGB;
            default: assert(0 && "VK format map failed");
        }

        return ImageFormat::R16G16B16A16_SFloat;
    }

    VkAttachmentLoadOp MapAttachmentLoadOp(AttachmentLoadOperation loadOp)
    {
        switch (loadOp) {
            case AttachmentLoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
            case AttachmentLoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
            case AttachmentLoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            default: return VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    }

    VkAttachmentStoreOp MapAttachmentStoreOp(AttachmentStoreOperation storeOp)
    {
        switch (storeOp) {
            case AttachmentStoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
            case AttachmentStoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
            default: return VK_ATTACHMENT_STORE_OP_STORE;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_ATTACHMENT_STORE_OP_STORE;
    }

    VkSampleCountFlagBits MapSampleCount(MultiSampling multiSampling)
    {
        switch(multiSampling)
        {
            case MultiSampling::Sample_Count_1: return VK_SAMPLE_COUNT_1_BIT;
            case MultiSampling::Sample_Count_2: return VK_SAMPLE_COUNT_2_BIT;
            case MultiSampling::Sample_Count_4: return VK_SAMPLE_COUNT_4_BIT;
            case MultiSampling::Sample_Count_8: return VK_SAMPLE_COUNT_8_BIT;
            case MultiSampling::Sample_Count_16: return VK_SAMPLE_COUNT_16_BIT;
            case MultiSampling::Sample_Count_32: return VK_SAMPLE_COUNT_32_BIT;
            case MultiSampling::Sample_Count_64: return VK_SAMPLE_COUNT_64_BIT;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_SAMPLE_COUNT_1_BIT;
    }

    VkCullModeFlags MapCullMode(CullMode cullMode)
    {
        switch (cullMode) {
            case CullMode::None: return VK_CULL_MODE_NONE;
            case CullMode::Back: return VK_CULL_MODE_BACK_BIT;
            case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Both: return VK_CULL_MODE_FRONT_AND_BACK;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_CULL_MODE_NONE;
    }

    VkCompareOp MapCompareOp(CompareOp cmp)
    {
        switch (cmp) {
            case CompareOp::Never: return VK_COMPARE_OP_NEVER;
            case CompareOp::Less: return VK_COMPARE_OP_LESS;
            case CompareOp::Less_or_Equal: return VK_COMPARE_OP_LESS_OR_EQUAL;
            case CompareOp::Greater: return VK_COMPARE_OP_GREATER;
            case CompareOp::Not_Equal: return VK_COMPARE_OP_NOT_EQUAL;
            case CompareOp::Greater_Or_Equal: return VK_COMPARE_OP_GREATER_OR_EQUAL;
            case CompareOp::Always: return VK_COMPARE_OP_ALWAYS;
            case CompareOp::Equal: return VK_COMPARE_OP_EQUAL;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_COMPARE_OP_ALWAYS;
    }

    VkStencilOp MapStencilOp(StencilOp op)
    {
        switch(op)
        {
            case StencilOp::Keep: return VK_STENCIL_OP_KEEP;
            case StencilOp::Zero: return VK_STENCIL_OP_ZERO;
            case StencilOp::Replace: return VK_STENCIL_OP_REPLACE;
            case StencilOp::Increment_And_Clamp: return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
            case StencilOp::Decrement_And_Clamp: return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
            case StencilOp::Invert: return VK_STENCIL_OP_INVERT;
            case StencilOp::Increment_And_Wrap: return VK_STENCIL_OP_INCREMENT_AND_WRAP;
            case StencilOp::Decrement_And_Wrap: return VK_STENCIL_OP_DECREMENT_AND_WRAP;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_STENCIL_OP_KEEP;
    }

    VkBlendFactor MapBlendFactor(BlendFactor bf)
    {
        switch (bf)
        {
            case BlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
            case BlendFactor::One: return VK_BLEND_FACTOR_ONE;
            case BlendFactor::Src_Color: return VK_BLEND_FACTOR_SRC_COLOR;
            case BlendFactor::One_Minus_Src_Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
            case BlendFactor::Dst_Color: return VK_BLEND_FACTOR_DST_COLOR;
            case BlendFactor::One_Minus_Dst_Color: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
            case BlendFactor::Src_Alpha: return VK_BLEND_FACTOR_SRC_ALPHA;
            case BlendFactor::One_Minus_Src_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
            case BlendFactor::Dst_Alpha: return VK_BLEND_FACTOR_DST_ALPHA;
            case BlendFactor::One_Minus_Dst_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
            case BlendFactor::Constant_Color: return VK_BLEND_FACTOR_CONSTANT_COLOR;
            case BlendFactor::One_Minus_Constant_Color: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
            case BlendFactor::Constant_Alpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
            case BlendFactor::One_Minus_Constant_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
            case BlendFactor::Src_Alpha_Saturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
            case BlendFactor::Src1_Color: return VK_BLEND_FACTOR_SRC1_COLOR;
            case BlendFactor::One_Minus_Src1_Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
            case BlendFactor::Src1_Alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
            case BlendFactor::One_Minus_Src1_Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_BLEND_FACTOR_ZERO;
    }

    VkImageUsageFlags MapImageUsage(ImageUsageFlags in)
    {
        VkImageUsageFlags flags = 0;
        if (in & ImageUsage::ColorAttachment) flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        if (in & ImageUsage::DepthStencilAttachment) flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (in & ImageUsage::Texture) flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (in & ImageUsage::TransferSrc) flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (in & ImageUsage::TransferDst) flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;

        return flags;
    }

    VkBlendOp MapBlendOp(BlendOp op)
    {
        switch (op)
        {
            case BlendOp::Add: return VK_BLEND_OP_ADD;
            case BlendOp::Subtract: return VK_BLEND_OP_SUBTRACT;
            case BlendOp::Reverse_Subtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
            case BlendOp::Min: return VK_BLEND_OP_MIN;
            case BlendOp::Max: return VK_BLEND_OP_MAX;
        }

        SPDLOG_WARN("VKEnum map failed");
        return VK_BLEND_OP_ADD;
    }

    VkColorComponentFlagBits MapColorComponentBits(ColorComponentBits bits)
    {
        return (VkColorComponentFlagBits)bits;

        SPDLOG_WARN("VKEnum map failed");
        return VK_COLOR_COMPONENT_R_BIT;
    }

    VkPipelineStageFlags MapPipelineStage(PipelineStageFlags stages)
    {
        VkPipelineStageFlags flags = 0;

        if (HasFlag(stages, PipelineStage::Top_Of_Pipe)) { flags |= VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT; }
        if (HasFlag(stages, PipelineStage::Draw_Indirect)) { flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT; }
        if (HasFlag(stages, PipelineStage::Vertex_Input)) { flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT; }
        if (HasFlag(stages, PipelineStage::Vertex_Shader)) { flags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT; }
        if (HasFlag(stages, PipelineStage::Tessellation_Control_Shader)) { flags |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT; }
        if (HasFlag(stages, PipelineStage::Tessellation_Evaluation_Shader)) { flags |= VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT; }
        if (HasFlag(stages, PipelineStage::Geometry_Shader)) { flags |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT; }
        if (HasFlag(stages, PipelineStage::Fragment_Shader)) { flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT; }
        if (HasFlag(stages, PipelineStage::Early_Fragment_Tests)) { flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT; }
        if (HasFlag(stages, PipelineStage::Late_Fragment_Tests)) { flags |=VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;  }
        if (HasFlag(stages, PipelineStage::Color_Attachment_Output)) { flags |=VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT ; }
        if (HasFlag(stages, PipelineStage::Compute_Shader)) { flags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT; }
        if (HasFlag(stages, PipelineStage::Transfer)) { flags |= VK_PIPELINE_STAGE_TRANSFER_BIT; }
        if (HasFlag(stages, PipelineStage::Bottom_Of_Pipe)) { flags |= VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT; }
        if (HasFlag(stages, PipelineStage::Host)) { flags |= VK_PIPELINE_STAGE_HOST_BIT; }
        if (HasFlag(stages, PipelineStage::All_Graphics)) { flags |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT; }
        if (HasFlag(stages, PipelineStage::All_Commands)) { flags |= VK_PIPELINE_STAGE_ALL_COMMANDS_BIT; }

        return flags;
    }

    VkAccessFlags MapAccessMask(AccessMaskFlags masks)
    {
        VkAccessFlags flags = 0;

        if (HasFlag(masks, AccessMask::Indirect_Command_Read)) { flags |= VK_ACCESS_INDIRECT_COMMAND_READ_BIT; }
        if (HasFlag(masks, AccessMask::Index_Read)) { flags |= VK_ACCESS_INDEX_READ_BIT; }
        if (HasFlag(masks, AccessMask::Vertex_Attribute_Read)) { flags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT; }
        if (HasFlag(masks, AccessMask::Uniform_Read)) { flags |= VK_ACCESS_UNIFORM_READ_BIT; }
        if (HasFlag(masks, AccessMask::Input_Attachment_Read)) { flags |= VK_ACCESS_INPUT_ATTACHMENT_READ_BIT; }
        if (HasFlag(masks, AccessMask::Shader_Read)) { flags |= VK_ACCESS_SHADER_READ_BIT; }
        if (HasFlag(masks, AccessMask::Shader_Write)) { flags |= VK_ACCESS_SHADER_WRITE_BIT; }
        if (HasFlag(masks, AccessMask::Color_Attachment_Read)) { flags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT; }
        if (HasFlag(masks, AccessMask::Color_Attachment_Write)) { flags |= VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; }
        if (HasFlag(masks, AccessMask::Depth_Stencil_Attachment_Read)) { flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT; }
        if (HasFlag(masks, AccessMask::Depth_Stencil_Attachment_Write)) { flags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT; }
        if (HasFlag(masks, AccessMask::Transfer_Read)) { flags |= VK_ACCESS_TRANSFER_READ_BIT; }
        if (HasFlag(masks, AccessMask::Transfer_Write)) { flags |= VK_ACCESS_TRANSFER_WRITE_BIT; }
        if (HasFlag(masks, AccessMask::Host_Read)) { flags |= VK_ACCESS_HOST_READ_BIT; }
        if (HasFlag(masks, AccessMask::Host_Write)) { flags |= VK_ACCESS_HOST_WRITE_BIT; }
        if (HasFlag(masks, AccessMask::Memory_Read)) { flags |= VK_ACCESS_MEMORY_READ_BIT; }
        if (HasFlag(masks, AccessMask::Memory_Write)) { flags |= VK_ACCESS_MEMORY_WRITE_BIT; }

        return flags;
    }

    VkImageLayout MapImageLayout(ImageLayout layout)
    {
        if (layout == ImageLayout::Undefined) return VK_IMAGE_LAYOUT_UNDEFINED;
        if (layout == ImageLayout::General) return VK_IMAGE_LAYOUT_GENERAL;
        if (layout == ImageLayout::Color_Attachment) return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        if (layout == ImageLayout::Depth_Stencil_Attachment) return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        if (layout == ImageLayout::Depth_Stencil_Read_Only) return VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        if (layout == ImageLayout::Shader_Read_Only) return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        if (layout == ImageLayout::Transfer_Src) return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        if (layout == ImageLayout::Transfer_Dst) return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        if (layout == ImageLayout::Preinitialized) return VK_IMAGE_LAYOUT_PREINITIALIZED;

        if (layout == ImageLayout::Depth_Read_Only_Stencil_Attachment) return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
        if (layout == ImageLayout::Depth_Attachment_Stencil_Read_Only) return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
        if (layout == ImageLayout::Depth_Attachment) return VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        if (layout == ImageLayout::Depth_Read_Only) return VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
        if (layout == ImageLayout::Stencil_Attachment) return VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
        if (layout == ImageLayout::Stencil_Read_Only) return VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
        if (layout == ImageLayout::Read_Only) return VK_IMAGE_LAYOUT_READ_ONLY_OPTIMAL;
        if (layout == ImageLayout::Attachment) return VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL;
        if (layout == ImageLayout::Present_Src_Khr) return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        return VK_IMAGE_LAYOUT_GENERAL;
    }

    VkImageAspectFlags MapImageAspect(ImageAspectFlags aspects)
    {
        VkImageAspectFlags flags = 0;

        if (HasFlag(aspects, ImageAspect::Color)) { flags |= VK_IMAGE_ASPECT_COLOR_BIT; }
        if (HasFlag(aspects, ImageAspect::Depth)) { flags |= VK_IMAGE_ASPECT_DEPTH_BIT; }
        if (HasFlag(aspects, ImageAspect::Stencil)) { flags |= VK_IMAGE_ASPECT_STENCIL_BIT; }
        if (HasFlag(aspects, ImageAspect::Metadata)) { flags |= VK_IMAGE_ASPECT_METADATA_BIT; }
        if (HasFlag(aspects, ImageAspect::Memory_Plane_0)) { flags |= VK_IMAGE_ASPECT_PLANE_0_BIT; }
        if (HasFlag(aspects, ImageAspect::Memory_Plane_1)) { flags |= VK_IMAGE_ASPECT_PLANE_1_BIT; }
        if (HasFlag(aspects, ImageAspect::Memory_Plane_2)) { flags |= VK_IMAGE_ASPECT_PLANE_2_BIT; }
        if (HasFlag(aspects, ImageAspect::Memory_Plane_3)) { flags |= VK_IMAGE_ASPECT_NONE; }

        return flags;
    }
}
