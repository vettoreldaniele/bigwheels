#ifndef ppx_grfx_query_h
#define ppx_grfx_query_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

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

protected:
    virtual Result Create(const grfx::QueryPoolCreateInfo* pCreateInfo) override;
    friend class grfx::Device;
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_query_h
