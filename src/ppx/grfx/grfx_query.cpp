#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {

Result QueryPool::Create(const grfx::QueryPoolCreateInfo* pCreateInfo)
{
    if (pCreateInfo->type == grfx::QUERY_TYPE_UNDEFINED) {
        return ppx::ERROR_GRFX_INVALID_QUERY_TYPE;
    }

    if (pCreateInfo->count == 0)  {
        return ppx::ERROR_GRFX_INVALID_QUERY_COUNT;
    }

    Result ppxres = grfx::DeviceObject<grfx::QueryPoolCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
