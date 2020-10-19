#include "ppx/grfx/grfx_model.h"
#include "ppx/grfx/grfx_device.h"

namespace ppx {
namespace grfx {

Result Model::CreateApiObjects(const grfx::ModelCreateInfo* pCreateInfo)
{
    mIndexType   = pCreateInfo->indexType;
    mIndexBuffer = pCreateInfo->pIndexBuffer;
    if (!mIndexBuffer) {
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = pCreateInfo->indexBufferSize;
        bufferCreateInfo.usageFlags                  = pCreateInfo->indexBufferUsageFlags;
        bufferCreateInfo.usageFlags.bits.indexBuffer = true;
        bufferCreateInfo.usageFlags.bits.transferDst = true;
        bufferCreateInfo.memoryUsage                 = pCreateInfo->indexBufferMemoryUsage;
        bufferCreateInfo.ownership                   = grfx::OWNERSHIP_EXCLUSIVE;

        Result ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &mIndexBuffer);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "create model index buffer failed");
            return ppxres;
        }
    }
    else {
        mIndexBuffer->SetOwnership(grfx::OWNERSHIP_REFERENCE);
    }

    if (pCreateInfo->vertexBufferCount > 0) {
        mVertexBuffers.resize(pCreateInfo->vertexBufferCount);
        for (uint32_t i = 0; i < pCreateInfo->vertexBufferCount; ++i) {
            mVertexBuffers[i].binding = pCreateInfo->vertexBuffers[i].binding;
            mVertexBuffers[i].buffer  = pCreateInfo->vertexBuffers[i].pBuffer;
            if (!mVertexBuffers[i].buffer) {
                grfx::BufferCreateInfo bufferCreateInfo       = {};
                bufferCreateInfo.size                         = pCreateInfo->vertexBuffers[i].size;
                bufferCreateInfo.usageFlags                   = pCreateInfo->vertexBuffers[i].usageFlags;
                bufferCreateInfo.usageFlags.bits.vertexBuffer = true;
                bufferCreateInfo.usageFlags.bits.transferDst  = true;
                bufferCreateInfo.memoryUsage                  = pCreateInfo->vertexBuffers[i].memoryUsage;
                bufferCreateInfo.ownership                    = grfx::OWNERSHIP_EXCLUSIVE;

                grfx::BufferPtr buffer;
                Result          ppxres = GetDevice()->CreateBuffer(&bufferCreateInfo, &buffer);
                if (Failed(ppxres)) {
                    PPX_ASSERT_MSG(false, "create model index buffer failed i=" << i);
                    return ppxres;
                }

                VertexBuffer entry = {};
                entry.binding      = pCreateInfo->vertexBuffers[i].binding;
                entry.buffer       = buffer;
                mVertexBuffers.push_back(entry);
            }
            else {
                mVertexBuffers[i].buffer->SetOwnership(grfx::OWNERSHIP_REFERENCE);
            }
        }
    }

    if (pCreateInfo->textureCount > 0) {
        mTextures.resize(pCreateInfo->textureCount);
        for (uint32_t i = 0; i < pCreateInfo->textureCount; ++i) {
            mTextures[i] = pCreateInfo->textures[i].pTexture;
            if (!mTextures[i]) {
                grfx::TextureCreateInfo textureCreateInfo     = {};
                textureCreateInfo.pImage                      = nullptr;
                textureCreateInfo.imageType                   = pCreateInfo->textures[i].imageType;
                textureCreateInfo.width                       = pCreateInfo->textures[i].width;
                textureCreateInfo.height                      = pCreateInfo->textures[i].height;
                textureCreateInfo.depth                       = 1;
                textureCreateInfo.imageFormat                 = pCreateInfo->textures[i].format;
                textureCreateInfo.sampleCount                 = grfx::SAMPLE_COUNT_1;
                textureCreateInfo.mipLevelCount               = pCreateInfo->textures[i].mipLevelCount;
                textureCreateInfo.arrayLayerCount             = pCreateInfo->textures[i].arrayLayerCount;
                textureCreateInfo.usageFlags                  = pCreateInfo->textures[i].usageFlags;
                textureCreateInfo.usageFlags.bits.sampled     = true;
                textureCreateInfo.usageFlags.bits.transferDst = true;
                textureCreateInfo.memoryUsage                 = pCreateInfo->textures[i].memoryUsage;
                textureCreateInfo.initialState                = grfx::RESOURCE_STATE_SHADER_RESOURCE;
                textureCreateInfo.RTVClearValue               = pCreateInfo->textures[i].RTVClearValue;
                textureCreateInfo.DSVClearValue               = pCreateInfo->textures[i].DSVClearValue;
                textureCreateInfo.sampledImageViewType        = grfx::IMAGE_VIEW_TYPE_UNDEFINED;
                textureCreateInfo.sampledImageViewFormat      = grfx::FORMAT_UNDEFINED;
                textureCreateInfo.renderTargetViewFormat      = grfx::FORMAT_UNDEFINED;
                textureCreateInfo.depthStencilViewFormat      = grfx::FORMAT_UNDEFINED;
                textureCreateInfo.storageImageViewFormat      = grfx::FORMAT_UNDEFINED;
                textureCreateInfo.ownership                   = grfx::OWNERSHIP_EXCLUSIVE;

                grfx::TexturePtr texture;
                Result ppxres = GetDevice()->CreateTexture(&textureCreateInfo, &texture);
                if (Failed(ppxres)) {
                    PPX_ASSERT_MSG(false, "create model index buffer failed");
                    return ppxres;
                }

                mTextures.push_back(texture);
            }
            else {
                mTextures[i]->SetOwnership(grfx::OWNERSHIP_REFERENCE);
            }
        }
    }

    return ppx::ERROR_FAILED;
}

void Model::DestroyApiObjects()
{
}

void Model::CheckedDestroy(grfx::Buffer* pBuffer)
{
    if (pBuffer->GetOwnership() != grfx::OWNERSHIP_REFERENCE) {
        GetDevice()->DestroyBuffer(pBuffer);
    }
}

void Model::CheckedDestroy(grfx::Texture* pTexture)
{
    if (pTexture->GetOwnership() != grfx::OWNERSHIP_REFERENCE) {
        GetDevice()->DestroyTexture(pTexture);
    }
}

grfx::VertexBinding* Model::GetVertexBinding(uint32_t index)
{
    if (!IsIndexInRange(index, mVertexBuffers)) {
        return nullptr;
    }
    grfx::VertexBinding* ptr = &mVertexBuffers[index].binding;
    return ptr;
}

const grfx::VertexBinding* Model::GetVertexBinding(uint32_t index) const
{
    if (!IsIndexInRange(index, mVertexBuffers)) {
        return nullptr;
    }
    const grfx::VertexBinding* ptr = &mVertexBuffers[index].binding;
    return ptr;
}

grfx::BufferPtr Model::GetVertexBuffer(uint32_t index) const
{
    grfx::BufferPtr buffer;
    if (IsIndexInRange(index, mVertexBuffers)) {
        buffer = mVertexBuffers[index].buffer;
    }
    return buffer;
}

grfx::TexturePtr Model::GetTexture(uint32_t index) const
{
    grfx::TexturePtr texture;
    if (IsIndexInRange(index, mTextures)) {
        texture = mTextures[index];
    }
    return texture;
}

void Model::SetIndexBuffer(grfx::Buffer* pBuffer)
{
    CheckedDestroy(mIndexBuffer);
    mIndexBuffer = pBuffer;
    if (mIndexBuffer) {
        mIndexBuffer->SetOwnership(grfx::OWNERSHIP_REFERENCE);
    }
}

void Model::SetVertexBuffer(uint32_t index, grfx::Buffer* pBuffer)
{
    if (IsIndexInRange(index, mVertexBuffers)) {
        CheckedDestroy(mVertexBuffers[index].buffer);
    }

    if (index >= mVertexBuffers.size()) {
        mVertexBuffers.resize(index + 1);
    }

    mVertexBuffers[index].buffer = pBuffer;
    if (mVertexBuffers[index].buffer) {
        mVertexBuffers[index].buffer->SetOwnership(grfx::OWNERSHIP_REFERENCE);
    }
}

void Model::SetVertexBinding(uint32_t index, const grfx::VertexBinding& binding)
{
    if (index >= mVertexBuffers.size()) {
        mVertexBuffers.resize(index + 1);
    }
    mVertexBuffers[index].binding = binding;
}

void Model::SetVertexBuffer(uint32_t index, const grfx::VertexBinding& binding, grfx::Buffer* pBuffer)
{
    if (IsIndexInRange(index, mVertexBuffers)) {
        CheckedDestroy(mVertexBuffers[index].buffer);
    }

    if (index >= mVertexBuffers.size()) {
        mVertexBuffers.resize(index + 1);
    }

    mVertexBuffers[index].binding = binding;
    mVertexBuffers[index].buffer  = pBuffer;
    if (mVertexBuffers[index].buffer) {
        mVertexBuffers[index].buffer->SetOwnership(grfx::OWNERSHIP_REFERENCE);
    }
}

void Model::SetTexture(uint32_t index, grfx::Texture* pTexture)
{
    if (IsIndexInRange(index, mTextures)) {
        CheckedDestroy(mTextures[index]);
    }

    if (index >= mTextures.size()) {
        mTextures.resize(index + 1);
    }

    mTextures[index] = pTexture;
    if (mTextures[index]) {
        mTextures[index]->SetOwnership(grfx::OWNERSHIP_REFERENCE);
    }
}

void Model::AddIndexBuffer(grfx::Buffer* pBuffer)
{
    CheckedDestroy(mIndexBuffer);
    mIndexBuffer = pBuffer;
    mIndexBuffer->SetOwnership(grfx::OWNERSHIP_EXCLUSIVE);
}

void Model::AddVertexBuffer(grfx::Buffer* pBuffer)
{
    VertexBuffer entry = {};
    entry.buffer       = pBuffer;
    mVertexBuffers.push_back(entry);

    uint32_t index = CountU32(mVertexBuffers) - 1;
    if (mVertexBuffers[index].buffer) {
        mVertexBuffers[index].buffer->SetOwnership(grfx::OWNERSHIP_EXCLUSIVE);
    }
}

void Model::AddVertexBinding(const grfx::VertexBinding& binding)
{
    VertexBuffer entry = {};
    entry.binding      = binding;
    mVertexBuffers.push_back(entry);
}

void Model::AddVertexBuffer(const grfx::VertexBinding& binding, grfx::Buffer* pBuffer)
{
    VertexBuffer entry = {};
    entry.binding      = binding;
    entry.buffer       = pBuffer;
    entry.buffer->SetOwnership(grfx::OWNERSHIP_EXCLUSIVE);
    mVertexBuffers.push_back(entry);
}

void Model::AddVertexBuffer(uint32_t index, grfx::Buffer* pBuffer)
{
    SetVertexBuffer(index, pBuffer);
    if (mVertexBuffers[index].buffer) {
        mVertexBuffers[index].buffer->SetOwnership(grfx::OWNERSHIP_EXCLUSIVE);
    }
}

void Model::AddVertexBinding(uint32_t index, const grfx::VertexBinding& binding)
{
    SetVertexBinding(index, binding);
}

void Model::AddVertexBuffer(uint32_t index, const grfx::VertexBinding& binding, grfx::Buffer* pBuffer)
{
    SetVertexBuffer(index, binding, pBuffer);
    if (mVertexBuffers[index].buffer) {
        mVertexBuffers[index].buffer->SetOwnership(grfx::OWNERSHIP_EXCLUSIVE);
    }
}

void Model::AddTexture(uint32_t index, grfx::Texture* pTexture)
{
    SetTexture(index, pTexture);
    if (mTextures[index]) {
        mTextures[index]->SetOwnership(grfx::OWNERSHIP_EXCLUSIVE);
    }
}

} // namespace grfx
} // namespace ppx
