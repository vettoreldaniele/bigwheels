// Copyright 2022 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     https://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "ppx/grfx/grfx_query.h"

namespace ppx {
namespace grfx {

Result Query::Create(const grfx::QueryCreateInfo* pCreateInfo)
{
    if (pCreateInfo->type == grfx::QUERY_TYPE_UNDEFINED) {
        return ppx::ERROR_GRFX_INVALID_QUERY_TYPE;
    }

    if (pCreateInfo->count == 0) {
        return ppx::ERROR_GRFX_INVALID_QUERY_COUNT;
    }

    Result ppxres = grfx::DeviceObject<grfx::QueryCreateInfo>::Create(pCreateInfo);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

} // namespace grfx
} // namespace ppx
