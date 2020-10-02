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

// clang-format off
#define PPX_STRINGIFY_(x)       #x
#define PPX_STRINGIFY(x)        PPX_STRINGIFY_(x)
#define PPX_LINE                PPX_STRINGIFY(__LINE__)
#define PPX_SOURCE_LOCATION     __FUNCTION__ ## " @ " ## __FILE__ ## ":" ## PPX_LINE
#define PPX_VAR_VALUE(var)      #var << ":"  << var
// clang-format on

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

namespace ppx {

enum Result
{
    SUCCESS                        = 0,
    ERROR_FAILED                   = -1,
    ERROR_ALLOCATION_FAILED        = -2,
    ERROR_ELEMENT_NOT_FOUND        = -3,
    ERROR_OUT_OF_RANGE             = -4,
    ERROR_PATH_DOES_NOT_EXIST      = -5,
    ERROR_SINGLE_INIT_ONLY         = -6,
    ERROR_UNEXPECTED_NULL_ARGUMENT = -7,
    ERROR_UNSUPPORTED_API          = -10,
    ERROR_API_FAILURE              = -11,

    ERROR_GLFW_INIT_FAILED          = -200,
    ERROR_GLFW_CREATE_WINDOW_FAILED = -201,

    ERROR_INVALID_CREATE_ARGUMENT = -300,
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

template <typename T>
T InvalidValue(const T& value = static_cast<T>(~0))
{
    return value;
}

// -------------------------------------------------------------------------------------------------

} // namespace ppx

#endif // ppx_config_h
