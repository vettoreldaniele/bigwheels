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
        if (itr != fileNameToContents.end()) {
            *ppData = itr->second.data();
            *pBytes = itr->second.size();
            return S_OK;
        }

        fs::path filePath             = baseDirPath;
        filePath                      = filePath / pFileName;
        auto includeFile              = fs::load_file(filePath);
        fileNameToContents[pFileName] = includeFile;
        *ppData                       = includeFile.data();
        *pBytes                       = includeFile.size();
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
