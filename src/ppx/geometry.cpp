#include "ppx/geometry.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define NOT_INTERLEAVED_MSG "cannot append interleaved data if attribute layout is not interleaved"
#define NOT_PLANAR_MSG      "cannot append planar data if attribute layout is not planar"

namespace ppx {

// -------------------------------------------------------------------------------------------------
// GeometryCreateInfo
// -------------------------------------------------------------------------------------------------
GeometryCreateInfo GeometryCreateInfo::InterleavedU16()
{
    GeometryCreateInfo ci = {};
    ci.attributeLayout    = GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED;
    ci.indexType          = grfx::INDEX_TYPE_UINT16;
    ci.vertexBindingCount = 1; // Interleave attrbute layout always has 1 vertex binding
    ci.AddPosition();
    return ci;
}

GeometryCreateInfo GeometryCreateInfo::InterleavedU32()
{
    GeometryCreateInfo ci = {};
    ci.attributeLayout    = GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED;
    ci.indexType          = grfx::INDEX_TYPE_UINT32;
    ci.vertexBindingCount = 1; // Interleave attrbute layout always has 1 vertex binding
    ci.AddPosition();
    return ci;
}

GeometryCreateInfo GeometryCreateInfo::PlanarU16()
{
    GeometryCreateInfo ci = {};
    ci.attributeLayout    = GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR;
    ci.indexType          = grfx::INDEX_TYPE_UINT16;
    ci.AddPosition();
    return ci;
}

GeometryCreateInfo GeometryCreateInfo::PlanarU32()
{
    GeometryCreateInfo ci = {};
    ci.attributeLayout    = GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR;
    ci.indexType          = grfx::INDEX_TYPE_UINT32;
    ci.AddPosition();
    return ci;
}

GeometryCreateInfo GeometryCreateInfo::Interleaved()
{
    GeometryCreateInfo ci = {};
    ci.attributeLayout    = GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED;
    ci.indexType          = grfx::INDEX_TYPE_UNDEFINED;
    ci.vertexBindingCount = 1; // Interleave attrbute layout always has 1 vertex binding
    ci.AddPosition();
    return ci;
}

GeometryCreateInfo GeometryCreateInfo::Planar()
{
    GeometryCreateInfo ci = {};
    ci.attributeLayout    = GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR;
    ci.indexType          = grfx::INDEX_TYPE_UNDEFINED;
    ci.AddPosition();
    return ci;
}

GeometryCreateInfo& GeometryCreateInfo::AddAttribute(grfx::VertexSemantic semantic, grfx::Format format)
{
    bool exists = false;
    for (uint32_t bindingIndex = 0; bindingIndex < vertexBindingCount; ++bindingIndex) {
        const grfx::VertexBinding& binding = vertexBindings[bindingIndex];
        for (uint32_t attrIndex = 0; attrIndex < binding.GetAttributeCount(); ++attrIndex) {
            const grfx::VertexAttribute* pAttribute = nullptr;
            binding.GetAttribute(attrIndex, &pAttribute);
            exists = (pAttribute->semantic == semantic);
            if (exists) {
                break;
            }
        }
        if (exists) {
            break;
        }
    }

    if (!exists) {
        uint32_t location = 0;
        for (uint32_t bindingIndex = 0; bindingIndex < vertexBindingCount; ++bindingIndex) {
            const grfx::VertexBinding& binding = vertexBindings[bindingIndex];
            location += binding.GetAttributeCount();
        }

        grfx::VertexAttribute attribute = {};
        attribute.semanticName          = ToString(semantic);
        attribute.location              = location;
        attribute.format                = format;
        attribute.binding               = PPX_VALUE_IGNORED; // Determined below
        attribute.offset                = PPX_APPEND_OFFSET_ALIGNED;
        attribute.inputRate             = grfx::VERTEX_INPUT_RATE_VERTEX;
        attribute.semantic              = semantic;

        if (attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED) {
            attribute.binding = 0;
            vertexBindings[0].AppendAttribute(attribute);
        }
        else if (attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
            PPX_ASSERT_MSG(vertexBindingCount < PPX_MAX_VERTEX_BINDINGS, "max vertex bindings exceeded");

            vertexBindings[vertexBindingCount].AppendAttribute(attribute);
            vertexBindings[vertexBindingCount].SetBinding(vertexBindingCount);
            vertexBindingCount += 1;
        }
    }
    return *this;
}

GeometryCreateInfo& GeometryCreateInfo::AddPosition(grfx::Format format)
{
    AddAttribute(grfx::VERTEX_SEMANTIC_POSITION, format);
    return *this;
}

GeometryCreateInfo& GeometryCreateInfo::AddNormal(grfx::Format format)
{
    AddAttribute(grfx::VERTEX_SEMANTIC_NORMAL, format);
    return *this;
}

GeometryCreateInfo& GeometryCreateInfo::AddColor(grfx::Format format)
{
    AddAttribute(grfx::VERTEX_SEMANTIC_COLOR, format);
    return *this;
}

GeometryCreateInfo& GeometryCreateInfo::AddTexCoord(grfx::Format format)
{
    AddAttribute(grfx::VERTEX_SEMANTIC_TEXCOORD, format);
    return *this;
}

GeometryCreateInfo& GeometryCreateInfo::AddTangent(grfx::Format format)
{
    AddAttribute(grfx::VERTEX_SEMANTIC_TANGENT, format);
    return *this;
}

GeometryCreateInfo& GeometryCreateInfo::AddBitangent(grfx::Format format)
{
    AddAttribute(grfx::VERTEX_SEMANTIC_BITANGENT, format);
    return *this;
}

// -------------------------------------------------------------------------------------------------
// Geometry::Buffer
// -------------------------------------------------------------------------------------------------
uint32_t Geometry::Buffer::GetElementCount() const
{
    size_t   sizeOfData = mData.size();
    uint32_t count      = static_cast<uint32_t>(sizeOfData / mElementSize);
    return count;
}

// -------------------------------------------------------------------------------------------------
// Geometry
// -------------------------------------------------------------------------------------------------
Result Geometry::InternalCtor()
{
    if (mCreateInfo.indexType != grfx::INDEX_TYPE_UNDEFINED) {
        uint32_t elementSize = grfx::IndexTypeSize(mCreateInfo.indexType);

        if (elementSize == 0) {
            // Shouldn't occur unless there's corruption
            PPX_ASSERT_MSG(false, "could not determine index type size");
            return ppx::ERROR_FAILED;
        }

        mIndexBuffer = Buffer(BUFFER_TYPE_INDEX, elementSize);
    }

    if (mCreateInfo.attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED) {
        mCreateInfo.vertexBindingCount = 1;

        const grfx::VertexBinding& binding     = mCreateInfo.vertexBindings[0];
        uint32_t                   elementSize = binding.GetStride();
        mVertexBuffers.push_back(Buffer(BUFFER_TYPE_VERTEX, elementSize));
    }
    else if (mCreateInfo.attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        // Create buffers
        for (uint32_t i = 0; i < mCreateInfo.vertexBindingCount; ++i) {
            const grfx::VertexBinding& binding     = mCreateInfo.vertexBindings[i];
            uint32_t                   elementSize = binding.GetStride();
            mVertexBuffers.push_back(Buffer(BUFFER_TYPE_VERTEX, elementSize));
        }

        // Cache buffer indices if possible
        for (uint32_t i = 0; i < mCreateInfo.vertexBindingCount; ++i) {
            const grfx::VertexBinding&   binding    = mCreateInfo.vertexBindings[i];
            const grfx::VertexAttribute* pAttribute = nullptr;
            bool                         found      = binding.GetAttribute(0, &pAttribute);
            if (!found) {
                // Shouldn't occur unless there's corruption
                PPX_ASSERT_MSG(false, "could not get attribute at index 0");
                return ppx::ERROR_FAILED;
            }

            // clang-format off
            switch (pAttribute->semantic) {
                default: break;
                case grfx::VERTEX_SEMANTIC_POSITION  : mPositionBufferIndex  = i; break;
                case grfx::VERTEX_SEMANTIC_NORMAL    : mNormaBufferIndex     = i; break;
                case grfx::VERTEX_SEMANTIC_COLOR     : mColorBufferIndex     = i; break;
                case grfx::VERTEX_SEMANTIC_TANGENT   : mTangentBufferIndex   = i; break;
                case grfx::VERTEX_SEMANTIC_BITANGENT : mBitangentBufferIndex = i; break;
                case grfx::VERTEX_SEMANTIC_TEXCOORD  : mTexCoordBufferIndex  = i; break;
            }
            // clang-format on
        }
    }

    return ppx::SUCCESS;
}

Result Geometry::Create(const GeometryCreateInfo& createInfo, Geometry* pGeometry)
{
    PPX_ASSERT_NULL_ARG(pGeometry);

    if (createInfo.primtiveTopolgy != grfx::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST) {
        PPX_ASSERT_MSG(false, "only triangle list is supported");
        return ppx::ERROR_INVALID_CREATE_ARGUMENT;
    }

    if (createInfo.indexType != grfx::INDEX_TYPE_UNDEFINED) {
        uint32_t elementSize = 0;
        if (createInfo.indexType == grfx::INDEX_TYPE_UINT16) {
            elementSize = sizeof(uint16_t);
        }
        else if (createInfo.indexType == grfx::INDEX_TYPE_UINT32) {
            elementSize = sizeof(uint32_t);
        }
        else {
            PPX_ASSERT_MSG(false, "invalid index type");
            return ppx::ERROR_INVALID_CREATE_ARGUMENT;
        }
    }

    if (createInfo.vertexBindingCount == 0) {
        PPX_ASSERT_MSG(false, "must have at least one vertex binding");
        return ppx::ERROR_INVALID_CREATE_ARGUMENT;
    }

    if (createInfo.attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        for (uint32_t i = 0; i < createInfo.vertexBindingCount; ++i) {
            const grfx::VertexBinding& binding = createInfo.vertexBindings[i];
            if (binding.GetAttributeCount() != 1) {
                PPX_ASSERT_MSG(false, "planar layout binding must have 1 attribute");
                return ppx::ERROR_INVALID_CREATE_ARGUMENT;
            }
        }
    }

    pGeometry->mCreateInfo = createInfo;

    Result ppxres = pGeometry->InternalCtor();
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

const Geometry::Buffer* Geometry::GetVertxBuffer(uint32_t index) const
{
    const Geometry::Buffer* pBuffer = nullptr;
    if (IsIndexInRange(index, mVertexBuffers)) {
        pBuffer = &mVertexBuffers[index];
    }
    return pBuffer;
}

const grfx::VertexBinding* Geometry::GetVertexBinding(uint32_t index) const
{
    const grfx::VertexBinding* pBinding = nullptr;
    if (IsIndexInRange(index, mVertexBuffers)) {
        pBinding = &mCreateInfo.vertexBindings[index];
    }
    return pBinding;
}

uint32_t Geometry::GetBiggestBufferSize() const
{
    uint32_t size = mIndexBuffer.GetSize();
    for (size_t i = 0; i < mVertexBuffers.size(); ++i) {
        size = std::max(size, mVertexBuffers[i].GetSize());
    }
    return size;
}

void Geometry::AppendIndicesTriangle(uint32_t vtx0, uint32_t vtx1, uint32_t vtx2)
{
    if (mCreateInfo.indexType == grfx::INDEX_TYPE_UINT16) {
        mIndexBuffer.Append(static_cast<uint16_t>(vtx0));
        mIndexBuffer.Append(static_cast<uint16_t>(vtx1));
        mIndexBuffer.Append(static_cast<uint16_t>(vtx2));
    }
    else if (mCreateInfo.indexType == grfx::INDEX_TYPE_UINT32) {
        mIndexBuffer.Append(vtx0);
        mIndexBuffer.Append(vtx1);
        mIndexBuffer.Append(vtx2);
    }
}

uint32_t Geometry::AppendVertexInterleaved(const VertexData& vtx)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED) {
        PPX_ASSERT_MSG(false, NOT_INTERLEAVED_MSG);
        return PPX_VALUE_IGNORED;
    }

    uint32_t       startSize = mVertexBuffers[0].GetSize();
    const uint32_t attrCount = mCreateInfo.vertexBindings[0].GetAttributeCount();
    for (uint32_t attrIndex = 0; attrIndex < attrCount; ++attrIndex) {
        const grfx::VertexAttribute* pAttribute = nullptr;
        bool                         res        = mCreateInfo.vertexBindings[0].GetAttribute(attrIndex, &pAttribute);
        PPX_ASSERT_MSG(res, "attribute not found at index=" << attrIndex);

        // clang-format off
        switch (pAttribute->semantic) {
            default: break;
            case grfx::VERTEX_SEMANTIC_POSITION  : mVertexBuffers[0].Append(vtx.position); break;
            case grfx::VERTEX_SEMANTIC_NORMAL    : mVertexBuffers[0].Append(vtx.normal); break;
            case grfx::VERTEX_SEMANTIC_COLOR     : mVertexBuffers[0].Append(vtx.color); break;
            case grfx::VERTEX_SEMANTIC_TANGENT   : mVertexBuffers[0].Append(vtx.tangent); break;
            case grfx::VERTEX_SEMANTIC_BITANGENT : mVertexBuffers[0].Append(vtx.bitangent); break;
            case grfx::VERTEX_SEMANTIC_TEXCOORD  : mVertexBuffers[0].Append(vtx.texCoord); break;
        }
        // clang-format on
    }
    uint32_t endSize = mVertexBuffers[0].GetSize();

    uint32_t bytesWritten = (endSize - startSize);
    PPX_ASSERT_MSG((bytesWritten == mVertexBuffers[0].GetElementSize()), "size of vertex data written does not match buffer's element size");

    uint32_t n = mVertexBuffers[0].GetElementCount() - 1;
    return n;
}

uint32_t Geometry::AppendVertexData(const VertexData& vtx)
{
    uint32_t n = 0;
    if (mCreateInfo.attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED) {
        n = AppendVertexInterleaved(vtx);
    }
    else if (mCreateInfo.attributeLayout == GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        n = AppendPosition(vtx.position);
        AppendNormal(vtx.normal);
        AppendColor(vtx.color);
        AppendTexCoord(vtx.texCoord);
        AppendTangent(vtx.tangent);
        AppendBitangent(vtx.bitangent);
    }
    else {
        // Something went horribly wrong
        PPX_ASSERT_MSG(false, "unknown attribute layout");
    }
    return n;
}

void Geometry::AppendTriangle(
    const ppx::Geometry::VertexData& vtx0,
    const ppx::Geometry::VertexData& vtx1,
    const ppx::Geometry::VertexData& vtx2)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_INTERLEAVED) {
        PPX_ASSERT_MSG(false, NOT_INTERLEAVED_MSG);
        return;
    }

    uint32_t n0 = AppendVertexData(vtx0);
    uint32_t n1 = AppendVertexData(vtx0);
    uint32_t n2 = AppendVertexData(vtx1);

    AppendIndicesTriangle(n0, n1, n2);
}

uint32_t Geometry::AppendPosition(const float3& value)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        PPX_ASSERT_MSG(false, NOT_PLANAR_MSG);
        return PPX_VALUE_IGNORED;
    }

    if (mPositionBufferIndex != PPX_VALUE_IGNORED) {
        mVertexBuffers[mPositionBufferIndex].Append(value);

        uint32_t n = mVertexBuffers[mPositionBufferIndex].GetElementCount() - 1;
        return n;
    }

    return PPX_VALUE_IGNORED;
}

void Geometry::AppendNormal(const float3& value)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        PPX_ASSERT_MSG(false, NOT_PLANAR_MSG);
        return;
    }

    if (mNormaBufferIndex != PPX_VALUE_IGNORED) {
        mVertexBuffers[mNormaBufferIndex].Append(value);
    }
}

void Geometry::AppendColor(const float3& value)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        PPX_ASSERT_MSG(false, NOT_PLANAR_MSG);
        return;
    }

    if (mColorBufferIndex != PPX_VALUE_IGNORED) {
        mVertexBuffers[mColorBufferIndex].Append(value);
    }
}

void Geometry::AppendTexCoord(const float2& value)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        PPX_ASSERT_MSG(false, NOT_PLANAR_MSG);
        return;
    }

    if (mTexCoordBufferIndex != PPX_VALUE_IGNORED) {
        mVertexBuffers[mTexCoordBufferIndex].Append(value);
    }
}

void Geometry::AppendTangent(const float3& value)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        PPX_ASSERT_MSG(false, NOT_PLANAR_MSG);
        return;
    }

    if (mTangentBufferIndex != PPX_VALUE_IGNORED) {
        mVertexBuffers[mTangentBufferIndex].Append(value);
    }
}

void Geometry::AppendBitangent(const float3& value)
{
    if (mCreateInfo.attributeLayout != GEOMETRY_ATTRIBUTE_LAYOUT_PLANAR) {
        PPX_ASSERT_MSG(false, NOT_PLANAR_MSG);
        return;
    }

    if (mBitangentBufferIndex != PPX_VALUE_IGNORED) {
        mVertexBuffers[mBitangentBufferIndex].Append(value);
    }
}

Result Geometry::CreateCube(const ppx::GeometryCreateInfo& createInfo, const float3& size, ppx::Geometry* pGeometry)
{
    PPX_ASSERT_NULL_ARG(pGeometry);

    Result ppxres = Geometry::Create(createInfo, pGeometry);
    if (Failed(ppxres)) {
        PPX_ASSERT_MSG(false, "failed creating geometry");
        return ppxres;
    }

    float hx = size.x / 2.0f;
    float hy = size.y / 2.0f;
    float hz = size.z / 2.0f;

    // clang-format off
    std::vector<float> vertexData = {  
        // position      // normal           // vertex colors    // texcoords  // tangents         // bitangents
         hx,  hy, -hz,   0.0f, 0.0f,-1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  0  -Z side
         hx, -hy, -hz,   0.0f, 0.0f,-1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  1
        -hx, -hy, -hz,   0.0f, 0.0f,-1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  2
        -hx,  hy, -hz,   0.0f, 0.0f,-1.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  3

        -hx,  hy,  hz,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  4  +Z side
        -hx, -hy,  hz,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  5
         hx, -hy,  hz,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  6
         hx,  hy,  hz,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  7

        -hx,  hy, -hz,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  8  -X side
        -hx, -hy, -hz,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  //  9
        -hx, -hy,  hz,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 10
        -hx,  hy,  hz,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 11

         hx,  hy,  hz,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 12  +X side
         hx, -hy,  hz,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 13
         hx, -hy, -hz,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 14
         hx,  hy, -hz,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 15

        -hx, -hy,  hz,   0.0f,-1.0f, 0.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 16  -Y side
        -hx, -hy, -hz,   0.0f,-1.0f, 0.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 17
         hx, -hy, -hz,   0.0f,-1.0f, 0.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 18
         hx, -hy,  hz,   0.0f,-1.0f, 0.0f,   1.0f, 0.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 19

        -hx,  hy, -hz,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 20  +Y side
        -hx,  hy,  hz,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 21
         hx,  hy,  hz,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 22
         hx,  hy, -hz,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f, 1.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 0.0f,  // 23
    };
    // clang-format on

    // clang-format off
    std::vector<uint16_t> indexData = {
        0,  1,  2, // -Z side
        0,  2,  3,

        4,  5,  6, // +Z side
        4,  6,  7,

        8,  9, 10, // -X side
        8, 10, 11,

        12, 13, 14, // +X side
        12, 14, 15,

        16, 17, 18, // -X side
        16, 18, 19,

        20, 21, 22, // +X side
        20, 22, 23,
    };
    // clang-format on

    // Size info and data pointer
    const size_t vertexDataSize    = (vertexData.size() * sizeof(float));
    const size_t vertexElementSize = sizeof(Geometry::VertexData);
    const char*  pVertexData       = reinterpret_cast<const char*>(vertexData.data());

    //
    // Geometry WITHOUT index data
    //
    if (createInfo.indexType == grfx::INDEX_TYPE_UNDEFINED) {
        // Itereate through the triange data in the index data and add
        // corresponding vertex data
        //
        for (size_t i = 0; i < indexData.size(); ++i) {
            uint32_t                    triVertexIndex = indexData[i];
            const Geometry::VertexData* pVertex        = reinterpret_cast<const Geometry::VertexData*>(pVertexData + triVertexIndex * vertexElementSize);
            pGeometry->AppendVertexData(*pVertex);
        }
    }
    //
    // Geometry WITH index data
    //
    else {
        // Vertex data
        size_t numVertices = vertexDataSize / vertexElementSize;
        for (size_t vertexIndex = 0; vertexIndex < numVertices; ++vertexIndex) {
            const Geometry::VertexData* pVertex = reinterpret_cast<const Geometry::VertexData*>(pVertexData + vertexIndex * vertexElementSize);
            pGeometry->AppendVertexData(*pVertex);
        }

        // Index data
        size_t numTris = indexData.size() / 3;
        for (size_t i = 0; i < numTris; ++i) {
            uint16_t vtx0 = indexData[3 * i + 0];
            uint16_t vtx1 = indexData[3 * i + 1];
            uint16_t vtx2 = indexData[3 * i + 2];
            pGeometry->AppendIndicesTriangle(vtx0, vtx1, vtx2);
        }
    }

    return ppx::SUCCESS;
}

Result Geometry::CreateFromOBJ(const ppx::GeometryCreateInfo& createInfo, const char* path, ppx::Geometry* pGeometry)
{
    PPX_ASSERT_NULL_ARG(pGeometry);

    const std::vector<float3> colors = {
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 0.0f},
        {1.0f, 0.0f, 1.0f},
        {0.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
    };

    tinyobj::attrib_t                attrib;
    std::vector<tinyobj::shape_t>    shapes;
    std::vector<tinyobj::material_t> materials;

    std::string warn;
    std::string err;
    bool        loaded = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path, nullptr, true);

    if (!loaded || !err.empty()) {
        return ppx::ERROR_GEOMETRY_FILE_LOAD_FAILED;
    }

    size_t numShapes = shapes.size();
    if (numShapes == 0) {
        return ppx::ERROR_GEOMETRY_FILE_NO_DATA;
    }

    // Create storage geometry
    Result ppxres = Geometry::Create(createInfo, pGeometry);
    if (Failed(ppxres)) {
        PPX_ASSERT_MSG(false, "failed creating geometry");
        return ppxres;
    }

    // Build geometry
    for (size_t shapeIdx = 0; shapeIdx < numShapes; ++shapeIdx) {
        const tinyobj::shape_t& shape = shapes[shapeIdx];
        const tinyobj::mesh_t&  mesh  = shape.mesh;

        size_t numTriangles = mesh.indices.size() / 3;
        for (size_t triIdx = 0; triIdx < numTriangles; ++triIdx) {
            size_t triVtxIdx0 = triIdx * 3 + 0;
            size_t triVtxIdx1 = triIdx * 3 + 1;
            size_t triVtxIdx2 = triIdx * 3 + 2;

            // Index data
            const tinyobj::index_t& dataIdx0 = mesh.indices[triVtxIdx0];
            const tinyobj::index_t& dataIdx1 = mesh.indices[triVtxIdx1];
            const tinyobj::index_t& dataIdx2 = mesh.indices[triVtxIdx2];

            // Vertex data
            Geometry::VertexData vtx0 = {};
            Geometry::VertexData vtx1 = {};
            Geometry::VertexData vtx2 = {};

            // Pick a face color
            float3 faceColor = colors[triIdx % colors.size()];
            vtx0.color       = faceColor;
            vtx1.color       = faceColor;
            vtx2.color       = faceColor;

            // Vertex positions
            {
                int i0        = 3 * dataIdx0.vertex_index + 0;
                int i1        = 3 * dataIdx0.vertex_index + 1;
                int i2        = 3 * dataIdx0.vertex_index + 2;
                vtx0.position = float3(attrib.vertices[i0], attrib.vertices[i1], attrib.vertices[i2]);

                i0            = 3 * dataIdx1.vertex_index + 0;
                i1            = 3 * dataIdx1.vertex_index + 1;
                i2            = 3 * dataIdx1.vertex_index + 2;
                vtx1.position = float3(attrib.vertices[i0], attrib.vertices[i1], attrib.vertices[i2]);

                i0            = 3 * dataIdx2.vertex_index + 0;
                i1            = 3 * dataIdx2.vertex_index + 1;
                i2            = 3 * dataIdx2.vertex_index + 2;
                vtx2.position = float3(attrib.vertices[i0], attrib.vertices[i1], attrib.vertices[i2]);
            }

            // Normals
            if ((dataIdx0.normal_index != -1) && (dataIdx1.normal_index != -1) && (dataIdx2.normal_index != -1)) {
                int i0      = 3 * dataIdx0.normal_index + 0;
                int i1      = 3 * dataIdx0.normal_index + 1;
                int i2      = 3 * dataIdx0.normal_index + 2;
                vtx0.normal = float3(attrib.normals[i0], attrib.normals[i1], attrib.normals[i2]);

                i0          = 3 * dataIdx1.normal_index + 0;
                i1          = 3 * dataIdx1.normal_index + 1;
                i2          = 3 * dataIdx1.normal_index + 2;
                vtx1.normal = float3(attrib.normals[i0], attrib.normals[i1], attrib.normals[i2]);

                i0          = 3 * dataIdx2.normal_index + 0;
                i1          = 3 * dataIdx2.normal_index + 1;
                i2          = 3 * dataIdx2.normal_index + 2;
                vtx2.normal = float3(attrib.normals[i0], attrib.normals[i1], attrib.normals[i2]);
            }

            // Texture coordinates
            if ((dataIdx0.texcoord_index != -1) && (dataIdx1.texcoord_index != -1) && (dataIdx2.texcoord_index != -1)) {
                int i0        = 2 * dataIdx0.texcoord_index + 0;
                int i1        = 2 * dataIdx0.texcoord_index + 1;
                vtx0.texCoord = float2(attrib.texcoords[i0], attrib.texcoords[i1]);

                i0            = 2 * dataIdx1.texcoord_index + 0;
                i1            = 2 * dataIdx1.texcoord_index + 1;
                vtx1.texCoord = float2(attrib.texcoords[i0], attrib.texcoords[i1]);

                i0            = 2 * dataIdx2.texcoord_index + 0;
                i1            = 2 * dataIdx2.texcoord_index + 1;
                vtx2.texCoord = float2(attrib.texcoords[i0], attrib.texcoords[i1]);
            }

            //
            // Geometry WITHOUT index data
            //
            if (createInfo.indexType == grfx::INDEX_TYPE_UNDEFINED) {
                pGeometry->AppendVertexData(vtx0);
                pGeometry->AppendVertexData(vtx1);
                pGeometry->AppendVertexData(vtx2);
            }
            //
            // Geometry WITH index data
            //
            else {
                uint32_t vtxi0 = pGeometry->AppendVertexData(vtx0);
                uint32_t vtxi1 = pGeometry->AppendVertexData(vtx1);
                uint32_t vtxi2 = pGeometry->AppendVertexData(vtx2);
                pGeometry->AppendIndicesTriangle(vtxi0, vtxi1, vtxi2);
            }
        }
    }

    return ppx::SUCCESS;
}

} // namespace ppx
