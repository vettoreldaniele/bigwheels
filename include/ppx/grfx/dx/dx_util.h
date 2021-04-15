#ifndef ppx_grfx_dx_util_h
#define ppx_grfx_dx_util_h

#include "ppx/grfx/grfx_format.h"

#include <dxgi1_6.h>

namespace ppx {
namespace grfx {
namespace dx {

DXGI_FORMAT ToDxgiFormat(grfx::Format value);

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_util_h
