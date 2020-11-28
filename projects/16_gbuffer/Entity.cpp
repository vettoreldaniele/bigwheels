#include "Entity.h"
#include "Render.h"

#include "ppx/grfx/grfx_descriptor.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/application.h"
#include "ppx/graphics_util.h"
using namespace ::ppx;

ppx::grfx::DescriptorSetLayoutPtr Entity::sModelDataLayout;
ppx::grfx::VertexDescription      Entity::sVertexDescription;
ppx::grfx::PipelineInterfacePtr   Entity::sPipelineInterface;
ppx::grfx::GraphicsPipelinePtr    Entity::sPipeline;

ppx::Result Entity::Create(ppx::grfx::Queue* pQueue, ppx::grfx::DescriptorPool* pPool, const EntityCreateInfo* pCreateInfo)
{
    PPX_ASSERT_NULL_ARG(pQueue);
    PPX_ASSERT_NULL_ARG(pPool);
    PPX_ASSERT_NULL_ARG(pCreateInfo);

    Result ppxres = ppx::ERROR_FAILED;

    grfx::Device* pDevice = pQueue->GetDevice();

    mModel    = pCreateInfo->pModel;
    mMaterial = pCreateInfo->pMaterial;

    // Model constants
    {
        grfx::BufferCreateInfo bufferCreateInfo      = {};
        bufferCreateInfo.size                        = PPX_MINIUM_CONSTANT_BUFFER_SIZE;
        bufferCreateInfo.usageFlags.bits.transferSrc = true;
        bufferCreateInfo.memoryUsage                 = grfx::MEMORY_USAGE_CPU_TO_GPU;
        PPX_CHECKED_CALL(ppxres = pDevice->CreateBuffer(&bufferCreateInfo, &mCpuModelConstants));

        bufferCreateInfo.usageFlags.bits.transferDst   = true;
        bufferCreateInfo.usageFlags.bits.uniformBuffer = true;
        bufferCreateInfo.memoryUsage                   = grfx::MEMORY_USAGE_GPU_ONLY;
        PPX_CHECKED_CALL(ppxres = pDevice->CreateBuffer(&bufferCreateInfo, &mGpuModelConstants));
    }

    // Allocate descriptor set
    {
        PPX_CHECKED_CALL(ppxres = pDevice->AllocateDescriptorSet(pPool, sModelDataLayout, &mModelDataSet));
    }

    // Update descriptors
    {
        grfx::WriteDescriptor write = {};
        write.binding               = MODEL_CONSTANTS_REGISTER;
        write.type                  = grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.bufferOffset          = 0;
        write.bufferRange           = PPX_WHOLE_SIZE;
        write.pBuffer               = mGpuModelConstants;
        PPX_CHECKED_CALL(ppxres = mModelDataSet->UpdateDescriptors(1, &write));
    }

    return ppx::SUCCESS;
}

void Entity::Destroy()
{
}

ppx::Result Entity::CreatePipelines(ppx::grfx::DescriptorSetLayout* pSceneDataLayout, ppx::grfx::DrawPass* pDrawPass)
{
    PPX_ASSERT_NULL_ARG(pSceneDataLayout);

    Result ppxres = ppx::ERROR_FAILED;

    grfx::Device* pDevice = pSceneDataLayout->GetDevice();

    // Model data
    {
        grfx::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.bindings.push_back({grfx::DescriptorBinding{MODEL_CONSTANTS_REGISTER, grfx::DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, grfx::SHADER_STAGE_ALL_GRAPHICS}});
        PPX_CHECKED_CALL(ppxres = pDevice->CreateDescriptorSetLayout(&createInfo, &sModelDataLayout));
    }

    // Pipeline interface
    {
        grfx::PipelineInterfaceCreateInfo createInfo = {};
        createInfo.setCount                          = 4;
        createInfo.sets[0].set                       = 0;
        createInfo.sets[0].pLayout                   = pSceneDataLayout;
        createInfo.sets[1].set                       = 1;
        createInfo.sets[1].pLayout                   = Material::GetMaterialResourcesLayout();
        createInfo.sets[2].set                       = 2;
        createInfo.sets[2].pLayout                   = Material::GetMaterialDataLayout();
        createInfo.sets[3].set                       = 3;
        createInfo.sets[3].pLayout                   = sModelDataLayout;

        PPX_CHECKED_CALL(ppxres = pDevice->CreatePipelineInterface(&createInfo, &sPipelineInterface));
    }

    // Pipeline
    {
        Application* pApp = Application::Get();

        grfx::ShaderModulePtr VS;
        std::vector<char>     bytecode = pApp->LoadShader(pApp->GetAssetPath("gbuffer/shaders"), "VertexShader.vs");
        PPX_ASSERT_MSG(!bytecode.empty(), "VS shader bytecode load failed");
        grfx::ShaderModuleCreateInfo shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = pDevice->CreateShaderModule(&shaderCreateInfo, &VS));

        grfx::ShaderModulePtr PS;
        bytecode = pApp->LoadShader(pApp->GetAssetPath("gbuffer/shaders"), "DeferredRender.ps");
        PPX_ASSERT_MSG(!bytecode.empty(), "PS shader bytecode load failed");
        shaderCreateInfo = {static_cast<uint32_t>(bytecode.size()), bytecode.data()};
        PPX_CHECKED_CALL(ppxres = pDevice->CreateShaderModule(&shaderCreateInfo, &PS));

        const grfx::VertexInputRate inputRate = grfx::VERTEX_INPUT_RATE_VERTEX;
        grfx::VertexDescription     vertexDescription;
        // clang-format off
        vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_POSITION , 0, grfx::FORMAT_R32G32B32_FLOAT, 0, PPX_APPEND_OFFSET_ALIGNED, inputRate});
        vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_COLOR    , 1, grfx::FORMAT_R32G32B32_FLOAT, 1, PPX_APPEND_OFFSET_ALIGNED, inputRate});
        vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_NORMAL   , 2, grfx::FORMAT_R32G32B32_FLOAT, 2, PPX_APPEND_OFFSET_ALIGNED, inputRate});
        vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_TEXCOORD , 3, grfx::FORMAT_R32G32_FLOAT,    3, PPX_APPEND_OFFSET_ALIGNED, inputRate});
        vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_TANGENT  , 4, grfx::FORMAT_R32G32B32_FLOAT, 4, PPX_APPEND_OFFSET_ALIGNED, inputRate});
        vertexDescription.AppendBinding(grfx::VertexAttribute{PPX_SEMANTIC_NAME_BITANGENT, 5, grfx::FORMAT_R32G32B32_FLOAT, 5, PPX_APPEND_OFFSET_ALIGNED, inputRate});
        // clang-format on

        grfx::GraphicsPipelineCreateInfo2 gpCreateInfo = {};
        gpCreateInfo.VS                                = {VS.Get(), "vsmain"};
        gpCreateInfo.PS                                = {PS.Get(), "psmain"};
        gpCreateInfo.topology                          = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        gpCreateInfo.polygonMode                       = grfx::POLYGON_MODE_FILL;
        gpCreateInfo.cullMode                          = grfx::CULL_MODE_BACK;
        gpCreateInfo.frontFace                         = grfx::FRONT_FACE_CCW;
        gpCreateInfo.depthReadEnable                   = true;
        gpCreateInfo.depthWriteEnable                  = true;
        gpCreateInfo.pPipelineInterface                = sPipelineInterface;
        gpCreateInfo.outputState.depthStencilFormat    = pDrawPass->GetDepthStencilTexture()->GetImage()->GetFormat();
        // Render target
        gpCreateInfo.outputState.renderTargetCount = pDrawPass->GetRenderTargetCount();
        for (uint32_t i = 0; i < gpCreateInfo.outputState.renderTargetCount; ++i) {
            gpCreateInfo.blendModes[i]                      = grfx::BLEND_MODE_NONE;
            gpCreateInfo.outputState.renderTargetFormats[i] = pDrawPass->GetRenderTargetTexture(i)->GetImage()->GetFormat();
        }
        // Vertex description
        gpCreateInfo.vertexInputState.bindingCount = vertexDescription.GetBindingCount();
        for (uint32_t i = 0; i < vertexDescription.GetBindingCount(); ++i) {
            gpCreateInfo.vertexInputState.bindings[i] = *vertexDescription.GetBinding(i);
        }

        PPX_CHECKED_CALL(ppxres = pDevice->CreateGraphicsPipeline(&gpCreateInfo, &sPipeline));
        pDevice->DestroyShaderModule(VS);
        pDevice->DestroyShaderModule(PS);
    }

    return ppx::SUCCESS;
}

void Entity::DestroyPipelines()
{
}

void Entity::UpdateConstants(ppx::grfx::Queue* pQueue)
{
    Result ppxres = ppx::ERROR_FAILED;

    const float4x4& M = mTransform.GetConcatenatedMatrix();

    PPX_HLSL_PACK_BEGIN();
    struct HlslModelData
    {
        hlsl_float4x4<64> modelMatrix;
        hlsl_float4x4<64> normalMatrix;
        hlsl_float3<12>   debugColor;
    };
    PPX_HLSL_PACK_END();

    void* pMappedAddress = nullptr;
    PPX_CHECKED_CALL(ppxres = mCpuModelConstants->MapMemory(0, &pMappedAddress));

    HlslModelData* pModelData = static_cast<HlslModelData*>(pMappedAddress);
    pModelData->modelMatrix   = M;
    pModelData->normalMatrix  = glm::inverseTranspose(M);

    mCpuModelConstants->UnmapMemory();

    grfx::BufferToBufferCopyInfo copyInfo = {mCpuModelConstants->GetSize()};
    PPX_CHECKED_CALL(ppxres = pQueue->CopyBufferToBuffer(&copyInfo, mCpuModelConstants, mGpuModelConstants));
}

void Entity::Draw(ppx::grfx::DescriptorSet* pSceneDataSet, ppx::grfx::CommandBuffer* pCmd)
{
    grfx::DescriptorSet* sets[4] = {nullptr};
    sets[0]                      = pSceneDataSet;
    sets[1]                      = mMaterial->GetMaterialResourceSet();
    sets[2]                      = mMaterial->GetMaterialDataSet();
    sets[3]                      = mModelDataSet;
    pCmd->BindGraphicsDescriptorSets(sPipelineInterface, 4, sets);

    pCmd->BindGraphicsPipeline(sPipeline);

    pCmd->BindIndexBuffer(mModel);
    pCmd->BindVertexBuffers(mModel);
    pCmd->DrawIndexed(mModel->GetIndexCount());
}
