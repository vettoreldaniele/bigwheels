#ifndef BUFFER_H
#define BUFFER_H

#include "ppx/grfx/grfx_buffer.h"
using namespace ppx;

class UpdatedableBuffer
{
public:
    UpdatedableBuffer() {}
    virtual ~UpdatedableBuffer() {}

    uint64_t        GetSize() const { return mCpuBuffer->GetSize(); }
    grfx::BufferPtr GetCpuBuffer() const { return mCpuBuffer; }
    grfx::BufferPtr GetGpuBuffer() const { return mGpuBuffer; }
    void*           GetMappedAddress() const { return mMappedAdress; }

protected:
    Result CreateObjects(grfx::Device* pDevice, uint32_t size, grfx::BufferUsageFlagBits usage);
    void   DestroyObjects();

private:
    void*           mMappedAdress = nullptr;
    grfx::BufferPtr mCpuBuffer;
    grfx::BufferPtr mGpuBuffer;
};

// -------------------------------------------------------------------------------------------------

class ConstantBuffer
    : public UpdatedableBuffer
{
public:
    ConstantBuffer() {}
    virtual ~ConstantBuffer() {}

    Result Create(grfx::Device* pDevice, uint32_t size);
    void   Destroy();
};

#endif // BUFFER_H
