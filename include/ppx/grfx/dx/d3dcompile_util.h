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

#ifndef ppx_grfx_dx_d3dcompile_util_h
#define ppx_grfx_dx_d3dcompile_util_h

#include <unordered_map>
#include <vector>

#include "d3dcompiler.h"
#include "ppx/ppx.h"

namespace ppx {
namespace grfx {
namespace dx {

class ShaderIncludeHandler : public ID3DInclude
{
public:
    ShaderIncludeHandler(const fs::path& baseDir)
        : baseDirPath(baseDir) {}

    HRESULT Open(
        D3D_INCLUDE_TYPE IncludeType,
        LPCSTR           pFileName,
        LPCVOID          pParentData,
        LPCVOID*         ppData,
        UINT*            pBytes) override
    {
        auto itr = fileNameToContents.find(pFileName);
        if (itr == fileNameToContents.end()) {
            fs::path filePath = baseDirPath;
            filePath          = filePath / pFileName;
            itr               = fileNameToContents.insert({pFileName, fs::load_file(filePath)}).first;
        }
        *ppData = itr->second.data();
        *pBytes = static_cast<UINT>(itr->second.size());
        return S_OK;
    }

    HRESULT Close(
        LPCVOID pData) override
    {
        return S_OK;
    }

private:
    fs::path                                      baseDirPath;
    std::unordered_map<LPCSTR, std::vector<char>> fileNameToContents;
};

std::vector<char> CompileShader(const fs::path& baseDir, const std::string& baseName, const char* shaderModel, ShaderIncludeHandler* includeHandler);

} // namespace dx
} // namespace grfx
} // namespace ppx

#endif // ppx_grfx_dx_d3dcompile_util_h
