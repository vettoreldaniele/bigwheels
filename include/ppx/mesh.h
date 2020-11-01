#ifndef ppx_mesh_h
#define ppx_mesh_h

#include "ppx/000_config.h"
#include "ppx/000_math_config.h"
#include "ppx/grfx/000_grfx_config.h"
#include "ppx/fs.h"

namespace ppx {

//! @enum TriMeshAttributeDim
//!
//!
enum TriMeshAttributeDim
{
    TRI_MESH_ATTRIBUTE_DIM_UNDEFINED = 0,
    TRI_MESH_ATTRIBUTE_DIM_2         = 2,
    TRI_MESH_ATTRIBUTE_DIM_3         = 3,
    TRI_MESH_ATTRIBUTE_DIM_4         = 4,
};

//! @struct VertexData
//!
//!
struct VertexData
{
    float3 position;
    float3 color;
    float3 normal;
    float2 texCoord;
    float4 tangent;
    float3 bitangent;
};

//! @class TriMesh
//!
//!
class TriMesh
{
public:
    class Options
    {
    public:
        Options() {}
        ~Options() {}
        // clang-format off
        //! Enable/disable indices
        Options& Indices(bool value = true) { mEnableIndices = value; return *this; }
        //! Enable/disable vertex colors
        Options& VertexColors(bool value = true) { mEnableVertexColors = value; return *this; }
        //! Enable/disable normals
        Options& Normals(bool value = true) { mEnableNormals = value; return *this; }
        //! Enable/disable texture coordinates, most geometry will have 2-dimensional texture coordinates
        Options& TexCoords(bool value = true) { mEnableTexCoords = value; return *this; }
        //! Enable/disable tangent and bitangent creation flag
        Options& Tangents(bool value = true) { mEnableTangents = value; return *this; }
        //! Set and/or enable/disable object color, object color will override vertex colors
        Options& ObjectColor(const float3& color, bool enable = true) { mObjectColor = color; mEnableObjectColor = enable; return *this;}
        // clang-format on
    private:
        bool   mEnableIndices      = false;
        bool   mEnableVertexColors = false;
        bool   mEnableNormals      = false;
        bool   mEnableTexCoords    = false;
        bool   mEnableTangents     = false;
        bool   mEnableObjectColor  = false;
        float3 mObjectColor        = float3(0.7f);
        friend class TriMesh;
    };

    TriMesh();
    TriMesh(grfx::IndexType indexType);
    TriMesh(TriMeshAttributeDim texCoordDim);
    TriMesh(grfx::IndexType indexType, TriMeshAttributeDim texCoordDim);
    ~TriMesh();

    grfx::IndexType     GetIndexType() const { return mIndexType; }
    TriMeshAttributeDim GetTexCoordDim() const { return mTexCoordDim; }

    bool HasColors() const { return GetCountColors() > 0; }
    bool HasNormals() const { return GetCountNormals() > 0; }
    bool HasTexCoords() const { return GetCountTexCoords() > 0; }
    bool HasTangents() const { return GetCountTangents() > 0; }
    bool HasBitangents() const { return GetCountBitangents() > 0; }

    uint32_t GetCountTriangles() const;
    uint32_t GetCountIndices() const;
    uint32_t GetCountPositions() const;
    uint32_t GetCountColors() const;
    uint32_t GetCountNormals() const;
    uint32_t GetCountTexCoords() const;
    uint32_t GetCountTangents() const;
    uint32_t GetCountBitangents() const;

    uint64_t GetDataSizeIndices() const;
    uint64_t GetDataSizePositions() const;
    uint64_t GetDataSizeColors() const;
    uint64_t GetDataSizeNormalls() const;
    uint64_t GetDataSizeTexCoords() const;
    uint64_t GetDataSizeTangents() const;
    uint64_t GetDataSizeBitangents() const;

    const uint16_t* GetDataIndicesU16(uint32_t index = 0) const;
    const uint32_t* GetDataIndicesU32(uint32_t index = 0) const;
    const float3*   GetDataPositions(uint32_t index = 0) const;
    const float3*   GetDataColors(uint32_t index = 0) const;
    const float3*   GetDataNormalls(uint32_t index = 0) const;
    const float2*   GetDataTexCoords2(uint32_t index = 0) const;
    const float3*   GetDataTexCoords3(uint32_t index = 0) const;
    const float4*   GetDataTexCoords4(uint32_t index = 0) const;
    const float4*   GetDataTangents(uint32_t index = 0) const;
    const float3*   GetDataBitangents(uint32_t index = 0) const;

    uint32_t AppendTriangle(uint32_t v0, uint32_t v1, uint32_t v2);
    uint32_t AppendPosition(const float3& value);
    uint32_t AppendColor(const float3& value);
    uint32_t AppendTexCoord(const float2& value);
    uint32_t AppendTexCoord(const float3& value);
    uint32_t AppendTexCoord(const float4& value);
    uint32_t AppendNormal(const float3& value);
    uint32_t AppendTangent(const float4& value);
    uint32_t AppendBitangent(const float3& value);

    Result GetTriangle(uint32_t triIndex, uint32_t& v0, uint32_t& v1, uint32_t& v2) const;
    Result GetVertexData(uint32_t vtxIndex, VertexData* pVertexData) const;

    static TriMesh CreatePlane(const float2& size, const TriMesh::Options& options = TriMesh::Options());
    static TriMesh CreateCube(const float3& size, const TriMesh::Options& options = TriMesh::Options());
    static TriMesh CreateFromOBJ(const fs::path& path, const TriMesh::Options& options = TriMesh::Options());

private:
    void AppendIndexU16(uint16_t value);
    void AppendIndexU32(uint32_t value);

    static void AppendIndexAndVertexData(
        std::vector<uint32_t>&    indexData,
        const std::vector<float>& vertexData,
        const uint32_t            expectedVertexCount,
        const TriMesh::Options&   options,
        TriMesh&                  mesh);

private:
    grfx::IndexType      mIndexType   = grfx::INDEX_TYPE_UNDEFINED;
    TriMeshAttributeDim  mTexCoordDim = TRI_MESH_ATTRIBUTE_DIM_UNDEFINED;
    std::vector<uint8_t> mIndices;    // Stores both 16 and 32 bit indices
    std::vector<float3>  mPositions;  // Vertex positions
    std::vector<float3>  mColors;     // Vertex colors
    std::vector<float3>  mNormals;    // Vertex normals
    std::vector<float>   mTexCoords;  // Vertex texcoords, dimension can be 2, 3, or 4
    std::vector<float4>  mTangents;   // Vertex tangents
    std::vector<float3>  mBitangents; // Vertex bitangents
};

} // namespace ppx

#endif // ppx_mesh_h