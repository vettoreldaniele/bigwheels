"""Toolchain definition for DXC.
"""

load(":dxc_toolchain.bzl", "dxc_toolchain")

dxc_toolchain(
    name = "yeti_dxc",
)

toolchain_type(name = "dxc_toolchain_type")

toolchain(
    name = "yeti_dxc_toolchain",
    exec_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    target_compatible_with = [
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
    toolchain = ":yeti_dxc",
    toolchain_type = ":dxc_toolchain_type",
)
