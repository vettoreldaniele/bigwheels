load("@bazel_tools//tools/build_defs/pkg:pkg.bzl", "pkg_tar")

# Compiler options to be added to every yeti_cc_* rule.
COPTS = select({
    "//conditions:default": [
        "-DOS_LINUX",
        "-Wvla",
    ],
    "//tools:windows": [
        "/GS",  # Buffer security check
        "/GL",  # Whole program optimization
        "/analyze-",  # Disable code analyze
        "/W4",  # Warning level 4
        "/Zc:wchar_t",  # wchar_t is native char type
        "/Gm-",  # Disable minimal rebuild
        "/sdl",  # Enable additional security checks
        "/fp:precise",  # Precise floating point behavior
        "/errorReport:prompt",  # Internal compiler error triggers dialog prompt
        "/WX",  # Treat all compiler warnings as errors
        "/Zc:forScope",  # for loop initializer goes out of scope after loop
        "/Gd",  # Calling convention is __cdecl
        "/Oy-",  # Disable frame pointer omission
        "/EHsc",  # Catches C++ exceptions only; assuming functions with `extern "C"` linkage never throw
        "/Zc:rvalueCast",  # Enforce type conversion rules
        "/Zc:strictStrings",  # Disable string literal type conversion
        # These warnings are disabled because the code would not compile with
        # them enabled. Please fix the code and then remove them! b/70508940
        "/wd4100",  # unreferenced formal parameter
        "/wd4125",  # decimal digit terminates octal escape sequence
        "/wd4127",  # conditional expression is constant
        "/wd4146",  # unary minus operator applied to unsigned type, result still unsigned (needed for protobuf headers)
        "/wd4189",  # local variable is initialized but not referenced
        "/wd4245",  # signed/unsigned mismatch
        "/wd4389",  # signed/unsigned mismatch
        "/wd4457",  # declaration of 'x' hides function parameter
        "/wd4459",  # declaration of 'x' hides global declaration
        "/wd4702",  # unreachable code
        "/wd4800",  # 'int': forcing value to bool 'true' or 'false'
        "/wd4996",  # use _strdup instead of strdup
        "/wd4503",  # decorated name length exceeded, name was truncated (gRPC can generate some huge names)
    ],
    "//tools:caraway": [
        "-DYETI_OS_CARAWAY",
    ],
    "//tools:chromecast": [
        "-DOS_LINUX",
        "-DYETI_OS_CHROMECAST",
    ],
}) + select({
    "//conditions:default": [],
    "//tools:gamelet": [
        "-DYETI_PRODUCT_GAMELET",
    ],
    "//tools:gamelet_ubuntu": [
        "-DYETI_PRODUCT_GAMELET_UBUNTU",
    ],
    "//tools:gotham": [
        "-DYETI_PRODUCT_GOTHAM",
    ],
})

# Linker options to be added to every yeti_cc_* rule.
LINKOPTS = select({
    "//conditions:default": [
        "-pthread",
    ],
    "//tools:windows": [
    ],
    "//tools:caraway": [],
})

# Runfiles required for every binary execution (including tests).
RUNTIME_LIBS = select({
    "//conditions:default": ["@yeti_crosstool_debian//:dynamic-runtime-libs-k8"],
    "//tools:gamelet_ubuntu": ["@yeti_crosstool_ubuntu//:dynamic-runtime-libs-k8"],
    "//tools:windows": [],
    "//tools:chromecast": [],
})

SANITIZER_DEPS = select({
    "//conditions:default": [],
    "//tools:windows": [],
    "//tools:asan": ["//tools:suppress_asan"],
    "//tools:tsan": ["//tools:suppress_tsan"],
})

TEST_DATA = SANITIZER_DEPS + RUNTIME_LIBS

FEATURES = ["yeti_windows_cpp_flags"]

# The following rules were created in order to pass all the correct flags when
# building targets under //dev/... //clients/... //common/... //cloud/...
# Any such targets should use these rules unless there is a reason to the
# contrary.

def yeti_cc_library(
        copts = None,
        linkopts = None,
        features = None,
        **kwargs):
    """Yeti-specific specialization of a cc_library rule."""
    native.cc_library(
        copts = copts + COPTS if copts else COPTS,
        linkopts = linkopts + LINKOPTS if linkopts else LINKOPTS,
        features = features + FEATURES if features else FEATURES,
        **kwargs
    )

def yeti_cc_binary(
        copts = None,
        data = None,
        linkopts = None,
        features = None,
        malloc = None,
        **kwargs):
    """Yeti-specific specialization of a cc_binary rule."""
    native.cc_binary(
        copts = copts + COPTS if copts else COPTS,
        linkopts = linkopts + LINKOPTS if linkopts else LINKOPTS,
        data = data + RUNTIME_LIBS if data else RUNTIME_LIBS,
        features = features + FEATURES if features else FEATURES,
        # Allow malloc override for the default configuration only.
        # This is to avoid conflicts of TCMalloc with ASAN/etc.
        malloc = select({
            "//tools:asan": None,
            "//tools:tsan": None,
            "//tools:chromecast": None,
            "//tools:caraway": None,
            "//tools:gotham": None,
            "//conditions:default": malloc,
        }),
        **kwargs
    )

def yeti_cc_test(
        copts = None,
        data = None,
        linkopts = None,
        features = None,
        malloc = None,
        **kwargs):
    """Yeti-specific specialization of a cc_test rule."""
    native.cc_test(
        copts = copts + COPTS if copts else COPTS,
        linkopts = linkopts + LINKOPTS if linkopts else LINKOPTS,
        data = data + TEST_DATA if data else TEST_DATA,
        features = features + FEATURES if features else FEATURES,
        malloc = select({
            "//tools:asan": None,
            "//tools:tsan": None,
            "//tools:chromecast": None,
            "//tools:caraway": None,
            "//tools:gotham": None,
            "//conditions:default": malloc,
        }),
        **kwargs
    )
