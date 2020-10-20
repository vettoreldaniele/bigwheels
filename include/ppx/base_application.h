#ifndef ppx_base_application_h
#define ppx_base_application_h

#include "ppx/000_config.h"
#include "ppx/fs.h"
#include "ppx/geometry.h"
#include "ppx/platform.h"
#include "ppx/grfx/grfx_instance.h"

namespace ppx {

class BaseApplication
{
public:
    BaseApplication();
    virtual ~BaseApplication();

    ppx::Platform GetPlatform() const;

    void AddAssetDir(const fs::path& dir);
    bool GetAssetPath(const fs::path& searchPath, fs::path* pFoundPath) const;
    bool LoadAsset(const fs::path& path, std::vector<char>* pData) const;

    grfx::InstancePtr GetGrfxInstance() const;
    uint32_t          GetGrfxGpuCount() const;
    bool              GetGrfxGpu(uint32_t index, grfx::Gpu** ppGpu) const;

private:
    std::vector<fs::path> mAssetDirs;
    grfx::InstancePtr     mGrfxInstance;
};

} // namespace ppx

#endif // ppx_base_application_h
