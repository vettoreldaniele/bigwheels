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

#include "ppx/base_application.h"

namespace ppx {

BaseApplication::BaseApplication()
{
}

BaseApplication::~BaseApplication()
{
}

ppx::PlatformId BaseApplication::GetPlatformId() const
{
    ppx::PlatformId platform = Platform::GetPlatformId();
    return platform;
}

uint32_t BaseApplication::GetProcessId() const
{
    uint32_t pid = UINT32_MAX;
#if defined(PPX_LINUX) || defined(PPX_GGP)
    pid = static_cast<uint32_t>(getpid());
#elif defined(PPX_MSW)
    pid                       = static_cast<uint32_t>(::GetCurrentProcessId());
#endif
    return pid;
}

fs::path BaseApplication::GetApplicationPath() const
{
    fs::path path;
#if defined(PPX_LINUX) || defined(PPX_GGP)
    char buf[PATH_MAX];
    std::memset(buf, 0, PATH_MAX);
    readlink("/proc/self/exe", buf, PATH_MAX);
    path = fs::path(buf);
#elif defined(PPX_MSW)
    HMODULE this_win32_module = GetModuleHandleA(nullptr);
    char    buf[MAX_PATH];
    std::memset(buf, 0, MAX_PATH);
    GetModuleFileNameA(this_win32_module, buf, MAX_PATH);
    path = fs::path(buf);
#else
#error "not implemented"
#endif
    return path;
}

void BaseApplication::AddAssetDir(const fs::path& path, bool insertAtFront)
{
    auto it = Find(mAssetDirs, path);
    if (it != std::end(mAssetDirs)) {
        return;
    }

    if (!fs::is_directory(path)) {
        return;
    }

    mAssetDirs.push_back(path);

    if (insertAtFront) {
        // Rotate to front
        std::rotate(
            std::rbegin(mAssetDirs),
            std::rbegin(mAssetDirs) + 1,
            std::rend(mAssetDirs));
    }
}

fs::path BaseApplication::GetAssetPath(const fs::path& subPath) const
{
    fs::path assetPath;
    for (auto& assetDir : mAssetDirs) {
        fs::path path = assetDir / subPath;
        if (fs::exists(path)) {
            assetPath = path;
            break;
        }
    }
    return assetPath;
}

} // namespace ppx
