#include "Buffer.h"
#include "ppx/grfx/grfx_device.h"

// -------------------------------------------------------------------------------------------------
// UpdatedableBuffer
// -------------------------------------------------------------------------------------------------
Result UpdatedableBuffer::CreateObjects(grfx::Device* pDevice, uint32_t size, grfx::BufferUsageFlagBits usage)
{
    grfx::BufferCreateInfo createInfo = {};
    createInfo.size                   = static_cast<uint64_t>(size);
    createInfo.usageFlags             = grfx::BUFFER_USAGE_TRANSFER_SRC;
    createInfo.memoryUsage            = grfx::MEMORY_USAGE_CPU_TO_GPU;
    createInfo.initialState           = grfx::RESOURCE_STATE_CONSTANT_BUFFER;
    createInfo.ownership              = grfx::OWNERSHIP_REFERENCE;

    // Create CPU buffer
    Result ppxres = pDevice->CreateBuffer(&createInfo, &mCpuBuffer);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Set GPU buffer values
    createInfo.usageFlags  = usage | grfx::BUFFER_USAGE_TRANSFER_DST;
    createInfo.memoryUsage = grfx::MEMORY_USAGE_GPU_ONLY;

    // Create GPU buffer
    ppxres = pDevice->CreateBuffer(&createInfo, &mGpuBuffer);
    if (Failed(ppxres)) {
        return ppxres;
    }

    // Persistent map the CPU buffer
    ppxres = mCpuBuffer->MapMemory(0, &mMappedAdress);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void UpdatedableBuffer::DestroyObjects()
{
    if (mCpuBuffer) {
        if (!IsNull(mMappedAdress)) {
            mCpuBuffer->UnmapMemory();
        }

        grfx::DevicePtr device = mCpuBuffer->GetDevice();
        device->DestroyBuffer(mCpuBuffer);
        mCpuBuffer.Reset();
    }

    if (mGpuBuffer) {
        grfx::DevicePtr device = mGpuBuffer->GetDevice();
        device->DestroyBuffer(mGpuBuffer);
        mGpuBuffer.Reset();
    }
}

// -------------------------------------------------------------------------------------------------
// ConstantBuffer
// -------------------------------------------------------------------------------------------------
Result ConstantBuffer::Create(grfx::Device* pDevice, uint32_t size)
{
    Result ppxres = CreateObjects(pDevice, size, grfx::BUFFER_USAGE_UNIFORM_BUFFER);
    if (Failed(ppxres)) {
        Destroy();
        return ppxres;
    }
    return ppx::SUCCESS;
}

void ConstantBuffer::Destroy()
{
    DestroyObjects();
}
