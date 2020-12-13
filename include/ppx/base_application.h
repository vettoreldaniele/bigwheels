#ifndef ppx_base_application_h
#define ppx_base_application_h

#include "ppx/000_config.h"
#include "ppx/fs.h"
#include "ppx/platform.h"
#include "ppx/grfx/grfx_device.h"
#include "ppx/grfx/grfx_instance.h"

namespace ppx {

class BaseApplication
{
public:
    BaseApplication();
    virtual ~BaseApplication();

    ppx::PlatformId GetPlatformId() const;
    uint32_t        GetProcessId() const;
    fs::path        GetApplicationPath() const;

    const std::vector<fs::path>& GetAssetDirs() const { return mAssetDirs; }
    void                         AddAssetDir(const fs::path& path, bool insertAtFront = false);

    // Returns the first valid subPath in the asset directories list
    //
    // Example(s):
    //
    //    mAssetDirs = {"/a/valid/system/path",
    //                  "/another/valid/system/path",
    //                  "/some/valid/system/path"};
    //
    //    GetAssetPath("file.ext") - returns the full path to file.ext if it exists
    //      in any of the paths in mAssetDirs on the file system.
    //      Search starts with mAssetsDir[0].
    //
    //    GetAssetPath("subdir") - returns the full path to subdir if it exists
    //      in any of the paths in mAssetDirs on the file system.
    //      Search starts with mAssetsDir[0].
    //
    fs::path GetAssetPath(const fs::path& subPath) const;

private:
    std::vector<fs::path> mAssetDirs;
};

} // namespace ppx

#endif // ppx_base_application_h
