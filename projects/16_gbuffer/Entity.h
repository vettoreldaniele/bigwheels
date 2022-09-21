#ifndef ENTITY_H
#define ENTITY_H

#include "ppx/grfx/grfx_mesh.h"
#include "ppx/grfx/grfx_pipeline.h"
#include "ppx/transform.h"

#include "Material.h"

struct EntityCreateInfo
{
    ppx::grfx::Mesh* pMesh     = nullptr;
    const Material*  pMaterial = nullptr;
};

class Entity
{
public:
    Entity() {}
    ~Entity() {}

    ppx::Result Create(ppx::grfx::Queue* pQueue, ppx::grfx::DescriptorPool* pPool, const EntityCreateInfo* pCreateInfo);
    void        Destroy();

    static ppx::Result CreatePipelines(ppx::grfx::DescriptorSetLayout* pSceneDataLayout, ppx::grfx::DrawPass* pDrawPass);
    static void        DestroyPipelines();

    ppx::Transform&       GetTransform() { return mTransform; }
    const ppx::Transform& GetTransform() const { return mTransform; }

    void UpdateConstants(ppx::grfx::Queue* pQueue);
    void Draw(ppx::grfx::DescriptorSet* pSceneDataSet, ppx::grfx::CommandBuffer* pCmd);

private:
    ppx::Transform              mTransform;
    ppx::grfx::MeshPtr          mMesh;
    const Material*             mMaterial = nullptr;
    ppx::grfx::BufferPtr        mCpuModelConstants;
    ppx::grfx::BufferPtr        mGpuModelConstants;
    ppx::grfx::DescriptorSetPtr mModelDataSet;

    static ppx::grfx::DescriptorSetLayoutPtr sModelDataLayout;
    static ppx::grfx::VertexDescription      sVertexDescription;
    static ppx::grfx::PipelineInterfacePtr   sPipelineInterface;
    static ppx::grfx::GraphicsPipelinePtr    sPipeline;
};

#endif // ENTITY_H
