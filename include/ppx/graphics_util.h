#ifndef ppx_graphics_util_h
#define ppx_graphics_util_h

#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/fs.h"

namespace ppx {

Result CreateTextureFromFile(grfx::Queue* pQueue, const fs::path& path, grfx::Image** ppImage);

} // namespace ppx

#endif // ppx_graphics_util_h
