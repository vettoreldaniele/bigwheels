"""Rules for compiling HLSL code using DXC.

To use these rules, load them in your BUILD file using

load("//build_defs:hlsl_rules.bzl", "spv_binary")
"""

load("@yeti_dxc//:dxc_toolchain.bzl", "dxc_toolchain")

VALID_SHADER_TYPES = ["cs", "vs", "ps"]
DEFAULT_SHADER_MODEL = "6_0"

def _spv_binary_impl(ctx):
    in_fname = ctx.attr.src.label.name
    output = ctx.actions.declare_file(ctx.attr.out.name)
    args = ctx.actions.args()
    args.add("-spirv")
    args.add("-T", "%s_%s" % (ctx.attr.shader_type, DEFAULT_SHADER_MODEL))
    args.add("-E", "%smain" % ctx.attr.shader_type)
    args.add("-Fo", output)
    args.add("-I", ctx.file.src.dirname)
    args.add_all(ctx.attr.dxc_flags)
    args.add_all(ctx.files.src)
    info = ctx.toolchains["@yeti_dxc//:dxc_toolchain_type"].DxcInfo
    ctx.actions.run(
        mnemonic = "DxcCompile",
        progress_message = "Compiling HLSL shader",
        executable = info.dxc_path,
        env = {"LD_LIBRARY_PATH": info.dxc_lib},
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
    },
    toolchains = ["@yeti_dxc//:dxc_toolchain_type"],
)

def get_shader_name(fname, shader_type, shader_target):
    """Computes the name of an output file for a given shader.

    Args:
      fname: The path name of the source shader file.
      shader_type: The type of output shader ('ps', 'vs' or 'cs').
      shader_target: Target format to be generated ('spv', 'dxil').

    Returns:
      A path name to be used as the output target for the shader binary
      compilation rule.
    """

    # NOTE: No access to os.path manipulation functions, so we must do this
    #       using string operations.
    path_elements = fname.split("/")
    out_dirname = "/".join(path_elements[:-1])
    out_basename = (path_elements[-1].replace(
        ".hlsl",
        ".%s.%s" % (shader_type, shader_target),
    ))
    return "%s/%s/%s" % (out_dirname, shader_target, out_basename)
