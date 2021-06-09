package(default_visibility = ["//visibility:public"])

load("//:crosstool.bzl", "cc_toolchain_config")

cc_toolchain_config(
    name = "toolchain_config",
    cpu = "k8",
)

cc_toolchain_suite(
    name = "cc_toolchain_suite",
    toolchains = {
        "k8": ":cc_toolchain",
        "k8|clang": ":cc_toolchain",
    },
)

cc_toolchain(
    name = "cc_toolchain",
    all_files = ":all_files-k8",
    compiler_files = ":all_files-k8",
    dwp_files = ":empty",
    dynamic_runtime_lib = ":dynamic-runtime-libs-k8",
    linker_files = ":all_files-k8",
    objcopy_files = ":all_files-k8",
    static_runtime_lib = ":static-runtime-libs-k8",
    strip_files = ":all_files-k8",
    supports_param_files = 1,
    toolchain_identifier = "clang_k8",
    toolchain_config = ":toolchain_config",
)

filegroup(
    name = "all_files-k8",
    srcs = [
        ":sysroot_files-k8",
        ":toolchain_files-k8",
    ],
)

filegroup(
    name = "toolchain_files-k8",
    srcs = glob(["toolchain/**/*"]),
)

filegroup(
    name = "sysroot_files-k8",
    srcs = glob(["sysroot/**/*"]),
)

filegroup(
    name = "static-runtime-libs-k8",
    srcs = glob(["{%install_path}sysroot/lib/*.a"]),
)

filegroup(
    name = "dynamic-runtime-libs-k8",
    srcs = glob(["{%install_path}sysroot/lib/*.so*"]),
)

filegroup(
    name = "empty",
    srcs = [],
)

# Any rule that depends upon clang-format must also depend on this rule, which
# pulls in the necessary shared objects for clang-format to run.
filegroup(
    name = "clang_format_deps",
    srcs = [
        "{%install_path}toolchain/lib/libc++.so.1",
        "{%install_path}toolchain/lib/libc++abi.so.1",
    ],
)

