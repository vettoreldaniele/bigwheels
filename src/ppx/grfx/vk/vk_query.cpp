#include "ppx/grfx/vk/vk_query.h"

namespace ppx {
namespace grfx {
namespace vk {

Result QueryPool::CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo)
{
    return ppx::SUCCESS;
}

void QueryPool::DestroyApiObjects()
{
}

} // namespace vk
} // namespace grfx
} // namespace ppx
