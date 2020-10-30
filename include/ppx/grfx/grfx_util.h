#ifndef ppx_grfx_util_h
#define ppx_grfx_util_h

#include "ppx/grfx/grfx_enums.h"
#include "ppx/grfx/grfx_format.h"
#include "gli/target.hpp"
#include "gli/format.hpp"

namespace ppx {
namespace grfx {

const char* ToString(grfx::Api value);
const char* ToString(grfx::DescriptorType value);
const char* ToString(grfx::VertexSemantic value);

uint32_t     IndexTypeSize(grfx::IndexType value);
grfx::Format VertexSemanticFormat(grfx::VertexSemantic value);

const char* ToString(const gli::target& target);
const char* ToString(const gli::format& format);

} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_util_h
