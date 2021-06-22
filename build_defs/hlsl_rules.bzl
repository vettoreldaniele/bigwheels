"""Rules for compiling HLSL code using DXC.

To use these rules, load them in your BUILD file using

load("//build_defs:hlsl_rules.bzl", "spv_binary")
"""

VALID_SHADER_TYPES = ["cs", "vs", "ps"]
DEFAULT_SHADER_MODEL = "6_0"

def _spv_binary_impl(ctx):
    in_fname = ctx.attr.src.label.name
    out_fname = in_fname.replace(
        ".hlsl",
        "." + ctx.attr.shader_type + ".spv",
    )
    output = ctx.actions.declare_file(out_fname)
    args = ctx.actions.args()
    args.add("-spirv")
    args.add("-T", "%s_%s" % (ctx.attr.shader_type, DEFAULT_SHADER_MODEL))
    args.add("-E", "%smain" % ctx.attr.shader_type)
    args.add("-Fo", output)
    args.add("-I", ctx.file.src.dirname)
    args.add_all(ctx.attr.dxc_flags)
    args.add_all(ctx.files.src)
    ctx.actions.run(
        mnemonic = "DxcCompile",
        executable = ctx.executable.compiler,
        arguments = [args],
        inputs = ctx.files.src + ctx.files.hdrs,
        outputs = [output],
    )
    return [OutputGroupInfo(all_files = depset([output]))]

spv_binary = rule(
    implementation = _spv_binary_impl,
    attrs = {
        "src": attr.label(
            allow_single_file = True,
            mandatory = True,
            doc = "Path to source HLSL file to compile with DXC.",
        ),
        "out": attr.output(),
        "hdrs": attr.label_list(
            doc = "List of headers that the HLSL file needs.",
            allow_files = [".h", ".hlsli"],
        ),
        "shader_type": attr.string(
            mandatory = True,
            values = VALID_SHADER_TYPES,
            doc = ("Shader type to compile. NOTE: each type will set a " +
                   "default entry point named '<TYPE>main'. This can be " +
                   "overriden by specifying -E in dxc_flags. Valid types: " +
                   "ps, vs, cs."),
        ),
        "dxc_flags": attr.string_list(
            doc = ("A list of DXC arguments to pass when compiling the given " +
                   "shader type."),
        ),
        "compiler": attr.label(
            default = Label("@dxc//:dxc"),
            allow_single_file = True,
            executable = True,
            cfg = "exec",
        ),
    },
)
