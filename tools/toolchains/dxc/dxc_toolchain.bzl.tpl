DxcInfo = provider(
    doc = "Information used to invoke DXC.",
    fields = [
        "dxc_lib",
        "dxc_path",
    ],
)

def _dxc_toolchain_impl(ctx):
    toolchain_info = platform_common.ToolchainInfo(
        DxcInfo = DxcInfo(
            dxc_lib = "{%toolchain_path}",
            dxc_path = "{%toolchain_path}/dxc",
        ),
    )
    return [toolchain_info]

dxc_toolchain = rule(
    implementation = _dxc_toolchain_impl,
    attrs = {
	"dxc_lib": attr.string(),
	"dxc_path": attr.string(),
    },
)
