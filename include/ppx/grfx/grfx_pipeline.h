#ifndef ppx_grfx_pipeline_h
#define ppx_grfx_pipeline_h

#include "ppx/grfx/000_grfx_config.h"

namespace ppx {
namespace grfx {

//! @struct ComputePipelineCreateInfo
//!
//!
struct ComputePipelineCreateInfo
{
};

//! @class ComputePipeline
//!
//!
class ComputePipeline
    : public grfx::DeviceObject<grfx::ComputePipelineCreateInfo>
{
public:
    ComputePipeline() {}
    virtual ~ComputePipeline() {}
};

// -------------------------------------------------------------------------------------------------

//! @struct GraphicsPipelineCreateInfo
//!
//!
struct GraphicsPipelineCreateInfo
{
};

//! @class GraphicsPipeline
//!
//!
class GraphicsPipeline
    : public grfx::DeviceObject<grfx::GraphicsPipelineCreateInfo>
{
public:
    GraphicsPipeline() {}
    virtual ~GraphicsPipeline() {}
};

// -------------------------------------------------------------------------------------------------

//! @struct PipelineLayoutCreateInfo
//!
//!
struct PipelineLayoutCreateInfo
{
};

//! @class PipelineLayout
//!
//!
class PipelineLayout
    : public grfx::DeviceObject<grfx::PipelineLayoutCreateInfo>
{
public:
    PipelineLayout() {}
    virtual ~PipelineLayout() {}
};

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_pipeline_h
