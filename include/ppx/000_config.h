#ifndef ppx_config_h
#define ppx_config_h

// clang-format off
#if defined(PPX_MSW)
#   if !defined(NOMINMAX)
#       define VC_EXTRALEAN
#   endif
#   if !defined(WIN32_LEAN_AND_MEAN)
#       define WIN32_LEAN_AND_MEAN 
#   endif
#   if !defined(NOMINMAX)
#       define NOMINMAX
#   endif
#endif
// clang-format on

#include <algorithm>
#include <cassert>
#include <string>
#include <vector>

#include "ppx/obj_ptr.h"
#include "ppx/log.h"
#include "ppx/util.h"

#define PPX_STRINGIFY_(x)   #x
#define PPX_STRINGIFY(x)    PPX_STRINGIFY_(x)
#define PPX_LINE            PPX_STRINGIFY(__LINE__)
#define PPX_SOURCE_LOCATION __FUNCTION__##" @ "##__FILE__##":"##PPX_LINE
#define PPX_VAR_VALUE(var)  #var << ":" << var
#define PPX_ENDL            std::endl

#define PPX_ASSERT_MSG(COND, MSG)                                \
    if ((COND) == false) {                                       \
        PPX_LOG_ERROR(MSG << " "                                 \
                          << "(" << PPX_SOURCE_LOCATION << ")"); \
        assert(false);                                           \
    }

#define PPX_ASSERT_NULL_ARG(ARG)                             \
    if ((ARG) == nullptr) {                                  \
        PPX_LOG_ERROR(#ARG##"is NULL"                        \
                      << " "                                 \
                      << "(" << PPX_SOURCE_LOCATION << ")"); \
        assert(false);                                       \
    }

// clang-format off
#define PPX_CHECKED_CALL(EXPR)                                                         \
    {                                                                                  \
        ppx::Result ppx_checked_result_0xdeadbeef = EXPR;                              \
        if (ppx_checked_result_0xdeadbeef != ppx::SUCCESS) {                           \
            PPX_LOG_RAW(PPX_ENDL                                                       \
                << "*** PPX Call Failed ***" << PPX_ENDL                               \
                << "Return     : " << ppx_checked_result_0xdeadbeef << " " << PPX_ENDL \
                << "Expression : " << #EXPR << " " << PPX_ENDL                         \
                << "Function   : " << __FUNCTION__ << PPX_ENDL                         \
                << "Location   : " << __FILE__ << " : " << PPX_LINE << PPX_ENDL);      \
            assert(false);                                                             \
        }                                                                              \
    }
// clang-format on

namespace ppx {

enum Result
{
    SUCCESS                            = 0,
    ERROR_FAILED                       = -1,
    ERROR_ALLOCATION_FAILED            = -2,
    ERROR_OUT_OF_MEMORY                = -3,
    ERROR_ELEMENT_NOT_FOUND            = -4,
    ERROR_OUT_OF_RANGE                 = -5,
    ERROR_LIMIT_EXCEEDED               = -6,
    ERROR_PATH_DOES_NOT_EXIST          = -7,
    ERROR_SINGLE_INIT_ONLY             = -8,
    ERROR_UNEXPECTED_NULL_ARGUMENT     = -9,
    ERROR_UNEXPECTED_COUNT_VALUE       = -10,
    ERROR_UNSUPPORTED_API              = -11,
    ERROR_API_FAILURE                  = -12,
    ERROR_WAIT_FAILED                  = -13,
    ERROR_WAIT_TIMED_OUT               = -14,
    ERROR_NO_GPUS_FOUND                = -15,
    ERROR_REQUIRED_FEATURE_UNAVAILABLE = -16,

    ERROR_GLFW_INIT_FAILED          = -200,
    ERROR_GLFW_CREATE_WINDOW_FAILED = -201,

    ERROR_INVALID_CREATE_ARGUMENT    = -300,
    ERROR_RANGE_ALIASING_NOT_ALLOWED = -301,

    ERROR_GRFX_UNSUPPORTED_SWAPCHAIN_FORMAT         = -1000,
    ERROR_GRFX_UNSUPPORTED_PRESENT_MODE             = -1001,
    ERROR_GRFX_MAX_VERTEX_BINDING_EXCEEDED          = -1002,
    ERROR_GRFX_VERTEX_ATTRIBUTE_FROMAT_UNDEFINED    = -1003,
    ERROR_GRFX_VERTEX_ATTRIBUTE_OFFSET_OUT_OF_ORDER = -1004,
    ERROR_GRFX_CANNOT_MIX_VERTEX_INPUT_RATES        = -1005,
    ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE              = -1006,
    ERROR_GRFX_BINDING_NOT_IN_SET                   = -1007,
    ERROR_GRFX_NON_UNIQUE_SET                       = -1008,
    ERROR_GRFX_MINIMUM_BUFFER_SIZE_NOT_MET          = -1009,

    ERROR_IMAGE_FILE_LOAD_FAILED               = -2000,
    ERROR_IMAGE_CANNOT_RESIZE_EXTERNAL_STORAGE = -2001,

    ERROR_IMGUI_INITIALIZATION_FAILED = -3000,
};

inline bool Success(ppx::Result value)
{
    bool res = (value == ppx::SUCCESS);
    return res;
}

inline bool Failed(ppx::Result value)
{
    bool res = (value < ppx::SUCCESS);
    return res;
}

} // namespace ppx

#endif // ppx_config_h