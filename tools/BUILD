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
    name = "asan",
    values = {"copt": "-fsanitize=address"},
)

config_setting(
    name = "tsan",
    values = {"copt": "-fsanitize=thread"},
)
