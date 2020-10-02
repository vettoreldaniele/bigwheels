#include "ppx/grfx/grfx_queue.h"
#include "ppx/grfx/grfx_device.h"

namespace ppx {
namespace grfx {

Result Queue::CreateCommandBuffer(grfx::CommandBuffer** ppCommandBuffer)
{
    CommandSet set = {};

    grfx::CommandPoolCreateInfo ci = {};
    ci.pQueue                      = this;

    Result ppxres = GetDevice()->CreateCommandPool(&ci, &set.commandPool);
    if (Failed(ppxres)) {
        return ppxres;
    }

    ppxres = GetDevice()->AllocateCommandBuffer(set.commandPool, &set.commandBuffer);
    if (Failed(ppxres)) {
        GetDevice()->DestroyCommandPool(set.commandPool);
        return ppxres;
    }

    *ppCommandBuffer = set.commandBuffer;

    mCommandSets.push_back(set);

    return ppx::SUCCESS;
}

void Queue::DestroyCommandBuffer(const grfx::CommandBuffer* pCommandBuffer)
{
    auto it = std::find_if(
        std::begin(mCommandSets),
        std::end(mCommandSets),
        [pCommandBuffer](const CommandSet& elem) -> bool {
            bool isSame = (elem.commandBuffer.Get() == pCommandBuffer);
            return isSame; });
    if (it == std::end(mCommandSets)) {
        return;
    }

    CommandSet& set = *it;

    GetDevice()->FreeCommandBuffer(set.commandBuffer);
    GetDevice()->DestroyCommandPool(set.commandPool);
}

} // namespace grfx
} // namespace ppx
