#include "ppx/grfx/dx/dx_instance.h"

namespace ppx {
namespace grfx {
namespace dx {

Result Instance::CreateApiObjects(const grfx::InstanceCreateInfo* pCreateInfo)
{
    return ppx::ERROR_FAILED;
}

void Instance::DestroyApiObjects()
{
}

Result Instance::AllocateObject(grfx::Device** ppDevice)
{
    return ppx::ERROR_ALLOCATION_FAILED;
}

Result Instance::AllocateObject(grfx::Gpu** ppGpu)
{
    return ppx::ERROR_ALLOCATION_FAILED;
}

Result Instance::AllocateObject(grfx::Surface** ppSurface)
{
    return ppx::ERROR_ALLOCATION_FAILED;
}

} // namespace dx
} // namespace grfx
} // namespace ppx
