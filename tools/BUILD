package(default_visibility = ["//visibility:public"])

config_setting(
    name = "gamelet",
    define_values = {"yeti_product": "gamelet"},
    values = {
        "crosstool_top": "@yeti_crosstool_debian//:cc_toolchain_suite",
    },
)

config_setting(
    name = "gamelet_ubuntu",
    define_values = {"yeti_product": "gamelet"},
    values = {
        "crosstool_top": "@yeti_crosstool_ubuntu//:cc_toolchain_suite",
    },
)

config_setting(
    name = "windows",
    values = {
        "cpu": "x64_windows",
    },
)

config_setting(
    name = "msvc",
    values = {
        "compiler": "msvc-cl",
    },
)

config_setting(
    name = "windows-clang",
    values = {
        "cpu": "x64_windows",
        "compiler": "clang",
    },
)

# Actually the same as "msvc" since MSVC always implies Windows, but added here
# for consistency with "windows-clang".
config_setting(
    name = "windows-msvc",
    values = {
        "cpu": "x64_windows",
        "compiler": "msvc-cl",
    },
)

config_setting(
    name = "chromecast",
    define_values = {"yeti_os": "chromecast"},
)

config_setting(
    name = "caraway",
    define_values = {"yeti_os": "caraway"},
)

config_setting(
    name = "gotham",
    define_values = {"yeti_product": "gotham"},
)

config_setting(
    name = "gotham-mcu",
    define_values = {
        "yeti_product": "gotham",
        "yeti_os": "caraway",
    },
)

config_setting(
    name = "gotham-soc",
    define_values = {
        "yeti_product": "gotham",
        "yeti_os": "chromecast",
    },
)

config_setting(
    name = "headless",
    values = {"define": "HEADLESS=1"},
)

config_setting(
    name = "enable_x264",
    values = {"define": "enable_x264=1"},
)

config_setting(
    name = "asan",
    values = {"copt": "-fsanitize=address"},
)

config_setting(
    name = "tsan",
    values = {"copt": "-fsanitize=thread"},
)

config_setting(
    name = "msan",
    values = {"copt": "-fsanitize=memory"},
)

config_setting(
    name = "ubsan",
    values = {"copt": "-fsanitize=undefined"},
)

filegroup(
    name = "suppress_asan",
    srcs = [
        "suppress.lsan",
    ],
    visibility = ["//visibility:public"],
)

filegroup(
    name = "suppress_tsan",
    srcs = [
        "suppress.tsan",
    ],
    visibility = ["//visibility:public"],
)

sh_binary(
    name = "gcs_upload",
    srcs = ["gcs_upload.sh"],
)
