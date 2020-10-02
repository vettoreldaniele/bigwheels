#include "ppx/base_application.h"

namespace ppx {

BaseApplication::BaseApplication()
{
}

BaseApplication::~BaseApplication()
{
}

ppx::Platform BaseApplication::GetPlatform() const
{
    ppx::Platform platform = ppx::GetPlatform();
    return platform;
}

void BaseApplication::AddAssetDir(const fs::path& dir)
{
    auto it = std::find(std::begin(mAssetDirs), std::end(mAssetDirs), dir);
    if (it == std::end(mAssetDirs)) {
        mAssetDirs.push_back(dir);
    }
}

bool BaseApplication::GetAssetPath(const fs::path& searchPath, fs::path* pFoundPath) const
{
    bool found = false;
    for (size_t i = 0; i < mAssetDirs.size(); ++i) {
        fs::path path = mAssetDirs[i] / searchPath;
        if (fs::exists(path)) {
            if (!IsNull(pFoundPath)) {
                *pFoundPath = path;
                found       = true;
                break;
            }
        }
    }
    return found;
}

bool BaseApplication::LoadAsset(const fs::path& path, std::vector<char>* pData) const
{
    if (IsNull(pData)) {
        return false;
    }

    fs::path resolvedPath;
    bool found = GetAssetPath(path, &resolvedPath);
    if (!found) {
        return false;
    }

    *pData = fs::load_file(path);

    return true;
}

} // namespace ppx
