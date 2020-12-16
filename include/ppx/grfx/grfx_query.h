#ifndef ppx_grfx_query_h
#define ppx_grfx_query_h

#include "ppx/grfx/000_grfx_config.h"

#define PPX_GRFX_PIPELINE_STATISTIC_NUM_ENTRIES  11

namespace ppx {
namespace grfx {

union PipelineStatistics
{
    struct
    {
        uint64_t IAVertices   ; // Input Assembly Vertices
        uint64_t IAPrimitives ; // Input Assembly Primitives
        uint64_t VSInvocations; // Vertex Shader Invocations
        uint64_t GSInvocations; // Geometry Shader Invocations
        uint64_t GSPrimitives ; // Geometry Shader Primitives
        uint64_t CInvocations ; // Clipping Invocations
        uint64_t CPrimitives  ; // Clipping Primitives
        uint64_t PSInvocations; // Pixel Shader Invocations
        uint64_t HSInvocations; // Hull Shader Invocations
        uint64_t DSInvocations; // Domain Shader Invocations
        uint64_t CSInvocations; // Compute Shader Invocations
    };
    uint64_t Statistics[PPX_GRFX_PIPELINE_STATISTIC_NUM_ENTRIES] = {0};
};

//! @struct QueryPoolCreateInfo
//!
//!
struct QueryPoolCreateInfo
{
    grfx::QueryType type  = QUERY_TYPE_UNDEFINED;
    uint32_t        count = 0;
};

//! @class QueryPool
//!
//!
class QueryPool
    : public grfx::DeviceObject<grfx::QueryPoolCreateInfo>
{
public:
    QueryPool() {}
    virtual ~QueryPool() {}

    grfx::QueryType GetType() const { return mCreateInfo.type; }

    virtual void Reset(uint32_t firstQuery, uint32_t queryCount) = 0;

protected:
    virtual Result Create(const grfx::QueryPoolCreateInfo* pCreateInfo) override;
    friend class grfx::Device;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_query_h
