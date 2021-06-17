load("//build_defs:gsutil.bzl", "gsutil_toolchain")
load("//:third_party/YetiSDK/yeti_repository.bzl", "yeti_repository")

gsutil_toolchain(
    name = "yeti_crosstool_debian",
    build_tpl_path = "//tools/toolchains/debian:BUILD.tpl",
    crosstool_tpl_path = "//tools/toolchains/debian:crosstool.bzl.tpl",
    strip_components = ["YetiSDK"],
    url = "gs://yeti_base_sdk/releases/2021.3debian/YetiSDKBaseInternal-linux.tar.gz",
)

gsutil_toolchain(
    name = "yeti_crosstool_host",
    build_tpl_path = "//tools/toolchains/host:BUILD.tpl",
    crosstool_tpl_path = "//tools/toolchains/host:crosstool.bzl.tpl",
    strip_components = ["YetiSDK"],
    url = "gs://yeti_base_sdk/releases/2021.3debian/YetiSDKBaseInternal-linux.tar.gz",
)

new_local_repository(
    name = "contrib",
    build_file = "third_party/contrib.BUILD.bazel",
    path = "third_party/contrib",
)

new_local_repository(
    name = "cpu_features",
    build_file = "third_party/cpu_features.BUILD.bazel",
    path = "third_party/cpu_features",
)

new_local_repository(
    name = "glfw",
    build_file = "third_party/glfw.BUILD.bazel",
    path = "third_party/glfw",
)

new_local_repository(
    name = "gli",
    build_file = "third_party/gli.BUILD.bazel",
    path = "third_party/gli",
)

new_local_repository(
    name = "glm",
    build_file = "third_party/glm.BUILD.bazel",
    path = "third_party/glm",
)

new_local_repository(
    name = "imgui",
    build_file = "third_party/imgui.BUILD.bazel",
    path = "third_party/imgui",
)

new_local_repository(
    name = "pcg32",
    build_file = "third_party/pcg32.BUILD.bazel",
    path = "third_party/pcg32",
)

new_local_repository(
    name = "stb",
    build_file = "third_party/stb.BUILD.bazel",
    path = "third_party/stb",
)

new_local_repository(
    name = "tinyobjloader",
    build_file = "third_party/tinyobjloader.BUILD.bazel",
    path = "third_party/tinyobjloader",
)

new_local_repository(
    name = "utfcpp",
    build_file = "third_party/utfcpp.BUILD.bazel",
    path = "third_party/utfcpp",
)

new_local_repository(
    name = "VulkanMemoryAllocator",
    build_file = "third_party/VulkanMemoryAllocator.BUILD.bazel",
    path = "third_party/VulkanMemoryAllocator",
)

new_local_repository(
    name = "xxHash",
    build_file = "third_party/xxHash.BUILD.bazel",
    path = "third_party/xxHash",
)

local_repository(
    name = "dxc",
    path = "third_party/dxc",
)

local_repository(
    name = "spirv_tools",
    path = "third_party/spirv-tools",
)

local_repository(
    name = "spirv_headers",
    path = "third_party/spirv-headers",
)

# TODO(chouinard): Try replacing this with a `gsutil_archive`
# instead of relying on local SDK.
yeti_repository(
    name = "YetiSDK",
)
