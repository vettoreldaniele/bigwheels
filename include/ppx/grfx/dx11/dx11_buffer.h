#ifndef ppx_grfx_dx11_buffer_h
#define ppx_grfx_dx11_buffer_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_buffer.h"

namespace ppx {
namespace grfx {
namespace dx11 {

class Buffer
    : public grfx::Buffer
{
public:
    Buffer() {}
    virtual ~Buffer() {}

    typename D3D11BufferPtr::InterfaceType* GetDxBuffer() const { return mBuffer.Get(); }
    D3D11_MAP                               GetMapType() const;

    virtual Result MapMemory(uint64_t offset, void** ppMappedAddress) override;
    virtual void   UnmapMemory() override;

protected:
    virtual Result CreateApiObjects(const grfx::BufferCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D11_USAGE    mUsage          = InvalidValue<D3D11_USAGE>();
    UINT           mCpuAccessFlags = 0;
    D3D11BufferPtr mBuffer;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx11_buffer_h
