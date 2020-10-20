#ifndef ppx_graphics_util_h
#define ppx_graphics_util_h

#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_queue.h"
#include "ppx/fs.h"
#include "ppx/geometry.h"

namespace ppx {

Result CreateTextureFromFile(
    grfx::Queue*                 pQueue,
    const fs::path&              path,
    grfx::Image**                ppImage,
    const grfx::ImageUsageFlags& additionalImageUsage = grfx::ImageUsageFlags());

Result CreateModelFromGeometry(
    grfx::Queue*    pQueue,
    const Geometry* pGeometry,
    grfx::Model**   ppMode);

} // namespace ppx

#endif // ppx_graphics_util_h
