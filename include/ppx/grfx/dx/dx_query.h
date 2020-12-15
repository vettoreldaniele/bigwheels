#ifndef ppx_grfx_dx_query_h
#define ppx_grfx_dx_query_h

#include "ppx/grfx/dx/000_dx_config.h"
#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {
namespace dx {

class QueryPool
    : public grfx::QueryPool
{
public:
    QueryPool() {}
    virtual ~QueryPool() {}

    typename D3D12QueryHeapPtr::InterfaceType* GetDxQueryHeap() const { return mHeap.Get(); }

    D3D12_QUERY_TYPE GetQueryType() const { return mQueryType; }

    virtual void Reset(uint32_t firstQuery, uint32_t queryCount) override;

protected:
    virtual Result CreateApiObjects(const grfx::QueryPoolCreateInfo* pCreateInfo) override;
    virtual void   DestroyApiObjects() override;

private:
    D3D12QueryHeapPtr mHeap;
    D3D12_QUERY_TYPE  mQueryType = InvalidValue<D3D12_QUERY_TYPE>();
};

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_query_h
