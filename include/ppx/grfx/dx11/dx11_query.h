#ifndef ppx_grfx_dx11_query_h
#define ppx_grfx_dx11_query_h

#include "ppx/grfx/dx11/000_dx11_config.h"
#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {
namespace dx11 {

using D3D11QueryHeap = std::vector<ID3D11Query*>;

class QueryPool
    : public grfx::QueryPool
{
public:
    QueryPool() {}
    virtual ~QueryPool() {}

    void Begin(uint32_t queryIndex) const;
    void End(uint32_t queryIndex) const;
    void WriteTimestamp(grfx::PipelineStage pipelineStage, uint32_t queryIndex) const;

    ID3D11Query* GetQuery(uint32_t queryIndex) const;
    D3D11_QUERY GetQueryType() const { return mQueryType; }

    virtual void Reset(uint32_t firstQuery, uint32_t queryCount) override;

protected:
    virtual Result CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D11_QUERY    mQueryType = InvalidValue<D3D11_QUERY>();
    D3D11QueryHeap mQueryPool;
};

} // namespace dx11
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx11_query_h
