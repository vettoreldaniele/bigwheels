#include "ppx/mesh.h"
#include "ppx/math_util.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

namespace ppx {

TriMesh::TriMesh()
{
}

TriMesh::TriMesh(grfx::IndexType indexType)
    : mIndexType(indexType)
{
}

TriMesh::TriMesh(TriMeshAttributeDim texCoordDim)
    : mTexCoordDim(texCoordDim)
{
}

TriMesh::TriMesh(grfx::IndexType indexType, TriMeshAttributeDim texCoordDim)
    : mIndexType(indexType), mTexCoordDim(texCoordDim)
{
}

TriMesh::~TriMesh()
{
}

uint32_t TriMesh::GetCountTriangles() const
{
    uint32_t count = 0;
    if (mIndexType != grfx::INDEX_TYPE_UNDEFINED) {
        uint32_t elementSize  = grfx::IndexTypeSize(mIndexType);
        uint32_t elementCount = CountU32(mIndices) / elementSize;
        count                 = elementCount / 3;
    }
    else {
        count = CountU32(mPositions) / 3;
    }
    return count;
}

uint32_t TriMesh::GetCountIndices() const
{
    uint32_t indexSize = grfx::IndexTypeSize(mIndexType);
    if (indexSize == 0) {
        return 0;
    }

    uint32_t count = CountU32(mIndices) / indexSize;
    return count;
}

uint32_t TriMesh::GetCountPositions() const
{
    uint32_t count = CountU32(mPositions);
    return count;
}

uint32_t TriMesh::GetCountColors() const
{
    uint32_t count = CountU32(mColors);
    return count;
}

uint32_t TriMesh::GetCountNormals() const
{
    uint32_t count = CountU32(mNormals);
    return count;
}

uint32_t TriMesh::GetCountTexCoords() const
{
    if (mTexCoordDim == TRI_MESH_ATTRIBUTE_DIM_2) {
        uint32_t count = CountU32(mTexCoords) / 2;
        return count;
    }
    else if (mTexCoordDim == TRI_MESH_ATTRIBUTE_DIM_3) {
        uint32_t count = CountU32(mTexCoords) / 3;
        return count;
    }
    else if (mTexCoordDim == TRI_MESH_ATTRIBUTE_DIM_4) {
        uint32_t count = CountU32(mTexCoords) / 4;
        return count;
    }
    return 0;
}

uint32_t TriMesh::GetCountTangents() const
{
    uint32_t count = CountU32(mTangents);
    return count;
}

uint32_t TriMesh::GetCountBitangents() const
{
    uint32_t count = CountU32(mBitangents);
    return count;
}

uint64_t TriMesh::GetDataSizeIndices() const
{
    uint64_t size = static_cast<uint64_t>(mIndices.size());
    return size;
}

uint64_t TriMesh::GetDataSizePositions() const
{
    uint64_t size = static_cast<uint64_t>(mPositions.size() * sizeof(float3));
    return size;
}

uint64_t TriMesh::GetDataSizeColors() const
{
    uint64_t size = static_cast<uint64_t>(mColors.size() * sizeof(float3));
    return size;
}

uint64_t TriMesh::GetDataSizeNormalls() const
{
    uint64_t size = static_cast<uint64_t>(mNormals.size() * sizeof(float3));
    return size;
}

uint64_t TriMesh::GetDataSizeTexCoords() const
{
    uint64_t size = static_cast<uint64_t>(mTexCoords.size() * sizeof(float));
    return size;
}

uint64_t TriMesh::GetDataSizeTangents() const
{
    uint64_t size = static_cast<uint64_t>(mTangents.size() * sizeof(float3));
    return size;
}

uint64_t TriMesh::GetDataSizeBitangents() const
{
    uint64_t size = static_cast<uint64_t>(mBitangents.size() * sizeof(float3));
    return size;
}

const uint16_t* TriMesh::GetDataIndicesU16(uint32_t index) const
{
    if (mIndexType != grfx::INDEX_TYPE_UINT16) {
        return nullptr;
    }
    uint32_t count = GetCountIndices();
    if (index >= count) {
        return nullptr;
    }
    size_t      offset = sizeof(uint16_t) * index;
    const char* ptr    = reinterpret_cast<const char*>(mIndices.data()) + offset;
    return reinterpret_cast<const uint16_t*>(ptr);
}

const uint32_t* TriMesh::GetDataIndicesU32(uint32_t index) const
{
    if (mIndexType != grfx::INDEX_TYPE_UINT32) {
        return nullptr;
    }
    uint32_t count = GetCountIndices();
    if (index >= count) {
        return nullptr;
    }
    size_t      offset = sizeof(uint32_t) * index;
    const char* ptr    = reinterpret_cast<const char*>(mIndices.data()) + offset;
    return reinterpret_cast<const uint32_t*>(ptr);
}

const float3* TriMesh::GetDataPositions(uint32_t index) const
{
    if (index >= mPositions.size()) {
        return nullptr;
    }
    size_t      offset = sizeof(float3) * index;
    const char* ptr    = reinterpret_cast<const char*>(mPositions.data()) + offset;
    return reinterpret_cast<const float3*>(ptr);
}

const float3* TriMesh::GetDataColors(uint32_t index) const
{
    if (index >= mColors.size()) {
        return nullptr;
    }
    size_t      offset = sizeof(float3) * index;
    const char* ptr    = reinterpret_cast<const char*>(mColors.data()) + offset;
    return reinterpret_cast<const float3*>(ptr);
}

const float3* TriMesh::GetDataNormalls(uint32_t index) const
{
    if (index >= mNormals.size()) {
        return nullptr;
    }
    size_t      offset = sizeof(float3) * index;
    const char* ptr    = reinterpret_cast<const char*>(mNormals.data()) + offset;
    return reinterpret_cast<const float3*>(ptr);
}

const float2* TriMesh::GetDataTexCoords2(uint32_t index) const
{
    if (mTexCoordDim != TRI_MESH_ATTRIBUTE_DIM_2) {
        return nullptr;
    }
    uint32_t count = GetCountTexCoords();
    if (index >= count) {
        return nullptr;
    }
    size_t      offset = sizeof(float2) * index;
    const char* ptr    = reinterpret_cast<const char*>(mTexCoords.data()) + offset;
    return reinterpret_cast<const float2*>(ptr);
}

const float3* TriMesh::GetDataTexCoords3(uint32_t index) const
{
    if (mTexCoordDim != TRI_MESH_ATTRIBUTE_DIM_3) {
        return nullptr;
    }
    uint32_t count = GetCountTexCoords();
    if (index >= count) {
        return nullptr;
    }
    size_t      offset = sizeof(float3) * index;
    const char* ptr    = reinterpret_cast<const char*>(mTexCoords.data()) + offset;
    return reinterpret_cast<const float3*>(ptr);
}

const float4* TriMesh::GetDataTexCoords4(uint32_t index) const
{
    if (mTexCoordDim != TRI_MESH_ATTRIBUTE_DIM_4) {
        return nullptr;
    }
    uint32_t count = GetCountTexCoords();
    if (index >= count) {
        return nullptr;
    }
    size_t      offset = sizeof(float4) * index;
    const char* ptr    = reinterpret_cast<const char*>(mTexCoords.data()) + offset;
    return reinterpret_cast<const float4*>(ptr);
}

const float4* TriMesh::GetDataTangents(uint32_t index) const
{
    if (index >= mTangents.size()) {
        return nullptr;
    }
    size_t      offset = sizeof(float4) * index;
    const char* ptr    = reinterpret_cast<const char*>(mTangents.data()) + offset;
    return reinterpret_cast<const float4*>(ptr);
}

const float3* TriMesh::GetDataBitangents(uint32_t index) const
{
    if (index >= mBitangents.size()) {
        return nullptr;
    }
    size_t      offset = sizeof(float3) * index;
    const char* ptr    = reinterpret_cast<const char*>(mBitangents.data()) + offset;
    return reinterpret_cast<const float3*>(ptr);
}

void TriMesh::AppendIndexU16(uint16_t value)
{
    const uint8_t* pBytes = reinterpret_cast<const uint8_t*>(&value);
    mIndices.push_back(pBytes[0]);
    mIndices.push_back(pBytes[1]);
}

void TriMesh::AppendIndexU32(uint32_t value)
{
    const uint8_t* pBytes = reinterpret_cast<const uint8_t*>(&value);
    mIndices.push_back(pBytes[0]);
    mIndices.push_back(pBytes[1]);
    mIndices.push_back(pBytes[2]);
    mIndices.push_back(pBytes[3]);
}

uint32_t TriMesh::AppendTriangle(uint32_t v0, uint32_t v1, uint32_t v2)
{
    if (mIndexType == grfx::INDEX_TYPE_UINT16) {
        mIndices.reserve(mIndices.size() + 3 * sizeof(uint16_t));
        AppendIndexU16(static_cast<uint16_t>(v0));
        AppendIndexU16(static_cast<uint16_t>(v1));
        AppendIndexU16(static_cast<uint16_t>(v2));
    }
    else if (mIndexType == grfx::INDEX_TYPE_UINT32) {
        mIndices.reserve(mIndices.size() + 3 * sizeof(uint32_t));
        AppendIndexU32(v0);
        AppendIndexU32(v1);
        AppendIndexU32(v2);
    }
    else {
        PPX_ASSERT_MSG(false, "unknown index type");
        return 0;
    }
    uint32_t count = GetCountTriangles();
    return count;
}

uint32_t TriMesh::AppendPosition(const float3& value)
{
    mPositions.push_back(value);
    uint32_t count = GetCountPositions();
    return count;
}

uint32_t TriMesh::AppendColor(const float3& value)
{
    mColors.push_back(value);
    uint32_t count = GetCountColors();
    return count;
}

uint32_t TriMesh::AppendTexCoord(const float2& value)
{
    if (mTexCoordDim != TRI_MESH_ATTRIBUTE_DIM_2) {
        PPX_ASSERT_MSG(false, "unknown tex coord dim");
        return 0;
    }
    mTexCoords.reserve(mTexCoords.size() + 2);
    mTexCoords.push_back(value.x);
    mTexCoords.push_back(value.y);
    uint32_t count = GetCountTexCoords();
    return count;
}

uint32_t TriMesh::AppendTexCoord(const float3& value)
{
    if (mTexCoordDim != TRI_MESH_ATTRIBUTE_DIM_3) {
        PPX_ASSERT_MSG(false, "unknown tex coord dim");
        return 0;
    }
    mTexCoords.reserve(mTexCoords.size() + 3);
    mTexCoords.push_back(value.x);
    mTexCoords.push_back(value.y);
    mTexCoords.push_back(value.z);
    uint32_t count = GetCountTexCoords();
    return count;
}

uint32_t TriMesh::AppendTexCoord(const float4& value)
{
    if (mTexCoordDim != TRI_MESH_ATTRIBUTE_DIM_4) {
        PPX_ASSERT_MSG(false, "unknown tex coord dim");
        return 0;
    }
    mTexCoords.reserve(mTexCoords.size() + 3);
    mTexCoords.push_back(value.x);
    mTexCoords.push_back(value.y);
    mTexCoords.push_back(value.z);
    mTexCoords.push_back(value.w);
    uint32_t count = GetCountTexCoords();
    return count;
}

uint32_t TriMesh::AppendNormal(const float3& value)
{
    mNormals.push_back(value);
    uint32_t count = GetCountNormals();
    return count;
}

uint32_t TriMesh::AppendTangent(const float4& value)
{
    mTangents.push_back(value);
    uint32_t count = GetCountTangents();
    return count;
}

uint32_t TriMesh::AppendBitangent(const float3& value)
{
    mBitangents.push_back(value);
    uint32_t count = GetCountBitangents();
    return count;
}

Result TriMesh::GetTriangle(uint32_t triIndex, uint32_t& v0, uint32_t& v1, uint32_t& v2) const
{
    if (mIndexType == grfx::INDEX_TYPE_UNDEFINED) {
        return ppx::ERROR_NO_INDEX_DATA;
    }

    uint32_t triCount = GetCountTriangles();
    if (triIndex >= triCount) {
        return ppx::ERROR_OUT_OF_RANGE;
    }

    const uint8_t* pData       = mIndices.data();
    uint32_t       elementSize = grfx::IndexTypeSize(mIndexType);

    if (mIndexType == grfx::INDEX_TYPE_UINT16) {
        size_t          offset     = 3 * triIndex * elementSize;
        const uint16_t* pIndexData = reinterpret_cast<const uint16_t*>(pData + offset);
        v0                         = static_cast<uint32_t>(pIndexData[0]);
        v1                         = static_cast<uint32_t>(pIndexData[1]);
        v2                         = static_cast<uint32_t>(pIndexData[2]);
    }
    else if (mIndexType == grfx::INDEX_TYPE_UINT32) {
        size_t          offset     = 3 * triIndex * elementSize;
        const uint32_t* pIndexData = reinterpret_cast<const uint32_t*>(pData + offset);
        v0                         = static_cast<uint32_t>(pIndexData[0]);
        v1                         = static_cast<uint32_t>(pIndexData[1]);
        v2                         = static_cast<uint32_t>(pIndexData[2]);
    }

    return ppx::SUCCESS;
}

Result TriMesh::GetVertexData(uint32_t vtxIndex, VertexData* pVertexData) const
{
    uint32_t vertexCount = GetCountPositions();
    if (vtxIndex >= vertexCount) {
        return ppx::ERROR_OUT_OF_RANGE;
    }

    const float3* pPosition  = GetDataPositions(vtxIndex);
    const float3* pColor     = GetDataColors(vtxIndex);
    const float3* pNormal    = GetDataNormalls(vtxIndex);
    const float2* pTexCoord2 = GetDataTexCoords2(vtxIndex);
    const float3* pTexCoord3 = GetDataTexCoords3(vtxIndex);
    const float4* pTexCoord4 = GetDataTexCoords4(vtxIndex);
    const float4* pTangent   = GetDataTangents(vtxIndex);
    const float3* pBitangent = GetDataBitangents(vtxIndex);

    pVertexData->position = *pPosition;

    if (!IsNull(pColor)) {
        pVertexData->color = *pColor;
    }

    if (!IsNull(pNormal)) {
        pVertexData->normal = *pNormal;
    }

    if (!IsNull(pTexCoord2)) {
        pVertexData->texCoord = *pTexCoord2;
    }

    if (!IsNull(pTangent)) {
        pVertexData->tangent = *pTangent;
    }
    if (!IsNull(pBitangent)) {
        pVertexData->bitangent = *pBitangent;
    }

    return ppx::SUCCESS;
}

void TriMesh::AppendIndexAndVertexData(
    std::vector<uint32_t>&    indexData,
    const std::vector<float>& vertexData,
    const uint32_t            expectedVertexCount,
    const TriMesh::Options&   options,
    TriMesh&                  mesh)
{
    grfx::IndexType     indexType   = options.mEnableIndices ? grfx::INDEX_TYPE_UINT32 : grfx::INDEX_TYPE_UNDEFINED;
    TriMeshAttributeDim texCoordDim = options.mEnableTexCoords ? TRI_MESH_ATTRIBUTE_DIM_2 : TRI_MESH_ATTRIBUTE_DIM_UNDEFINED;

    // Verify expected vertex count
    size_t vertexCount = (vertexData.size() * sizeof(float)) / sizeof(VertexData);
    PPX_ASSERT_MSG(vertexCount == expectedVertexCount, "unexpected vertex count");

    // Get base pointer to vertex data
    const char* pData = reinterpret_cast<const char*>(vertexData.data());

    if (indexType != grfx::INDEX_TYPE_UNDEFINED) {
        for (size_t i = 0; i < vertexCount; ++i) {
            const VertexData* pVertexData = reinterpret_cast<const VertexData*>(pData + (i * sizeof(VertexData)));

            mesh.AppendPosition(pVertexData->position);

            if (options.mEnableVertexColors || options.mEnableObjectColor) {
                float3 color = options.mEnableObjectColor ? options.mObjectColor : pVertexData->color;
                mesh.AppendColor(color);
            }

            if (options.mEnableNormals) {
                mesh.AppendNormal(pVertexData->normal);
            }

            if (options.mEnableTexCoords) {
                mesh.AppendTexCoord(pVertexData->texCoord);
            }

            if (options.mEnableTangents) {
                mesh.AppendTangent(pVertexData->tangent);
                mesh.AppendBitangent(pVertexData->bitangent);
            }
        }

        size_t triCount = indexData.size() / 3;
        for (size_t triIndex = 0; triIndex < triCount; ++triIndex) {
            uint32_t v0 = indexData[3 * triIndex + 0];
            uint32_t v1 = indexData[3 * triIndex + 1];
            uint32_t v2 = indexData[3 * triIndex + 2];
            mesh.AppendTriangle(v0, v1, v2);
        }
    }
    else {
        for (size_t i = 0; i < indexData.size(); ++i) {
            uint32_t          vi          = indexData[i];
            const VertexData* pVertexData = reinterpret_cast<const VertexData*>(pData + (vi * sizeof(VertexData)));

            mesh.AppendPosition(pVertexData->position);

            if (options.mEnableVertexColors) {
                mesh.AppendColor(pVertexData->color);
            }

            if (options.mEnableNormals) {
                mesh.AppendNormal(pVertexData->normal);
            }

            if (options.mEnableTexCoords) {
                mesh.AppendTexCoord(pVertexData->texCoord);
            }

            if (options.mEnableTangents) {
                mesh.AppendTangent(pVertexData->tangent);
                mesh.AppendBitangent(pVertexData->bitangent);
            }
        }
    }
}

TriMesh TriMesh::CreatePlane(const float2& size, const TriMesh::Options& options)
{
    float hx = size.x / 2.0f;
    float hz = size.y / 2.0f;
    // clang-format off
    std::vector<float> vertexData = {
        // position       // vertex color     // normal           // texcoord   // tangent                // bitangent
        -hx, 0.0f, -hz,   0.7f, 0.7f, 0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
        -hx, 0.0f,  hz,   0.7f, 0.7f, 0.7f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
         hx, 0.0f,  hz,   0.7f, 0.7f, 0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
         hx, 0.0f, -hz,   0.7f, 0.7f, 0.7f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 0.0f,
    };

    std::vector<uint32_t> indexData = {
        0, 1, 2,
        0, 2, 3
    };
    // clang-format on

    grfx::IndexType     indexType   = options.mEnableIndices ? grfx::INDEX_TYPE_UINT32 : grfx::INDEX_TYPE_UNDEFINED;
    TriMeshAttributeDim texCoordDim = options.mEnableTexCoords ? TRI_MESH_ATTRIBUTE_DIM_2 : TRI_MESH_ATTRIBUTE_DIM_UNDEFINED;
    TriMesh             mesh        = TriMesh(indexType, texCoordDim);

    AppendIndexAndVertexData(indexData, vertexData, 4, options, mesh);

    return mesh;
}

TriMesh TriMesh::CreateCube(const float3& size, const TriMesh::Options& options)
{
    float hx = size.x / 2.0f;
    float hy = size.y / 2.0f;
    float hz = size.z / 2.0f;

    // clang-format off
    std::vector<float> vertexData = {  
        // position      // vertex colors    // normal           // texcoords   // tangents               // bitangents
         hx,  hy, -hz,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,-1.0f,   0.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  0  -Z side
         hx, -hy, -hz,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,-1.0f,   0.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  1
        -hx, -hy, -hz,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,-1.0f,   1.0f, 1.0f,  -1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  2
        -hx,  hy, -hz,    1.0f, 0.0f, 0.0f,   0.0f, 0.0f,-1.0f,   1.0f, 0.0f,  -1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  3

        -hx,  hy,  hz,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  4  +Z side
        -hx, -hy,  hz,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  5
         hx, -hy,  hz,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  6
         hx,  hy,  hz,    0.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  7

        -hx,  hy, -hz,   -0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  8  -X side
        -hx, -hy, -hz,   -0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  //  9
        -hx, -hy,  hz,   -0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  // 10
        -hx,  hy,  hz,   -0.0f, 0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  // 11

         hx,  hy,  hz,    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 0.0f,   0.0f, 0.0f,-1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  // 12  +X side
         hx, -hy,  hz,    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,   0.0f, 0.0f,-1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  // 13
         hx, -hy, -hz,    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   0.0f, 0.0f,-1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  // 14
         hx,  hy, -hz,    1.0f, 1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 0.0f,   0.0f, 0.0f,-1.0f, 1.0f,   0.0f,-1.0f, 0.0f,  // 15

        -hx, -hy,  hz,    1.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,-1.0f,  // 16  -Y side
        -hx, -hy, -hz,    1.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,-1.0f,  // 17
         hx, -hy, -hz,    1.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,-1.0f,  // 18
         hx, -hy,  hz,    1.0f, 0.0f, 1.0f,   0.0f,-1.0f, 0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f,-1.0f,  // 19

        -hx,  hy, -hz,    0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,  // 20  +Y side
        -hx,  hy,  hz,    0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,  // 21
         hx,  hy,  hz,    0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 1.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,  // 22
         hx,  hy, -hz,    0.0f, 1.0f, 1.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,   0.0f, 0.0f, 1.0f,  // 23
    };

    std::vector<uint32_t> indexData = {
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

    grfx::IndexType     indexType   = options.mEnableIndices ? grfx::INDEX_TYPE_UINT32 : grfx::INDEX_TYPE_UNDEFINED;
    TriMeshAttributeDim texCoordDim = options.mEnableTexCoords ? TRI_MESH_ATTRIBUTE_DIM_2 : TRI_MESH_ATTRIBUTE_DIM_UNDEFINED;
    TriMesh             mesh        = TriMesh(indexType, texCoordDim);

    AppendIndexAndVertexData(indexData, vertexData, 24, options, mesh);

    return mesh;
}

TriMesh TriMesh::CreateSphere(float radius, uint32_t usegs, uint32_t vsegs, const TriMesh::Options& options)
{
    constexpr float kPi    = glm::pi<float>();
    constexpr float kTwoPi = 2.0f * kPi;

    const uint32_t uverts = usegs + 1;
    const uint32_t vverts = vsegs + 1;

    float dt = kTwoPi / static_cast<float>(usegs);
    float dp = kPi / static_cast<float>(vsegs);

    std::vector<float> vertexData;
    for (uint32_t i = 0; i < uverts; ++i) {
        for (uint32_t j = 0; j < vverts; ++j) {
            float  theta     = i * dt;
            float  phi       = j * dp;
            float  u         = options.mTexCoordScale.x * theta / kTwoPi;
            float  v         = options.mTexCoordScale.x * phi / kPi;
            float3 P         = SphericalToCartesian(theta, phi);
            float3 position  = radius * P;
            float3 color     = float3(u, v, 0);
            float3 normal    = P;
            float2 texcoord  = float2(u, v);
            float4 tangent   = float4(-SphericalTangent(theta, phi), 1.0);
            float3 bitangent = glm::cross(normal, float3(tangent));

            vertexData.push_back(position.x);
            vertexData.push_back(position.y);
            vertexData.push_back(position.z);
            vertexData.push_back(color.r);
            vertexData.push_back(color.g);
            vertexData.push_back(color.b);
            vertexData.push_back(normal.x);
            vertexData.push_back(normal.y);
            vertexData.push_back(normal.z);
            vertexData.push_back(texcoord.x);
            vertexData.push_back(texcoord.y);
            vertexData.push_back(tangent.x);
            vertexData.push_back(tangent.y);
            vertexData.push_back(tangent.z);
            vertexData.push_back(tangent.w);
            vertexData.push_back(bitangent.x);
            vertexData.push_back(bitangent.y);
            vertexData.push_back(bitangent.z);
        }
    }

    std::vector<uint32_t> indexData;
    for (uint32_t i = 1; i < uverts; ++i) {
        for (uint32_t j = 1; j < vverts; ++j) {
            uint32_t i0 = i - 1;
            uint32_t i1 = i;
            uint32_t j0 = j - 1;
            uint32_t j1 = j;
            uint32_t v0 = i1 * vverts + j0;
            uint32_t v1 = i1 * vverts + j1;
            uint32_t v2 = i0 * vverts + j1;
            uint32_t v3 = i0 * vverts + j0;

            indexData.push_back(v0);
            indexData.push_back(v1);
            indexData.push_back(v2);

            indexData.push_back(v0);
            indexData.push_back(v2);
            indexData.push_back(v3);
        }
    }

    grfx::IndexType     indexType   = options.mEnableIndices ? grfx::INDEX_TYPE_UINT32 : grfx::INDEX_TYPE_UNDEFINED;
    TriMeshAttributeDim texCoordDim = options.mEnableTexCoords ? TRI_MESH_ATTRIBUTE_DIM_2 : TRI_MESH_ATTRIBUTE_DIM_UNDEFINED;
    TriMesh             mesh        = TriMesh(indexType, texCoordDim);

    uint32_t expectedVertexCount = uverts * vverts;
    AppendIndexAndVertexData(indexData, vertexData, expectedVertexCount, options, mesh);

    return mesh;
}

TriMesh TriMesh::CreateFromOBJ(const fs::path& path, const TriMesh::Options& options)
{
    grfx::IndexType     indexType   = options.mEnableIndices ? grfx::INDEX_TYPE_UINT32 : grfx::INDEX_TYPE_UNDEFINED;
    TriMeshAttributeDim texCoordDim = options.mEnableTexCoords ? TRI_MESH_ATTRIBUTE_DIM_2 : TRI_MESH_ATTRIBUTE_DIM_UNDEFINED;
    TriMesh             mesh        = TriMesh(indexType, texCoordDim);

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
        PPX_ASSERT_MSG(false, "OBj load failed for path=" << path.c_str());
        return mesh;
    }

    size_t numShapes = shapes.size();
    if (numShapes == 0) {
        PPX_ASSERT_MSG(false, "no shapes found");
        return mesh;
    }

    //// Check to see if data can be indexed
    //bool indexable = true;
    //for (size_t shapeIdx = 0; shapeIdx < numShapes; ++shapeIdx) {
    //    const tinyobj::shape_t& shape     = shapes[shapeIdx];
    //    const tinyobj::mesh_t&  shapeMesh = shape.mesh;
    //
    //    size_t numTriangles = shapeMesh.indices.size() / 3;
    //    for (size_t triIdx = 0; triIdx < numTriangles; ++triIdx) {
    //        size_t triVtxIdx0 = triIdx * 3 + 0;
    //        size_t triVtxIdx1 = triIdx * 3 + 1;
    //        size_t triVtxIdx2 = triIdx * 3 + 2;
    //
    //        // Index data
    //        const tinyobj::index_t& dataIdx0 = shapeMesh.indices[triVtxIdx0];
    //        const tinyobj::index_t& dataIdx1 = shapeMesh.indices[triVtxIdx1];
    //        const tinyobj::index_t& dataIdx2 = shapeMesh.indices[triVtxIdx2];
    //
    //        bool sameIdx0 = (dataIdx0.vertex_index == dataIdx0.normal_index) && (dataIdx0.normal_index == dataIdx0.texcoord_index);
    //        bool sameIdx1 = (dataIdx1.vertex_index == dataIdx1.normal_index) && (dataIdx1.normal_index == dataIdx1.texcoord_index);
    //        bool sameIdx2 = (dataIdx2.vertex_index == dataIdx2.normal_index) && (dataIdx2.normal_index == dataIdx2.texcoord_index);
    //        bool same     = sameIdx0 && sameIdx1 && sameIdx2;
    //        if (!same) {
    //           indexable = false;
    //           break;
    //        }
    //    }
    //}

    // Build geometry
    for (size_t shapeIdx = 0; shapeIdx < numShapes; ++shapeIdx) {
        const tinyobj::shape_t& shape     = shapes[shapeIdx];
        const tinyobj::mesh_t&  shapeMesh = shape.mesh;

        size_t numTriangles = shapeMesh.indices.size() / 3;
        for (size_t triIdx = 0; triIdx < numTriangles; ++triIdx) {
            size_t triVtxIdx0 = triIdx * 3 + 0;
            size_t triVtxIdx1 = triIdx * 3 + 1;
            size_t triVtxIdx2 = triIdx * 3 + 2;

            // Index data
            const tinyobj::index_t& dataIdx0 = shapeMesh.indices[triVtxIdx0];
            const tinyobj::index_t& dataIdx1 = shapeMesh.indices[triVtxIdx1];
            const tinyobj::index_t& dataIdx2 = shapeMesh.indices[triVtxIdx2];

            // Vertex data
            VertexData vtx0 = {};
            VertexData vtx1 = {};
            VertexData vtx2 = {};

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

            uint32_t triVtx0 = mesh.AppendPosition(vtx0.position) - 1;
            uint32_t triVtx1 = mesh.AppendPosition(vtx1.position) - 1;
            uint32_t triVtx2 = mesh.AppendPosition(vtx2.position) - 1;

            if (options.mEnableVertexColors || options.mEnableObjectColor) {
                if (options.mEnableObjectColor) {
                    vtx0.color = options.mObjectColor;
                    vtx1.color = options.mObjectColor;
                    vtx2.color = options.mObjectColor;
                }
                mesh.AppendColor(vtx0.color);
                mesh.AppendColor(vtx1.color);
                mesh.AppendColor(vtx2.color);
            }

            if (options.mEnableNormals) {
                mesh.AppendNormal(vtx0.normal);
                mesh.AppendNormal(vtx1.normal);
                mesh.AppendNormal(vtx2.normal);
            }

            if (options.mEnableTexCoords) {
                mesh.AppendTexCoord(vtx0.texCoord);
                mesh.AppendTexCoord(vtx1.texCoord);
                mesh.AppendTexCoord(vtx2.texCoord);
            }

            if (options.mEnableTangents) {
                float3 edge1 = vtx1.position - vtx0.position;
                float3 edge2 = vtx2.position - vtx0.position;
                float2 duv1  = vtx1.texCoord - vtx0.texCoord;
                float2 duv2  = vtx2.texCoord - vtx0.texCoord;
                float  r     = 1.0f / (duv1.x * duv2.y - duv1.y * duv2.x);

                float3 tangent = float3(
                    ((edge1.x * duv2.y) - (edge2.x * duv1.y)) * r,
                    ((edge1.y * duv2.y) - (edge2.y * duv1.y)) * r,
                    ((edge1.z * duv2.y) - (edge2.z * duv1.y)) * r);

                float3 bitangent = float3(
                    ((edge1.x * duv2.x) - (edge2.x * duv1.x)) * r,
                    ((edge1.y * duv2.x) - (edge2.y * duv1.x)) * r,
                    ((edge1.z * duv2.x) - (edge2.z * duv1.x)) * r);

                tangent = glm::normalize(tangent - vtx0.normal * glm::dot(vtx0.normal, tangent));
                float w = 1.0f;

                mesh.AppendTangent(float4(-tangent, w));
                mesh.AppendTangent(float4(-tangent, w));
                mesh.AppendTangent(float4(-tangent, w));
                mesh.AppendBitangent(-bitangent);
                mesh.AppendBitangent(-bitangent);
                mesh.AppendBitangent(-bitangent);
            }

            if (indexType != grfx::INDEX_TYPE_UNDEFINED) {
                mesh.AppendTriangle(triVtx0, triVtx1, triVtx2);
            }
        }
    }

    //if (options.mEnableTangents) {
    //    size_t numPositions  = mesh.mPositions.size();
    //    size_t numNormals    = mesh.mNormals.size();
    //    size_t numTangents   = mesh.mTangents.size();
    //    size_t numBitangents = mesh.mBitangents.size();
    //    PPX_ASSERT_MSG(numPositions == numNormals == numTangents == numBitangents, "misaligned data for tangent calculation");
    //
    //    for (size_t i = 0; i < numPositions; ++i) {
    //        const float3& T = mesh.mTangents[i];
    //        const float3& B = mesh.mBitangents[i];
    //    }
    //}

    return mesh;
}

} // namespace ppx
