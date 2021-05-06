"""Rules for downloading files and archives using gsutil.

### Setup

To use these rules, load them in your `WORKSPACE` file as follows:

```python
load(
    "//build_defs:gsutil.bzl",
    "gsutil_archive",
    "gsutil_file",
)
```
"""

def _strip_ext(s):
    return s.rsplit(".", 1)[0]

def _gsutil(rctx):
    if rctx.os.name.lower().startswith("windows"):
        return rctx.which("gsutil.cmd")
    return rctx.which("gsutil")

def _create_workspace_from_archive(rctx, strip = []):
    result = rctx.execute([_gsutil(rctx), "-q", "-m", "cp", rctx.attr.url, "."])
    if result.return_code:
        fail("gsutil failed with error code %s:\n%s" % (
            result.return_code,
            result.stderr,
        ))
    archive = rctx.path(rctx.attr.url.rsplit("/", 1)[1])
    rctx.extract(archive, stripPrefix = "/".join(strip))
    rctx.file("WORKSPACE", "workspace(name = \"{name}\")\n".format(name = rctx.name))

def _gsutil_archive_impl(rctx):
    """Implementation of the gsutil_archive rule."""
    _create_workspace_from_archive(rctx, strip = rctx.attr.strip_components)
    if rctx.attr.build_tpl_path:
        rctx.template(
            "BUILD",
            rctx.attr.build_tpl_path,
            {},
        )

"""Downloads a Bazel repository as a compressed archive file, decompresses it,
and makes its targets available for binding.

The repository should already contain a BUILD file.

It supports compressed tar archives.

Examples:
  Suppose the current repository contains the source code for a chat program,
  rooted at the directory `~/chat-app`. It needs to depend on an SSL library
  which is available from gs://example/openssl.tar.gz. This `.tar.gz` file
  contains the following directory structure:

  ```
  WORKSPACE
  src/
    openssl.cc
    openssl.h
  ```

  In the local repository, the user creates a `openssl.BUILD` file which
  contains the following target definition:

  ```python
  cc_library(
      name = "openssl-lib",
      srcs = ["src/openssl.cc"],
      hdrs = ["src/openssl.h"],
  )
  ```

  Targets in the `~/chat-app` repository can depend on this target if the
  following lines are added to `~/chat-app/WORKSPACE`:

  ```python
  gsutil_archive(
      name = "my_ssl",
      url = "gs://example/openssl.tar.gz",
  )
  ```

  Then targets would specify `@my_ssl//:openssl-lib` as a dependency.

Args:
  name: A unique name for this rule.
  url: A URL to a file that will be made available to Bazel.
       This must be a file or gs URL.
  crosstool_tpl_path: Optional. A label to a local CROSSTOOL.tpl. Default uses
                      the CROSSTOOL.tpl in the root of the GCS archive.
  build_tpl_path: Optional. A label to a local BUILD file to place in the root
                  of the downloaded toolchain. Default uses the BUILD file in
                  the root of the GCS archive.


"""
gsutil_archive = repository_rule(
    attrs = {
        "url": attr.string(mandatory = True),
        "build_tpl_path": attr.label(mandatory = False, allow_single_file = True),
        "strip_components": attr.string_list(allow_empty = True, default = []),
    },
    implementation = _gsutil_archive_impl,
)

def _gsutil_toolchain_impl(ctx):
    """Implementation of the gsutil_toolchain rule."""
    _create_workspace_from_archive(ctx, strip = ctx.attr.strip_components)
    crosstool_tpl = ctx.attr.crosstool_tpl_path or "crosstool.bzl.tpl"
    crosstool_dst = _strip_ext(ctx.attr.crosstool_tpl_path.name) if ctx.attr.crosstool_tpl_path else "crosstool.bzl"
    ctx.template(
        crosstool_dst,
        crosstool_tpl,
        {"{%toolchain_path}": "{}".format(ctx.path(""))},
    )
    build_tpl = ctx.attr.build_tpl_path or "BUILD.tpl"
    ctx.template(
        "BUILD",
        build_tpl,
        {},
    )

"""Downloads a Bazel toolchain repository as a compressed archive file, decompresses it,
makes its targets available for binding, and configures the CROSSTOOL file.

Similar to gsutil_archive rule, but also configures CROSSTOOL file to use
abosulte paths.

Args:
  name: A unique name for this rule.
  url: A URL to a file that will be made available to Bazel.

    This must be a file or gs URL.
"""
gsutil_toolchain = repository_rule(
    attrs = {
        "url": attr.string(mandatory = True),
        "crosstool_tpl_path": attr.label(mandatory = False, allow_single_file = True),
        "strip_components": attr.string_list(allow_empty = True, default = []),
        "build_tpl_path": attr.label(mandatory = False, allow_single_file = True),
    },
    implementation = _gsutil_toolchain_impl,
)
