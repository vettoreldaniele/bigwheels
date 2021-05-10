""" Repository rule for Yeti SDK configuration.
`yeti_repo` depends on the following environment variables:
  * GGP_SDK_PATH
"""

def _yeti_repo_impl(repository_ctx):
    path = repository_ctx.os.environ.get("GGP_SDK_PATH")
    if path == None:
      fail("GGP_SDK_PATH must be set")
    repository_ctx.symlink(path, "YetiSDK")
    repository_ctx.symlink(Label("//:third_party/YetiSDK/YetiSDK.BUILD.bazel"), "BUILD")

yeti_repository = repository_rule(
    implementation = _yeti_repo_impl,
    local=True,
    environ = [
        "GGP_SDK_PATH"
    ],
)