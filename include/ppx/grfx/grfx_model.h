#ifndef ppx_grfx_model_h
#define ppx_grfx_model_h

#include "ppx/grfx/000_grfx_config.h"
#include "ppx/grfx/grfx_buffer.h"
#include "ppx/grfx/grfx_texture.h"
#include "ppx/geometry.h"

namespace ppx {
namespace grfx {

//! @struct ModelCreateInfo
//!
//! NOTE: *ALL* fields are optional.
//!
struct ModelCreateInfo
{
    // If pIndexBuffer is NOT NULL:
    //   - Create parameters are ignored
    //
    //! indexType
    //!   - use UNDEFINED to indicate there's not index data
    //
    // Usage flags:
    //   - grfx::BUFFER_USAGE_INDEX_BUFFER is always set
    //   - grfx::BUFFER_USAGE_TRANSFER_DST is always set
    //
    grfx::Buffer*          pIndexBuffer           = nullptr;
    grfx::IndexType        indexType              = grfx::INDEX_TYPE_UNDEFINED;
    uint64_t               indexBufferSize        = 0;
    grfx::BufferUsageFlags indexBufferUsageFlags  = {};
    grfx::MemoryUsage      indexBufferMemoryUsage = grfx::MEMORY_USAGE_GPU_ONLY;

    // The limits here only apply to the creation process.
    // grfx::Model objects can have as many vertex buffers
    // as the application as it wants using the the relevant
    // Set*() and/or Add*() for vertex bindings and buffers.
    //
    uint32_t vertexBufferCount = 0;
    struct
    {
        // If pBuffer is NOT NULL
        //   - Create parameters are ignored
        //   - binding is not a creation parameter
        //
        // Usage flags:
        //   - grfx::BUFFER_USAGE_VERTEX_BUFFER is always set
        //   - grfx::BUFFER_USAGE_TRANSFER_DST is always set
        //
        grfx::Buffer*          pBuffer     = nullptr;
        grfx::VertexBinding    binding     = {};
        uint64_t               size        = 0;
        grfx::BufferUsageFlags usageFlags  = {};
        grfx::MemoryUsage      memoryUsage = grfx::MEMORY_USAGE_GPU_ONLY;
    } vertexBuffers[PPX_MAX_VERTEX_BINDINGS];

    // The limits here only apply to the creation process.
    // grfx::Model objects can have as many textures
    // as the application as it wants using the the relevant
    // Set*() and/or Add*() for textures.
    //
    uint32_t textureCount = 0;
    struct
    {
        // If pTexture is NOT NULL:
        //   - Create parameters are ignored
        //
        // Usage flags:
        //   - grfx::IMAGE_USAGE_SAMPLED is always set
        //   - grfx::IMAGE_USAGE_TRANSFER_DST is always set
        //   - sample count is always 1
        //
        grfx::Texture*               pTexture        = nullptr;
        grfx::ImageType              imageType       = grfx::IMAGE_TYPE_2D;
        uint32_t                     width           = 0;
        uint32_t                     height          = 0;
        grfx::Format                 format          = grfx::FORMAT_UNDEFINED;
        uint32_t                     mipLevelCount   = 1;
        uint32_t                     arrayLayerCount = 1;
        grfx::ImageUsageFlags        usageFlags      = {};
        grfx::MemoryUsage            memoryUsage     = grfx::MEMORY_USAGE_GPU_ONLY;
        grfx::RenderTargetClearValue RTVClearValue   = {0, 0, 0, 0}; // Optimized RTV clear value
        grfx::DepthStencilClearValue DSVClearValue   = {1.0f, 0xFF}; // Optimized DSV clear value
    } textures[PPX_MAX_MODEL_TEXTURES_IN_CREATE_INFO];


    ModelCreateInfo() {}
    ModelCreateInfo(const ppx::Geometry& geometry);
};

//! @class Model
//!
//!
class Model
    : public grfx::DeviceObject<grfx::ModelCreateInfo>
{
public:
    Model() {}
    virtual ~Model() {}

    // Binding, buffer, and texture objects will return empty or nullptr
    // if index is out of range.
    //
    grfx::IndexType            GetIndexType() const { return mIndexType; }
    grfx::BufferPtr            GetIndexBuffer() const { return mIndexBuffer; }
    uint32_t                   GetVertexBufferCount() const { return CountU32(mVertexBuffers); }
    grfx::VertexBinding*       GetVertexBinding(uint32_t index);
    const grfx::VertexBinding* GetVertexBinding(uint32_t index) const;
    grfx::BufferPtr            GetVertexBuffer(uint32_t index) const;
    uint32_t                   GetTextureCount() const { return CountU32(mTextures); }
    grfx::TexturePtr           GetTexture(uint32_t index) const;

    //
    // What's the difference between Set*() and Add*()?
    //   - Set*() sets the ownership to OWNERSHIP_REFERENCE and will not
    //     destroy these objects during destruction.
    //   - Add*() sets the ownership to OWNERSHIP_EXCLUSIVE and will
    //     destroy these objects during destruction.
    //   - Owership rules do not apply to index type or vertex bindings
    //

    void SetIndexType(grfx::IndexType indexType) { mIndexType = indexType; }
    void SetIndexBuffer(grfx::Buffer* pBuffer);
    void SetVertexBuffer(uint32_t index, grfx::Buffer* pBuffer);
    void SetVertexBinding(uint32_t index, const grfx::VertexBinding& binding);
    void SetVertexBuffer(uint32_t index, const grfx::VertexBinding& binding, grfx::Buffer* pBuffer);
    void SetTexture(uint32_t index, grfx::Texture* pTexture);

    void AddIndexBuffer(grfx::Buffer* pBuffer);
    void AddVertexBuffer(grfx::Buffer* pBuffer);
    void AddVertexBinding(const grfx::VertexBinding& binding);
    void AddVertexBuffer(const grfx::VertexBinding& binding, grfx::Buffer* pBuffer);
    void AddVertexBuffer(uint32_t index, grfx::Buffer* pBuffer);
    void AddVertexBinding(uint32_t index, const grfx::VertexBinding& binding);
    void AddVertexBuffer(uint32_t index, const grfx::VertexBinding& binding, grfx::Buffer* pBuffer);
    void AddTexture(uint32_t index, grfx::Texture* pTexture);

private:
    void CheckedDestroy(grfx::Buffer* pBuffer);
    void CheckedDestroy(grfx::Texture* pTexture);

protected:
    virtual Result CreateApiObjects(const grfx::ModelCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

protected:
    grfx::IndexType mIndexType = grfx::INDEX_TYPE_UINT16;
    grfx::BufferPtr mIndexBuffer;

    struct VertexBuffer
    {
        grfx::VertexBinding binding;
        grfx::BufferPtr     buffer;
    };
    std::vector<VertexBuffer> mVertexBuffers;

    std::vector<grfx::TexturePtr> mTextures;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_model_h
