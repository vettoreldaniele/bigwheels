#include "ppx/grfx/vk/vk_util.h"

namespace ppx {
namespace grfx {
namespace vk {

const char* ToString(VkResult value)
{
    // clang-format off
    switch (value) {
        default: break;
        case VK_SUCCESS                                            : return "VK_SUCCESS"; break;
        case VK_NOT_READY                                          : return "VK_NOT_READY"; break;
        case VK_TIMEOUT                                            : return "VK_TIMEOUT"; break;
        case VK_EVENT_SET                                          : return "VK_EVENT_SET"; break;
        case VK_EVENT_RESET                                        : return "VK_EVENT_RESET"; break;
        case VK_INCOMPLETE                                         : return "VK_INCOMPLETE"; break;
        case VK_ERROR_OUT_OF_HOST_MEMORY                           : return "VK_ERROR_OUT_OF_HOST_MEMORY"; break;
        case VK_ERROR_OUT_OF_DEVICE_MEMORY                         : return "VK_ERROR_OUT_OF_DEVICE_MEMORY"; break;
        case VK_ERROR_INITIALIZATION_FAILED                        : return "VK_ERROR_INITIALIZATION_FAILED"; break;
        case VK_ERROR_DEVICE_LOST                                  : return "VK_ERROR_DEVICE_LOST"; break;
        case VK_ERROR_MEMORY_MAP_FAILED                            : return "VK_ERROR_MEMORY_MAP_FAILED"; break;
        case VK_ERROR_LAYER_NOT_PRESENT                            : return "VK_ERROR_LAYER_NOT_PRESENT"; break;
        case VK_ERROR_EXTENSION_NOT_PRESENT                        : return "VK_ERROR_EXTENSION_NOT_PRESENT"; break;
        case VK_ERROR_FEATURE_NOT_PRESENT                          : return "VK_ERROR_FEATURE_NOT_PRESENT"; break;
        case VK_ERROR_INCOMPATIBLE_DRIVER                          : return "VK_ERROR_INCOMPATIBLE_DRIVER"; break;
        case VK_ERROR_TOO_MANY_OBJECTS                             : return "VK_ERROR_TOO_MANY_OBJECTS"; break;
        case VK_ERROR_FORMAT_NOT_SUPPORTED                         : return "VK_ERROR_FORMAT_NOT_SUPPORTED"; break;
        case VK_ERROR_FRAGMENTED_POOL                              : return "VK_ERROR_FRAGMENTED_POOL"; break;
        case VK_ERROR_UNKNOWN                                      : return "VK_ERROR_UNKNOWN"; break;
        case VK_ERROR_OUT_OF_POOL_MEMORY                           : return "VK_ERROR_OUT_OF_POOL_MEMORY"; break;
        case VK_ERROR_INVALID_EXTERNAL_HANDLE                      : return "VK_ERROR_INVALID_EXTERNAL_HANDLE"; break;
        case VK_ERROR_FRAGMENTATION                                : return "VK_ERROR_FRAGMENTATION"; break;
        case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS               : return "VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS"; break;
        case VK_ERROR_SURFACE_LOST_KHR                             : return "VK_ERROR_SURFACE_LOST_KHR"; break;
        case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR                     : return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR"; break;
        case VK_SUBOPTIMAL_KHR                                     : return "VK_SUBOPTIMAL_KHR"; break;
        case VK_ERROR_OUT_OF_DATE_KHR                              : return "VK_ERROR_OUT_OF_DATE_KHR"; break;
        case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR                     : return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR"; break;
        case VK_ERROR_VALIDATION_FAILED_EXT                        : return "VK_ERROR_VALIDATION_FAILED_EXT"; break;
        case VK_ERROR_INVALID_SHADER_NV                            : return "VK_ERROR_INVALID_SHADER_NV"; break;
        case VK_ERROR_INCOMPATIBLE_VERSION_KHR                     : return "VK_ERROR_INCOMPATIBLE_VERSION_KHR"; break;
        case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT : return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT"; break;
        case VK_ERROR_NOT_PERMITTED_EXT                            : return "VK_ERROR_NOT_PERMITTED_EXT"; break;
        case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT          : return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT"; break;
        case VK_THREAD_IDLE_KHR                                    : return "VK_THREAD_IDLE_KHR"; break;
        case VK_THREAD_DONE_KHR                                    : return "VK_THREAD_DONE_KHR"; break;
        case VK_OPERATION_DEFERRED_KHR                             : return "VK_OPERATION_DEFERRED_KHR"; break;
        case VK_OPERATION_NOT_DEFERRED_KHR                         : return "VK_OPERATION_NOT_DEFERRED_KHR"; break;
        case VK_PIPELINE_COMPILE_REQUIRED_EXT                      : return "VK_PIPELINE_COMPILE_REQUIRED_EXT"; break;
    }
    // clang-format on
    return "<UNKNOWN VkResult value>";
}

VkAttachmentLoadOp ToVkAttachmentLoadOp(grfx::AttachmentLoadOp value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::ATTACHMENT_LOAD_OP_LOAD      : return VK_ATTACHMENT_LOAD_OP_LOAD; break;
        case grfx::ATTACHMENT_LOAD_OP_CLEAR     : return VK_ATTACHMENT_LOAD_OP_CLEAR; break;
        case grfx::ATTACHMENT_LOAD_OP_DONT_CARE : return VK_ATTACHMENT_LOAD_OP_DONT_CARE; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkAttachmentLoadOp>();
}

VkAttachmentStoreOp ToVkAttachmentStoreOp(grfx::AttachmentStoreOp value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::ATTACHMENT_STORE_OP_STORE     : return VK_ATTACHMENT_STORE_OP_STORE; break;
        case grfx::ATTACHMENT_STORE_OP_DONT_CARE : return VK_ATTACHMENT_STORE_OP_DONT_CARE; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkAttachmentStoreOp>();
}

VkClearColorValue ToVkClearColorValue(const grfx::RenderTargetClearValue& value)
{
    VkClearColorValue res = {};
    res.float32[0]        = value.rgba[0];
    res.float32[1]        = value.rgba[1];
    res.float32[2]        = value.rgba[2];
    res.float32[3]        = value.rgba[3];
    return res;
}

VkClearDepthStencilValue ToVkClearDepthStencilValue(const grfx::DepthStencilClearValue& value)
{
    VkClearDepthStencilValue res = {};
    res.depth                    = value.depth;
    res.stencil                  = value.stencil;
    return res;
}

VkComponentSwizzle ToVkComponentSwizzle(grfx::ComponentSwizzle value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::COMPONENT_SWIZZLE_IDENTITY : return VK_COMPONENT_SWIZZLE_IDENTITY; break;
        case grfx::COMPONENT_SWIZZLE_ZERO     : return VK_COMPONENT_SWIZZLE_ZERO    ; break;
        case grfx::COMPONENT_SWIZZLE_ONE      : return VK_COMPONENT_SWIZZLE_ONE     ; break;
        case grfx::COMPONENT_SWIZZLE_R        : return VK_COMPONENT_SWIZZLE_R       ; break;
        case grfx::COMPONENT_SWIZZLE_G        : return VK_COMPONENT_SWIZZLE_G       ; break;
        case grfx::COMPONENT_SWIZZLE_B        : return VK_COMPONENT_SWIZZLE_B       ; break;
        case grfx::COMPONENT_SWIZZLE_A        : return VK_COMPONENT_SWIZZLE_A       ; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkComponentSwizzle>();
}

VkComponentMapping ToVkComponentMapping(const grfx::ComponentMapping& value)
{
    VkComponentMapping res = {};
    res.r                  = ToVkComponentSwizzle(value.r);
    res.g                  = ToVkComponentSwizzle(value.g);
    res.b                  = ToVkComponentSwizzle(value.b);
    res.a                  = ToVkComponentSwizzle(value.a);
    return res;
}

VkFormat ToVkFormat(grfx::Format value)
{
    VkFormat result = VK_FORMAT_UNDEFINED;

    // clang-format off
    switch (value) {
        default: break;

        // 8-bit signed normalized
        case FORMAT_R8_SNORM           : result = VK_FORMAT_R8_SNORM; break;
        case FORMAT_R8G8_SNORM         : result = VK_FORMAT_R8G8_SNORM; break;
        case FORMAT_R8G8B8_SNORM       : result = VK_FORMAT_R8G8B8_SNORM; break;
        case FORMAT_R8G8B8A8_SNORM     : result = VK_FORMAT_R8G8B8A8_SNORM; break;
        case FORMAT_B8G8R8_SNORM       : result = VK_FORMAT_B8G8R8_SNORM; break;
        case FORMAT_B8G8R8A8_SNORM     : result = VK_FORMAT_B8G8R8A8_SNORM; break;
                                             
        // 8-bit unsigned normalized         
        case FORMAT_R8_UNORM           : result = VK_FORMAT_R8_UNORM; break;
        case FORMAT_R8G8_UNORM         : result = VK_FORMAT_R8G8_UNORM; break;
        case FORMAT_R8G8B8_UNORM       : result = VK_FORMAT_R8G8B8_UNORM; break;
        case FORMAT_R8G8B8A8_UNORM     : result = VK_FORMAT_R8G8B8A8_UNORM; break;
        case FORMAT_B8G8R8_UNORM       : result = VK_FORMAT_B8G8R8_UNORM; break;
        case FORMAT_B8G8R8A8_UNORM     : result = VK_FORMAT_B8G8R8A8_UNORM; break;
                                             
        // 8-bit signed integer              
        case FORMAT_R8_SINT            : result = VK_FORMAT_R8_SINT; break;
        case FORMAT_R8G8_SINT          : result = VK_FORMAT_R8G8_SINT; break;
        case FORMAT_R8G8B8_SINT        : result = VK_FORMAT_R8G8B8_SINT; break;
        case FORMAT_R8G8B8A8_SINT      : result = VK_FORMAT_R8G8B8A8_SINT; break;
        case FORMAT_B8G8R8_SINT        : result = VK_FORMAT_B8G8R8_SINT; break;
        case FORMAT_B8G8R8A8_SINT      : result = VK_FORMAT_B8G8R8A8_SINT; break;
                                             
        // 8-bit unsigned integer            
        case FORMAT_R8_UINT            : result = VK_FORMAT_R8_UINT; break;
        case FORMAT_R8G8_UINT          : result = VK_FORMAT_R8G8_UINT; break;
        case FORMAT_R8G8B8_UINT        : result = VK_FORMAT_R8G8B8_UINT; break;
        case FORMAT_R8G8B8A8_UINT      : result = VK_FORMAT_R8G8B8A8_UINT; break;
        case FORMAT_B8G8R8_UINT        : result = VK_FORMAT_B8G8R8_UINT; break;
        case FORMAT_B8G8R8A8_UINT      : result = VK_FORMAT_B8G8R8A8_UINT; break;
                                             
        // 16-bit signed normalized          
        case FORMAT_R16_SNORM          : result = VK_FORMAT_R16_SNORM; break;
        case FORMAT_R16G16_SNORM       : result = VK_FORMAT_R16G16_SNORM; break;
        case FORMAT_R16G16B16_SNORM    : result = VK_FORMAT_R16G16B16_SNORM; break;
        case FORMAT_R16G16B16A16_SNORM : result = VK_FORMAT_R16G16B16A16_SNORM; break;
                                             
        // 16-bit unsigned normalized        
        case FORMAT_R16_UNORM          : result = VK_FORMAT_R16_UNORM; break;
        case FORMAT_R16G16_UNORM       : result = VK_FORMAT_R16G16_UNORM; break;
        case FORMAT_R16G16B16_UNORM    : result = VK_FORMAT_R16G16B16_UNORM; break;
        case FORMAT_R16G16B16A16_UNORM : result = VK_FORMAT_R16G16B16A16_UNORM; break;
                                             
        // 16-bit signed integer             
        case FORMAT_R16_SINT           : result = VK_FORMAT_R16_SINT; break;
        case FORMAT_R16G16_SINT        : result = VK_FORMAT_R16G16_SINT; break;
        case FORMAT_R16G16B16_SINT     : result = VK_FORMAT_R16G16B16_SINT; break;
        case FORMAT_R16G16B16A16_SINT  : result = VK_FORMAT_R16G16B16A16_SINT; break;
                                             
        // 16-bit unsigned integer           
        case FORMAT_R16_UINT           : result = VK_FORMAT_R16_UINT; break;
        case FORMAT_R16G16_UINT        : result = VK_FORMAT_R16G16_UINT; break;
        case FORMAT_R16G16B16_UINT     : result = VK_FORMAT_R16G16B16_UINT; break;
        case FORMAT_R16G16B16A16_UINT  : result = VK_FORMAT_R16G16B16A16_UINT; break;
                                             
        // 16-bit float                      
        case FORMAT_R16_FLOAT          : result = VK_FORMAT_R16_SFLOAT; break;
        case FORMAT_R16G16_FLOAT       : result = VK_FORMAT_R16G16_SFLOAT; break;
        case FORMAT_R16G16B16_FLOAT    : result = VK_FORMAT_R16G16B16_SFLOAT; break;
        case FORMAT_R16G16B16A16_FLOAT : result = VK_FORMAT_R16G16B16A16_SFLOAT; break;
                                             
        // 32-bit signed integer             
        case FORMAT_R32_SINT           : result = VK_FORMAT_R32_SINT; break;
        case FORMAT_R32G32_SINT        : result = VK_FORMAT_R32G32_SINT; break;
        case FORMAT_R32G32B32_SINT     : result = VK_FORMAT_R32G32B32_SINT; break;
        case FORMAT_R32G32B32A32_SINT  : result = VK_FORMAT_R32G32B32A32_SINT; break;
                                             
        // 32-bit unsigned integer           
        case FORMAT_R32_UINT           : result = VK_FORMAT_R32_UINT; break;
        case FORMAT_R32G32_UINT        : result = VK_FORMAT_R32G32_UINT; break;
        case FORMAT_R32G32B32_UINT     : result = VK_FORMAT_R32G32B32_UINT; break;
        case FORMAT_R32G32B32A32_UINT  : result = VK_FORMAT_R32G32B32A32_UINT; break;
                                             
        // 32-bit float                      
        case FORMAT_R32_FLOAT          : result = VK_FORMAT_R32_SFLOAT; break;
        case FORMAT_R32G32_FLOAT       : result = VK_FORMAT_R32G32_SFLOAT; break;
        case FORMAT_R32G32B32_FLOAT    : result = VK_FORMAT_R32G32B32_SFLOAT; break;
        case FORMAT_R32G32B32A32_FLOAT : result = VK_FORMAT_R32G32B32A32_SFLOAT; break;
                                             
        // 8-bit unsigned integer stencil    
        case FORMAT_S8_UINT            : result = VK_FORMAT_S8_UINT; break;
                                             
        // 16-bit unsigned normalized depth  
        case FORMAT_D16_UNORM          : result = VK_FORMAT_D16_UNORM; break;
                                             
        // 32-bit float depth                
        case FORMAT_D32_FLOAT          : result = VK_FORMAT_D32_SFLOAT; break;
                                             
        // Depth/stencil combinations        
        case FORMAT_D16_UNORM_S8_UINT  : result = VK_FORMAT_D16_UNORM_S8_UINT; break;
        case FORMAT_D24_UNORM_S8_UINT  : result = VK_FORMAT_D24_UNORM_S8_UINT; break;
        case FORMAT_D32_FLOAT_S8_UINT  : result = VK_FORMAT_D32_SFLOAT_S8_UINT; break;
                                             
        // SRGB                              
        case FORMAT_R8_SRGB            : result = VK_FORMAT_R8_SRGB; break;
        case FORMAT_R8G8_SRGB          : result = VK_FORMAT_R8G8_SRGB; break;
        case FORMAT_R8G8B8_SRGB        : result = VK_FORMAT_R8G8B8_SRGB; break;
        case FORMAT_R8G8B8A8_SRGB      : result = VK_FORMAT_R8G8B8A8_SRGB; break;
        case FORMAT_B8G8R8_SRGB        : result = VK_FORMAT_B8G8R8_SRGB; break;
        case FORMAT_B8G8R8A8_SRGB      : result = VK_FORMAT_B8G8R8A8_SRGB; break;
    }
    // clang-format on

    return result;
}

VkImageType ToVkImageType(grfx::ImageType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_TYPE_1D : return VK_IMAGE_TYPE_1D; break;
        case grfx::IMAGE_TYPE_2D : return VK_IMAGE_TYPE_2D; break;
        case grfx::IMAGE_TYPE_3D : return VK_IMAGE_TYPE_3D; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkImageType>();
}

VkImageViewType ToVkImageViewType(grfx::ImageViewType value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::IMAGE_VIEW_TYPE_1D         : return VK_IMAGE_VIEW_TYPE_1D        ; break;
        case grfx::IMAGE_VIEW_TYPE_2D         : return VK_IMAGE_VIEW_TYPE_2D        ; break;
        case grfx::IMAGE_VIEW_TYPE_3D         : return VK_IMAGE_VIEW_TYPE_3D        ; break;
        case grfx::IMAGE_VIEW_TYPE_CUBE       : return VK_IMAGE_VIEW_TYPE_CUBE      ; break;
        case grfx::IMAGE_VIEW_TYPE_1D_ARRAY   : return VK_IMAGE_VIEW_TYPE_1D_ARRAY  ; break;
        case grfx::IMAGE_VIEW_TYPE_2D_ARRAY   : return VK_IMAGE_VIEW_TYPE_2D_ARRAY  ; break;
        case grfx::IMAGE_VIEW_TYPE_CUBE_ARRAY : return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkImageViewType>();
}

VkPresentModeKHR ToVkPresentMode(grfx::PresentMode value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::PRESENT_MODE_FIFO      : return VK_PRESENT_MODE_FIFO_KHR; break;
        case grfx::PRESENT_MODE_MAILBOX   : return VK_PRESENT_MODE_MAILBOX_KHR; break;
        case grfx::PRESENT_MODE_IMMEDIATE : return VK_PRESENT_MODE_IMMEDIATE_KHR; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkPresentModeKHR>();
}

VkSampleCountFlagBits ToVkSampleCount(grfx::SampleCount value)
{
    // clang-format off
    switch (value) {
        default: break;
        case grfx::SAMPLE_COUNT_1  : return VK_SAMPLE_COUNT_1_BIT; break;
        case grfx::SAMPLE_COUNT_2  : return VK_SAMPLE_COUNT_2_BIT; break;
        case grfx::SAMPLE_COUNT_4  : return VK_SAMPLE_COUNT_4_BIT; break;
        case grfx::SAMPLE_COUNT_8  : return VK_SAMPLE_COUNT_8_BIT; break;
        case grfx::SAMPLE_COUNT_16 : return VK_SAMPLE_COUNT_16_BIT; break;
        case grfx::SAMPLE_COUNT_32 : return VK_SAMPLE_COUNT_32_BIT; break;
        case grfx::SAMPLE_COUNT_64 : return VK_SAMPLE_COUNT_64_BIT; break;
    }
    // clang-format on
    return ppx::InvalidValue<VkSampleCountFlagBits>();
}

static Result ToVkBarrier(ResourceState state, bool isSource, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout)
{
    const VkPipelineStageFlags PIPELINE_STAGE_ALL_SHADER_STAGES =
        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
        VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
        VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    const VkPipelineStageFlags PIPELINE_STAGE_NON_PIXEL_SHADER_STAGES =
        VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
        VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
        VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
        VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT |
        VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

    switch (state) {
        default: return ppx::ERROR_FAILED; break;

        case grfx::RESOURCE_STATE_GENERAL: {
            stageMask  = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
            accessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_GENERAL;
        } break;

        case grfx::RESOURCE_STATE_CONSTANT_BUFFER:
        case grfx::RESOURCE_STATE_VERTEX_BUFFER: {
            stageMask  = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT | PIPELINE_STAGE_ALL_SHADER_STAGES;
            accessMask = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | VK_ACCESS_UNIFORM_READ_BIT;
            layout     = InvalidValue<VkImageLayout>();
        } break;

        case grfx::RESOURCE_STATE_INDEX_BUFFER: {
            stageMask  = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
            accessMask = VK_ACCESS_INDEX_READ_BIT;
            layout     = InvalidValue<VkImageLayout>();
        } break;

        case grfx::RESOURCE_STATE_RENDER_TARGET: {
            stageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            accessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_UNORDERED_ACCESS: {
            stageMask  = PIPELINE_STAGE_ALL_SHADER_STAGES;
            accessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_GENERAL;
        } break;

        case grfx::RESOURCE_STATE_DEPTH_STENCIL_READ: {
            stageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_DEPTH_STENCIL_WRITE: {
            stageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_DEPTH_WRITE_STENCIL_READ: {
            stageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_STENCIL_READ_ONLY_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_DEPTH_READ_STENCIL_WRITE: {
            stageMask  = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            accessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_STENCIL_ATTACHMENT_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE: {
            stageMask  = PIPELINE_STAGE_NON_PIXEL_SHADER_STAGES;
            accessMask = VK_ACCESS_UNIFORM_READ_BIT;
            layout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_PIXEL_SHADER_RESOURCE: {
            stageMask  = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            accessMask = VK_ACCESS_UNIFORM_READ_BIT;
            layout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_STREAM_OUT: {
            stageMask  = VK_PIPELINE_STAGE_TRANSFORM_FEEDBACK_BIT_EXT;
            accessMask = VK_ACCESS_TRANSFORM_FEEDBACK_WRITE_BIT_EXT;
            layout     = InvalidValue<VkImageLayout>();
        } break;

        case grfx::RESOURCE_STATE_INDIRECT_ARGUMENT: {
            stageMask  = VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;
            accessMask = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;
            layout     = InvalidValue<VkImageLayout>();
        } break;

        case grfx::RESOURCE_STATE_COPY_SRC: {
            stageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            accessMask = VK_ACCESS_TRANSFER_READ_BIT;
            layout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_COPY_DST: {
            stageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_RESOLVE_SRC: {
            stageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            accessMask = VK_ACCESS_TRANSFER_READ_BIT;
            layout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_RESOLVE_DST: {
            stageMask  = VK_PIPELINE_STAGE_TRANSFER_BIT;
            accessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        } break;

        case grfx::RESOURCE_STATE_PRESENT: {
            stageMask  = isSource ? VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT : VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
            accessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
            layout     = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        } break;

        case grfx::RESOURCE_STATE_PREDICATION: {
            stageMask  = VK_PIPELINE_STAGE_CONDITIONAL_RENDERING_BIT_EXT;
            accessMask = VK_ACCESS_CONDITIONAL_RENDERING_READ_BIT_EXT;
            layout     = InvalidValue<VkImageLayout>();
        } break;

        case grfx::RESOURCE_STATE_RAYTRACING_ACCELERATION_STRUCTURE: {
            stageMask  = InvalidValue<VkPipelineStageFlags>();
            accessMask = InvalidValue<VkAccessFlags>();
            layout     = InvalidValue<VkImageLayout>();
        } break;
    }

    return ppx::SUCCESS;
}

Result ToVkBarrierSrc(ResourceState state, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout)
{
    return ToVkBarrier(state, true, stageMask, accessMask, layout);
}

Result ToVkBarrierDst(ResourceState state, VkPipelineStageFlags& stageMask, VkAccessFlags& accessMask, VkImageLayout& layout)
{
    return ToVkBarrier(state, false, stageMask, accessMask, layout);
}

VkImageAspectFlags DetermineAspectMask(VkFormat format)
{
    // clang-format off
    switch (format) {
        // Depth
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT: {
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        } break;

        // Stencil
        case VK_FORMAT_S8_UINT: {
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        } break;

        // Depth/stencil
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT: {
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        } break;
        
            // Assume everything else is color
        default: break;
    }
    // clang-format on
    return VK_IMAGE_ASPECT_COLOR_BIT;
}

} // namespace vk
} // namespace grfx
} // namespace ppx
