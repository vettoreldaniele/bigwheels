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
    float3 tangent;
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
        //! Sets indices creation flag
        Options& EnableIndices(bool value = true) { mIndices = value; return *this; }
        //! Sets vertex color creation flag
        Options& EnableColors(bool value = true) { mColors = value; return *this; }
        //! Sets normals creation flag
        Options& EnableNormals(bool value = true) { mNormals = value; return *this; }
        //! Sets texture coordinates creation flag, most geometry will have 2-dimensional texture coordinates
        Options& EnableTexCoords(bool value = true) { mTexCoords = value; return *this; }
        //! Sets tangent and bitangent creation flag
        Options& EnableTangents(bool value = true) { mTangents = value; return *this; }
        // clang-format on
    private:
        bool mIndices   = false;
        bool mColors    = false;
        bool mNormals   = false;
        bool mTexCoords = false;
        bool mTangents  = false;
        friend class TriMesh;
    };

    TriMesh();
    TriMesh(grfx::IndexType indexType);
    TriMesh(TriMeshAttributeDim texCoordDim);
    TriMesh(grfx::IndexType indexType, TriMeshAttributeDim texCoordDim);
    ~TriMesh();

    grfx::IndexType     GetIndexType() const { return mIndexType; }
    TriMeshAttributeDim GetTexCoordDim() const { return mTexCoordDim; }

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
    const float3*   GetDataTangents(uint32_t index = 0) const;
    const float3*   GetDataBitangents(uint32_t index = 0) const;

    uint32_t AppendTriangle(uint32_t v0, uint32_t v1, uint32_t v2);
    uint32_t AppendPosition(const float3& value);
    uint32_t AppendColor(const float3& value);
    uint32_t AppendTexCoord(const float2& value);
    uint32_t AppendTexCoord(const float3& value);
    uint32_t AppendTexCoord(const float4& value);
    uint32_t AppendNormal(const float3& value);
    uint32_t AppendTangent(const float3& value);
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
    std::vector<float3>  mTangents;   // Vertex tangents
    std::vector<float3>  mBitangents; // Vertex bitangents
};

} // namespace ppx

#endif // ppx_mesh_h
