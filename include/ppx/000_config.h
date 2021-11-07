#ifndef ppx_config_h
#define ppx_config_h

// clang-format off
#if defined(PPX_MSW)
#   if !defined(VC_EXTRALEAN)
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
#include <array>
#include <functional>
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <vector>
#include <bitset>

#include "ppx/ccomptr.h"
#include "ppx/obj_ptr.h"
#include "ppx/log.h"
#include "ppx/util.h"

// clang-format off
#define PPX_STRINGIFY_(x)   #x
#define PPX_STRINGIFY(x)    PPX_STRINGIFY_(x)
#define PPX_LINE            PPX_STRINGIFY(__LINE__)
#define PPX_SOURCE_LOCATION __FUNCTION__ << " @ " __FILE__ ":" PPX_LINE
#define PPX_VAR_VALUE(var)  #var << ":" << var
#define PPX_ENDL            std::endl

#define PPX_ASSERT_MSG(COND, MSG)                                            \
    if ((COND) == false) {                                                   \
        PPX_LOG_RAW(PPX_ENDL                                                 \
            << "*** PPX ASSERT ***" << PPX_ENDL                              \
            << "Message   : " << MSG << " " << PPX_ENDL                      \
            << "Condition : " << #COND << " " << PPX_ENDL                    \
            << "Function  : " << __FUNCTION__ << PPX_ENDL                    \
            << "Location  : " << __FILE__ << " : " << PPX_LINE << PPX_ENDL); \
        assert(false);                                                       \
    }

#define PPX_ASSERT_NULL_ARG(ARG)                                             \
    if ((ARG) == nullptr) {                                                  \
        PPX_LOG_RAW(PPX_ENDL                                                 \
            << "*** PPX NULL ARGUMNET ***" << PPX_ENDL                       \
            << "Argument  : " << #ARG << " " << PPX_ENDL                     \
            << "Function  : " << __FUNCTION__ << PPX_ENDL                    \
            << "Location  : " << __FILE__ << " : " << PPX_LINE << PPX_ENDL); \
        assert(false);                                                       \
    }

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
    ERROR_DUPLICATE_ELEMENT            = -6,
    ERROR_LIMIT_EXCEEDED               = -7,
    ERROR_PATH_DOES_NOT_EXIST          = -8,
    ERROR_SINGLE_INIT_ONLY             = -9,
    ERROR_UNEXPECTED_NULL_ARGUMENT     = -10,
    ERROR_UNEXPECTED_COUNT_VALUE       = -11,
    ERROR_UNSUPPORTED_API              = -12,
    ERROR_API_FAILURE                  = -13,
    ERROR_WAIT_FAILED                  = -14,
    ERROR_WAIT_TIMED_OUT               = -15,
    ERROR_NO_GPUS_FOUND                = -16,
    ERROR_REQUIRED_FEATURE_UNAVAILABLE = -17,
    EEROR_BAD_DATA_SOURCE              = -18,

    ERROR_GLFW_INIT_FAILED          = -200,
    ERROR_GLFW_CREATE_WINDOW_FAILED = -201,

    ERROR_INVALID_CREATE_ARGUMENT    = -300,
    ERROR_RANGE_ALIASING_NOT_ALLOWED = -301,

    ERROR_GRFX_INVALID_OWNERSHIP                    = -1000,
    ERROR_GRFX_OBJECT_OWNERSHIP_IS_RESTRICTED       = -1001,
    ERROR_GRFX_UNSUPPORTED_SWAPCHAIN_FORMAT         = -1002,
    ERROR_GRFX_UNSUPPORTED_PRESENT_MODE             = -1003,
    ERROR_GRFX_MAX_VERTEX_BINDING_EXCEEDED          = -1004,
    ERROR_GRFX_VERTEX_ATTRIBUTE_FROMAT_UNDEFINED    = -1005,
    ERROR_GRFX_VERTEX_ATTRIBUTE_OFFSET_OUT_OF_ORDER = -1006,
    ERROR_GRFX_CANNOT_MIX_VERTEX_INPUT_RATES        = -1007,
    ERROR_GRFX_UNKNOWN_DESCRIPTOR_TYPE              = -1008,
    ERROR_GRFX_INVALID_DESCRIPTOR_TYPE              = -1009,
    ERROR_GRFX_DESCRIPTOR_COUNT_EXCEEDED            = -1010,
    ERROR_GRFX_BINDING_NOT_IN_SET                   = -1011,
    ERROR_GRFX_NON_UNIQUE_SET                       = -1012,
    ERROR_GRFX_MINIMUM_BUFFER_SIZE_NOT_MET          = -1013,
    ERROR_GRFX_INVALID_SHADER_BYTE_CODE             = -1014,
    ERROR_INVALID_PIPELINE_INTERFACE                = -1015,
    ERROR_GRFX_INVALID_QUERY_TYPE                   = -1016,
    ERROR_GRFX_INVALID_QUERY_COUNT                  = -1017,
    ERROR_GRFX_NO_QUEUES_AVAILABLE                  = -1018,

    ERROR_IMAGE_FILE_LOAD_FAILED               = -2000,
    ERROR_IMAGE_FILE_SAVE_FAILED               = -2001,
    ERROR_IMAGE_CANNOT_RESIZE_EXTERNAL_STORAGE = -2002,
    ERROR_IMAGE_INVALID_FORMAT                 = -2003,
    ERROR_IMAGE_RESIZE_FAILED                  = -2004,
    ERROR_BITMAP_CREATE_FAILED                 = -2005,
    ERROR_BITMAP_BAD_COPY_SOURCE               = -2006,
    ERROR_BITMAP_FOOTPRINT_MISMATCH            = -2007,

    ERROR_NO_INDEX_DATA             = -2400,
    ERROR_GEOMETRY_FILE_LOAD_FAILED = -2500,
    ERROR_GEOMETRY_FILE_NO_DATA     = -2501,

    ERROR_WINDOW_EVENTS_ALREADY_REGISTERED = -3000,
    ERROR_IMGUI_INITIALIZATION_FAILED      = -3001,

    ERROR_FONT_PARSE_FAILED   = -4000,
    ERROR_INVALID_UTF8_STRING = -4001,
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
