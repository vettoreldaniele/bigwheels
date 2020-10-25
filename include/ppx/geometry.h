#ifndef ppx_geometry_h
#define ppx_geometry_h

#include "ppx/mesh.h"
#include "ppx/grfx/000_grfx_config.h"

namespace ppx {

enum GeometryAttributeLayout
{
    GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED = 1,
    GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR      = 2,
};

//! @struct GeometryCreateInfo
//!
//! primtiveTopolgy
//!   - only TRIANGLE_LIST is currently supported
//!
//! indexType
//!   - use UNDEFINED to indicate there's not index data
//!
//! attributeLayout
//!   - if INTERLEAVED only bindings[0] is used
//!   - if PLANAR bindings[0..bindingCount] are used
//!   - if PLANAR bindings can only have 1 attribute
//!
struct GeometryCreateInfo
{
    GeometryAttributeLayout attributeLayout                         = ppx::GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED;
    grfx::IndexType         indexType                               = grfx::INDEX_TYPE_UNDEFINED;
    uint32_t                vertexBindingCount                      = 0;
    grfx::VertexBinding     vertexBindings[PPX_MAX_VERTEX_BINDINGS] = {};
    grfx::PrimitiveTopology primtiveTopolgy                         = grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    // Creates a create info objects with a UINT16 or UINT32 index
    // type and position vertex attribute.
    //
    static GeometryCreateInfo InterleavedU16();
    static GeometryCreateInfo InterleavedU32();
    static GeometryCreateInfo PlanarU16();
    static GeometryCreateInfo PlanarU32();

    // Create a create info with a position vertex attribute.
    //
    static GeometryCreateInfo Interleaved();
    static GeometryCreateInfo Planar();

    // NOTE: Vertex input locations (Vulkan) are based on the order of
    //       when the attribute is added.
    //
    //       Example (assumes no attributes exist):
    //         AddPosition(); // location = 0
    //         AddColor();    // location = 1
    //
    //       Example (2 attributes exist):
    //         AddTexCoord(); // location = 2
    //         AddTangent();  // location = 3
    //
    // WARNING: Changing the attribute layout, index type, or messing
    //          with the vertex bindings after or in between calling
    //          these functions can result in undefined behavior.
    //
    GeometryCreateInfo& AddPosition(grfx::Format format = grfx::FORMAT_R32G32B32_FLOAT);
    GeometryCreateInfo& AddNormal(grfx::Format format = grfx::FORMAT_R32G32B32_FLOAT);
    GeometryCreateInfo& AddColor(grfx::Format format = grfx::FORMAT_R32G32B32_FLOAT);
    GeometryCreateInfo& AddTexCoord(grfx::Format format = grfx::FORMAT_R32G32_FLOAT);
    GeometryCreateInfo& AddTangent(grfx::Format format = grfx::FORMAT_R32G32B32_FLOAT);
    GeometryCreateInfo& AddBitangent(grfx::Format format = grfx::FORMAT_R32G32B32_FLOAT);

private:
    GeometryCreateInfo& AddAttribute(grfx::VertexSemantic semantic, grfx::Format format);
};

//! @class Geometry
//!
//!
class Geometry
{
public:
    enum BufferType
    {
        BUFFER_TYPE_VERTEX = 1,
        BUFFER_TYPE_INDEX  = 2,
    };

    //! @class Buffer
    //!
    //! Element count is data size divided by element size.
    //! Example:
    //!   - If  buffer is storing 16 bit indices then element size
    //!     is 2 bytes. Dividing the value of GetSize() by 2 will
    //!     return the element count, i.e. the number of indices.
    //!
    //! There's two different ways to use Buffer. Mixing them will most
    //! likely lead to undefined behavior.
    //!
    //! Use #1:
    //!   - Create a buffer object with type and element size
    //!   - Call SetSize() to allocate storage
    //!   - Grab the pointer with GetData() and memcpy to buffer object
    //!
    //! Use #2:
    //!   - Create a buffer object with type and element size
    //!   - Call Append<T>() to append data to it
    //!
    class Buffer
    {
    public:
        Buffer() {}
        Buffer(BufferType type, uint32_t elementSize)
            : mType(type), mElementSize(elementSize) {}
        ~Buffer() {}

        BufferType  GetType() const { return mType; }
        uint32_t    GetElementSize() const { return mElementSize; }
        uint32_t    GetSize() const { return CountU32(mData); }
        void        SetSize(uint32_t size) { mData.resize(size); }
        char*       GetData() { return DataPtr(mData); }
        const char* GetData() const { return DataPtr(mData); }
        uint32_t    GetElementCount() const;

        // Trusts that calling code is well behaved :)
        //
        template <typename T>
        void Append(const T& value)
        {
            uint32_t sizeOfValue = static_cast<uint32_t>(sizeof(T));

            // Current size
            size_t offset = mData.size();
            // Allocate storage for incoming data
            mData.resize(offset + sizeOfValue);
            // Copy data
            const void* pSrc = &value;
            void*       pDst = mData.data() + offset;
            memcpy(pDst, pSrc, sizeOfValue);
        }

    private:
        BufferType        mType        = BUFFER_TYPE_VERTEX;
        uint32_t          mElementSize = 0;
        std::vector<char> mData;
    };

    // ---------------------------------------------------------------------------------------------
public:
    Geometry() {}
    virtual ~Geometry() {}

private:
    Result InternalCtor();

public:
    // Create object using parameters from createInfo
    static Result Create(const GeometryCreateInfo& createInfo, Geometry* pGeometry);

    // Create object using parameters from createInfo using data from mesh
    static Result Create(
        const GeometryCreateInfo& createInfo,
        const TriMesh&            mesh,
        Geometry*                 pGeometry);

    // Create object with a create info derived from mesh
    static Result Create(const TriMesh& mesh, Geometry* pGeomtry);

    grfx::IndexType            GetIndexType() const { return mCreateInfo.indexType; }
    const Geometry::Buffer*    GetIndexBuffer() const { return &mIndexBuffer; }
    uint32_t                   GetVertexBufferCount() const { return CountU32(mVertexBuffers); }
    const Geometry::Buffer*    GetVertxBuffer(uint32_t index) const;
    const grfx::VertexBinding* GetVertexBinding(uint32_t index) const;
    uint32_t                   GetBiggestBufferSize() const;

    // Appends triangle vertex indices to index buffer
    //
    // Will cast to uint16_t if geometry index type is UINT16.
    // NOOP if index type is UNDEFINED (geometry does not have index data).
    //
    void AppendIndicesTriangle(uint32_t vtx0, uint32_t vtx1, uint32_t vtx2);

    // Append multiple attributes at once
    //
    uint32_t AppendVertexData(const VertexData& vtx);

    // Appends triangle vertex data and indices (if present)
    //
    //
    void AppendTriangle(const VertexData& vtx0, const VertexData& vtx1, const VertexData& vtx2);

    // Append individual attributes
    //
    // For attribute layout GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR only, will NOOP
    // otherwise.
    //
    // Missing attributes will also result in NOOP. For instance if a geomtry
    // object was not created with color, calling AppendColor() will NOOP.
    //
    uint32_t AppendPosition(const float3& value);
    void     AppendNormal(const float3& value);
    void     AppendColor(const float3& value);
    void     AppendTexCoord(const float2& value);
    void     AppendTangent(const float3& value);
    void     AppendBitangent(const float3& value);

private:
    uint32_t AppendVertexInterleaved(const VertexData& vtx);

private:
    GeometryCreateInfo            mCreateInfo = {};
    Geometry::Buffer              mIndexBuffer;
    std::vector<Geometry::Buffer> mVertexBuffers;
    uint32_t                      mPositionBufferIndex  = PPX_VALUE_IGNORED;
    uint32_t                      mNormaBufferIndex     = PPX_VALUE_IGNORED;
    uint32_t                      mColorBufferIndex     = PPX_VALUE_IGNORED;
    uint32_t                      mTexCoordBufferIndex  = PPX_VALUE_IGNORED;
    uint32_t                      mTangentBufferIndex   = PPX_VALUE_IGNORED;
    uint32_t                      mBitangentBufferIndex = PPX_VALUE_IGNORED;
};

} // namespace ppx

#endif // ppx_geometry_h
