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

#ifndef ppx_base_application_h
#define ppx_base_application_h

#include "ppx/config.h"
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
