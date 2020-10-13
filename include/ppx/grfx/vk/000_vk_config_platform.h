#ifndef ppx_grfx_vk_config_platform_h
#define ppx_grfx_vk_config_platform_h

// clang-format off
#if defined(PPX_GGP)
#   if ! defined(VK_USE_PLATFORM_GGP)
#       define VK_USE_PLATFORM_GGP
#   endif
#elif defined(PPX_LINUX)
#   if defined(PPX_LINUX_WAYLAND)
#       if ! defined(VK_USE_PLATFORM_WAYLAND_KHR)
#           define VK_USE_PLATFORM_WAYLAND_KHR
#       endif
#   elif defined(PPX_LINUX_XCB)
#       if ! defined(VK_USE_PLATFORM_XCB_KHR)
#           define VK_USE_PLATFORM_XCB_KHR
#       endif
#   elif defined(PPX_LINUX_XLIB)
#       if ! defined(VK_USE_PLATFORM_XLIB_KHR)
#           define VK_USE_PLATFORM_XLIB_KHR
#       endif
    #endif
#elif defined(PPX_MSW)
#   if ! defined(VK_USE_PLATFORM_WIN32_KHR)
#       define VK_USE_PLATFORM_WIN32_KHR
#   endif
#endif
// clang-format on
#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#define VK_LAYER_KHRONOS_VALIDATION_NAME "VK_LAYER_KHRONOS_validation"

#endif // ppx_grfx_vk_config_platform_h
