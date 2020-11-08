#include "ppx/grfx/grfx_command.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_draw_pass.h"
#include "ppx/grfx/grfx_image.h"
#include "ppx/grfx/grfx_model.h"
#include "ppx/grfx/grfx_render_pass.h"

namespace ppx {
namespace grfx {

void CommandBuffer::BeginRenderPass(const grfx::RenderPass* pRenderPass)
{
    PPX_ASSERT_NULL_ARG(pRenderPass);

    grfx::RenderPassBeginInfo beginInfo = {};
    beginInfo.pRenderPass               = pRenderPass;
    beginInfo.renderArea                = pRenderPass->GetRenderArea();

    beginInfo.RTVClearCount = pRenderPass->GetRenderTargetCount();
    for (uint32_t i = 0; i < beginInfo.RTVClearCount; ++i) {
        grfx::ImagePtr rtvImage     = pRenderPass->GetRenderTargetImage(i);
        beginInfo.RTVClearValues[i] = rtvImage->GetRTVClearValue();
    }

    grfx::ImagePtr dsvImage = pRenderPass->GetDepthStencilImage();
    if (dsvImage) {
        beginInfo.DSVClearValue = dsvImage->GetDSVClearValue();
    }

    BeginRenderPass(&beginInfo);
}

void CommandBuffer::BeginRenderPass(
    const grfx::DrawPass*           pDrawPass,
    const grfx::DrawPassClearFlags& clearFlags)
{
    PPX_ASSERT_NULL_ARG(pDrawPass);

    grfx::RenderPassBeginInfo beginInfo = {};
    pDrawPass->PrepareRenderPassBeginInfo(clearFlags, &beginInfo);

    BeginRenderPass(&beginInfo);
}

void CommandBuffer::TransitionImageLayout(
    grfx::RenderPass*   pRenderPass,
    grfx::ResourceState renderTargetBeforeState,
    grfx::ResourceState renderTargetAfterState,
    grfx::ResourceState depthStencilTargetBeforeState,
    grfx::ResourceState depthStencilTargetAfterState)
{
    PPX_ASSERT_NULL_ARG(pRenderPass);

    const uint32_t n = pRenderPass->GetRenderTargetCount();
    for (uint32_t i = 0; i < n; ++i) {
        grfx::ImagePtr renderTarget;
        Result         ppxres = pRenderPass->GetRenderTargetImage(i, &renderTarget);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "failed getting render pass render target");

        TransitionImageLayout(
            renderTarget,
            PPX_ALL_SUBRESOURCES,
            renderTargetBeforeState,
            renderTargetAfterState);
    }

    if (pRenderPass->HasDepthStencil()) {
        grfx::ImagePtr depthStencil;
        Result         ppxres = pRenderPass->GetDepthStencilImage(&depthStencil);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "failed getting render pass depth/stencil");

        TransitionImageLayout(
            depthStencil,
            PPX_ALL_SUBRESOURCES,
            depthStencilTargetBeforeState,
            depthStencilTargetAfterState);
    }
}

void CommandBuffer::TransitionImageLayout(
    grfx::DrawPass*     pDrawPass,
    grfx::ResourceState renderTargetBeforeState,
    grfx::ResourceState renderTargetAfterState,
    grfx::ResourceState depthStencilTargetBeforeState,
    grfx::ResourceState depthStencilTargetAfterState)
{
    PPX_ASSERT_NULL_ARG(pDrawPass);

    const uint32_t n = pDrawPass->GetRenderTargetCount();
    for (uint32_t i = 0; i < n; ++i) {
        grfx::TexturePtr renderTarget;
        Result           ppxres = pDrawPass->GetRenderTargetTexture(i, &renderTarget);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "failed getting draw pass render target");

        TransitionImageLayout(
            renderTarget->GetImage(),
            PPX_ALL_SUBRESOURCES,
            renderTargetBeforeState,
            renderTargetAfterState);
    }

    if (pDrawPass->HasDepthStencil()) {
        grfx::TexturePtr depthSencil;
        Result           ppxres = pDrawPass->GetDepthStencilTexture(&depthSencil);
        PPX_ASSERT_MSG(ppxres == ppx::SUCCESS, "failed getting draw pass depth/stencil");

        TransitionImageLayout(
            depthSencil->GetImage(),
            PPX_ALL_SUBRESOURCES,
            depthStencilTargetBeforeState,
            depthStencilTargetAfterState);
    }
}

void CommandBuffer::SetViewports(const grfx::Viewport& viewport)
{
    SetViewports(1, &viewport);
}

void CommandBuffer::SetScissors(const grfx::Rect& scissor)
{
    SetScissors(1, &scissor);
}

void CommandBuffer::BindIndexBuffer(const grfx::Buffer* pBuffer, grfx::IndexType indexType, uint64_t offset)
{
    PPX_ASSERT_NULL_ARG(pBuffer);

    grfx::IndexBufferView view = {};
    view.pBuffer               = pBuffer;
    view.indexType             = indexType;
    view.offset                = offset;

    BindIndexBuffer(&view);
}

void CommandBuffer::BindIndexBuffer(const grfx::Model* pModel, uint64_t offset)
{
    PPX_ASSERT_NULL_ARG(pModel);

    BindIndexBuffer(pModel->GetIndexBuffer(), pModel->GetIndexType(), offset);
}

void CommandBuffer::BindVertexBuffers(
    uint32_t                   bufferCount,
    const grfx::Buffer* const* ppBuffers,
    const uint32_t*            pStrides,
    const uint64_t*            pOffsets)
{
    PPX_ASSERT_NULL_ARG(ppBuffers);
    PPX_ASSERT_NULL_ARG(pStrides);
    PPX_ASSERT_MSG(bufferCount < PPX_MAX_VERTEX_BINDINGS, "bufferCount exceeds PPX_MAX_VERTEX_ATTRIBUTES");

    grfx::VertexBufferView views[PPX_MAX_VERTEX_BINDINGS] = {};
    for (uint32_t i = 0; i < bufferCount; ++i) {
        views[i].pBuffer = ppBuffers[i];
        views[i].stride  = pStrides[i];
        if (!IsNull(pOffsets)) {
            views[i].offset = pOffsets[i];
        }
    }

    BindVertexBuffers(bufferCount, views);
}

void CommandBuffer::BindVertexBuffers(const grfx::Model* pModel, const uint64_t* pOffsets)
{
    PPX_ASSERT_NULL_ARG(pModel);

    const grfx::Buffer* buffers[PPX_MAX_VERTEX_BINDINGS] = {nullptr};
    uint32_t            strides[PPX_MAX_VERTEX_BINDINGS] = {0};

    uint32_t bufferCount = pModel->GetVertexBufferCount();
    for (uint32_t i = 0; i < bufferCount; ++i) {
        buffers[i] = pModel->GetVertexBuffer(i);
        strides[i] = pModel->GetVertexBinding(i)->GetStride();
    }

    BindVertexBuffers(bufferCount, buffers, strides, pOffsets);
}

} // namespace grfx
} // namespace ppx
