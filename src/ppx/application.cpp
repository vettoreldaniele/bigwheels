#include "ppx/application.h"
#include "examples/imgui_impl_glfw.h"

#include <map>
#include <unordered_map>

#if defined(PPX_LINUX_XCB)
#include <X11/Xlib-xcb.h>
#elif defined(PPX_LINUX_XLIB)
#error "Xlib not implemented"
#elif defined(PPX_LINUX_WAYLAND)
#error "Wayland not implemented"
#endif

namespace ppx {

const char*    kDefaultAppName      = "PPX Application";
const uint32_t kDefaultWindowWidth  = 640;
const uint32_t kDefaultWindowHeight = 480;

static Application* sApplicationInstance = nullptr;

// -------------------------------------------------------------------------------------------------
// Key code character map
// -------------------------------------------------------------------------------------------------
// clang-format off
static std::map<int32_t, char> sCharMap = {
    {KEY_SPACE,         ' '  },
    {KEY_APOSTROPHE,    '\'' },
    {KEY_COMMA,         ','  },
    {KEY_MINUS,         '-'  },
    {KEY_PERIOD,         '.' },
    {KEY_SLASH,         '\\' },
    {KEY_0,             '0'  },
    {KEY_1,             '1'  },
    {KEY_2,             '2'  },
    {KEY_3,             '3'  },
    {KEY_4,             '4'  },
    {KEY_5,             '5'  },
    {KEY_6,             '6'  },
    {KEY_7,             '7'  },
    {KEY_8,             '8'  },
    {KEY_9,             '9'  },
    {KEY_SEMICOLON,     ';'  },
    {KEY_EQUAL,         '='  },
    {KEY_A,             'A'  },
    {KEY_B,             'B'  },
    {KEY_C,             'C'  },
    {KEY_D,             'D'  },
    {KEY_E,             'E'  },
    {KEY_F,             'F'  },
    {KEY_G,             'G'  },
    {KEY_H,             'H'  },
    {KEY_I,             'I'  },
    {KEY_J,             'J'  },
    {KEY_K,             'K'  },
    {KEY_L,             'L'  },
    {KEY_M,             'M'  },
    {KEY_N,             'N'  },
    {KEY_O,             'O'  },
    {KEY_P,             'P'  },
    {KEY_Q,             'Q'  },
    {KEY_R,             'R'  },
    {KEY_S,             'S'  },
    {KEY_T,             'T'  },
    {KEY_U,             'U'  },
    {KEY_V,             'V'  },
    {KEY_W,             'W'  },
    {KEY_X,             'X'  },
    {KEY_Y,             'Y'  },
    {KEY_Z,             'Z'  },
    {KEY_LEFT_BRACKET,  '['  },
    {KEY_BACKSLASH,     '/'  },
    {KEY_RIGHT_BRACKET, ']'  },
    {KEY_GRAVE_ACCENT,  '`'  },
};
// clang-format on

// -------------------------------------------------------------------------------------------------
// Key string
// -------------------------------------------------------------------------------------------------
// clang-format off
static std::map<uint32_t, const char*> sKeyCodeString = {
    {KEY_UNDEFINED,        "KEY_UNDEFINED"         },
    {KEY_SPACE,            "KEY_SPACE"             },
    {KEY_APOSTROPHE,       "KEY_APOSTROPHE"        },
    {KEY_COMMA,            "KEY_COMMA"             },
    {KEY_MINUS,            "KEY_MINUS"             },
    {KEY_PERIOD,           "KEY_PERIOD"            },
    {KEY_SLASH,            "KEY_SLASH"             },
    {KEY_0,                "KEY_0"                 },
    {KEY_1,                "KEY_1"                 },
    {KEY_2,                "KEY_2"                 },
    {KEY_3,                "KEY_3"                 },
    {KEY_4,                "KEY_4"                 },
    {KEY_5,                "KEY_5"                 },
    {KEY_6,                "KEY_6"                 },
    {KEY_7,                "KEY_7"                 },
    {KEY_8,                "KEY_8"                 },
    {KEY_9,                "KEY_9"                 },
    {KEY_SEMICOLON,        "KEY_SEMICOLON"         },
    {KEY_EQUAL,            "KEY_EQUAL"             },
    {KEY_A,                "KEY_A"                 },
    {KEY_B,                "KEY_B"                 },
    {KEY_C,                "KEY_C"                 },
    {KEY_D,                "KEY_D"                 },
    {KEY_E,                "KEY_E"                 },
    {KEY_F,                "KEY_F"                 },
    {KEY_G,                "KEY_G"                 },
    {KEY_H,                "KEY_H"                 },
    {KEY_I,                "KEY_I"                 },
    {KEY_J,                "KEY_J"                 },
    {KEY_K,                "KEY_K"                 },
    {KEY_L,                "KEY_L"                 },
    {KEY_M,                "KEY_M"                 },
    {KEY_N,                "KEY_N"                 },
    {KEY_O,                "KEY_O"                 },
    {KEY_P,                "KEY_P"                 },
    {KEY_Q,                "KEY_Q"                 },
    {KEY_R,                "KEY_R"                 },
    {KEY_S,                "KEY_S"                 },
    {KEY_T,                "KEY_T"                 },
    {KEY_U,                "KEY_U"                 },
    {KEY_V,                "KEY_V"                 },
    {KEY_W,                "KEY_W"                 },
    {KEY_X,                "KEY_X"                 },
    {KEY_Y,                "KEY_Y"                 },
    {KEY_Z,                "KEY_Z"                 },
    {KEY_LEFT_BRACKET,     "KEY_LEFT_BRACKET"      },
    {KEY_BACKSLASH,        "KEY_BACKSLASH"         },
    {KEY_RIGHT_BRACKET,    "KEY_RIGHT_BRACKET"     },
    {KEY_GRAVE_ACCENT,     "KEY_GRAVE_ACCENT"      },
    {KEY_WORLD_1,          "KEY_WORLD_1"           },
    {KEY_WORLD_2,          "KEY_WORLD_2"           },
    {KEY_ESCAPE,           "KEY_ESCAPE"            },
    {KEY_ENTER,            "KEY_ENTER"             },
    {KEY_TAB,              "KEY_TAB"               },
    {KEY_BACKSPACE,        "KEY_BACKSPACE"         },
    {KEY_INSERT,           "KEY_INSERT"            },
    {KEY_DELETE,           "KEY_DELETE"            },
    {KEY_RIGHT,            "KEY_RIGHT"             },
    {KEY_LEFT,             "KEY_LEFT"              },
    {KEY_DOWN,             "KEY_DOWN"              },
    {KEY_UP,               "KEY_UP"                },
    {KEY_PAGE_UP,          "KEY_PAGE_UP"           },
    {KEY_PAGE_DOWN,        "KEY_PAGE_DOWN"         },
    {KEY_HOME,             "KEY_HOME"              },
    {KEY_END,              "KEY_END"               },
    {KEY_CAPS_LOCK,        "KEY_CAPS_LOCK"         },
    {KEY_SCROLL_LOCK,      "KEY_SCROLL_LOCK"       },
    {KEY_NUM_LOCK,         "KEY_NUM_LOCK"          },
    {KEY_PRINT_SCREEN,     "KEY_PRINT_SCREEN"      },
    {KEY_PAUSE,            "KEY_PAUSE"             },
    {KEY_F1,               "KEY_F1"                },
    {KEY_F2,               "KEY_F2"                },
    {KEY_F3,               "KEY_F3"                },
    {KEY_F4,               "KEY_F4"                },
    {KEY_F5,               "KEY_F5"                },
    {KEY_F6,               "KEY_F6"                },
    {KEY_F7,               "KEY_F7"                },
    {KEY_F8,               "KEY_F8"                },
    {KEY_F9,               "KEY_F9"                },
    {KEY_F10,              "KEY_F10"               },
    {KEY_F11,              "KEY_F11"               },
    {KEY_F12,              "KEY_F12"               },
    {KEY_F13,              "KEY_F13"               },
    {KEY_F14,              "KEY_F14"               },
    {KEY_F15,              "KEY_F15"               },
    {KEY_F16,              "KEY_F16"               },
    {KEY_F17,              "KEY_F17"               },
    {KEY_F18,              "KEY_F18"               },
    {KEY_F19,              "KEY_F19"               },
    {KEY_F20,              "KEY_F20"               },
    {KEY_F21,              "KEY_F21"               },
    {KEY_F22,              "KEY_F22"               },
    {KEY_F23,              "KEY_F23"               },
    {KEY_F24,              "KEY_F24"               },
    {KEY_F25,              "KEY_F25"               },
    {KEY_KEY_PAD_0,        "KEY_KEY_PAD_0"         },
    {KEY_KEY_PAD_1,        "KEY_KEY_PAD_1"         },
    {KEY_KEY_PAD_2,        "KEY_KEY_PAD_2"         },
    {KEY_KEY_PAD_3,        "KEY_KEY_PAD_3"         },
    {KEY_KEY_PAD_4,        "KEY_KEY_PAD_4"         },
    {KEY_KEY_PAD_5,        "KEY_KEY_PAD_5"         },
    {KEY_KEY_PAD_6,        "KEY_KEY_PAD_6"         },
    {KEY_KEY_PAD_7,        "KEY_KEY_PAD_7"         },
    {KEY_KEY_PAD_8,        "KEY_KEY_PAD_8"         },
    {KEY_KEY_PAD_9,        "KEY_KEY_PAD_9"         },
    {KEY_KEY_PAD_DECIMAL,  "KEY_KEY_PAD_DECIMAL"   },
    {KEY_KEY_PAD_DIVIDE,   "KEY_KEY_PAD_DIVIDE"    },
    {KEY_KEY_PAD_MULTIPLY, "KEY_KEY_PAD_MULTIPLY"  },
    {KEY_KEY_PAD_SUBTRACT, "KEY_KEY_PAD_SUBTRACT"  },
    {KEY_KEY_PAD_ADD,      "KEY_KEY_PAD_ADD"       },
    {KEY_KEY_PAD_ENTER,    "KEY_KEY_PAD_ENTER"     },
    {KEY_KEY_PAD_EQUAL,    "KEY_KEY_PAD_EQUAL"     },
    {KEY_LEFT_SHIFT,       "KEY_LEFT_SHIFT"        },
    {KEY_LEFT_CONTROL,     "KEY_LEFT_CONTROL"      },
    {KEY_LEFT_ALT,         "KEY_LEFT_ALT"          },
    {KEY_LEFT_SUPER,       "KEY_LEFT_SUPER"        },
    {KEY_RIGHT_SHIFT,      "KEY_RIGHT_SHIFT"       },
    {KEY_RIGHT_CONTROL,    "KEY_RIGHT_CONTROL"     },
    {KEY_RIGHT_ALT,        "KEY_RIGHT_ALT"         },
    {KEY_RIGHT_SUPER,      "KEY_RIGHT_SUPER"       },
    {KEY_MENU,             "KEY_MENU"              },
};
// clang-format on

// -------------------------------------------------------------------------------------------------
// WindowEvents
// -------------------------------------------------------------------------------------------------
// clang-format off
static std::map<int32_t, int32_t> sKeyCodeMap = {
    { GLFW_KEY_SPACE,           KEY_SPACE            },
    { GLFW_KEY_APOSTROPHE,      KEY_APOSTROPHE       },
    { GLFW_KEY_COMMA,           KEY_COMMA            },
    { GLFW_KEY_MINUS,           KEY_MINUS            },
    { GLFW_KEY_PERIOD,          KEY_PERIOD           },
    { GLFW_KEY_SLASH,           KEY_SLASH            },
    { GLFW_KEY_0,               KEY_0                },
    { GLFW_KEY_1,               KEY_1                },
    { GLFW_KEY_2,               KEY_2                },
    { GLFW_KEY_3,               KEY_3                },
    { GLFW_KEY_4,               KEY_4                },
    { GLFW_KEY_5,               KEY_5                },
    { GLFW_KEY_6,               KEY_6                },
    { GLFW_KEY_7,               KEY_7                },
    { GLFW_KEY_8,               KEY_8                },
    { GLFW_KEY_9,               KEY_9                },
    { GLFW_KEY_SEMICOLON,       KEY_SEMICOLON        },
    { GLFW_KEY_EQUAL,           KEY_EQUAL            },
    { GLFW_KEY_A,               KEY_A                },
    { GLFW_KEY_B,               KEY_B                },
    { GLFW_KEY_C,               KEY_C                },
    { GLFW_KEY_D,               KEY_D                },
    { GLFW_KEY_E,               KEY_E                },
    { GLFW_KEY_F,               KEY_F                },
    { GLFW_KEY_G,               KEY_G                },
    { GLFW_KEY_H,               KEY_H                },
    { GLFW_KEY_I,               KEY_I                },
    { GLFW_KEY_J,               KEY_J                },
    { GLFW_KEY_K,               KEY_K                },
    { GLFW_KEY_L,               KEY_L                },
    { GLFW_KEY_M,               KEY_M                },
    { GLFW_KEY_N,               KEY_N                },
    { GLFW_KEY_O,               KEY_O                },
    { GLFW_KEY_P,               KEY_P                },
    { GLFW_KEY_Q,               KEY_Q                },
    { GLFW_KEY_R,               KEY_R                },
    { GLFW_KEY_S,               KEY_S                },
    { GLFW_KEY_T,               KEY_T                },
    { GLFW_KEY_U,               KEY_U                },
    { GLFW_KEY_V,               KEY_V                },
    { GLFW_KEY_W,               KEY_W                },
    { GLFW_KEY_X,               KEY_X                },
    { GLFW_KEY_Y,               KEY_Y                },
    { GLFW_KEY_Z,               KEY_Z                },
    { GLFW_KEY_LEFT_BRACKET,    KEY_LEFT_BRACKET     },
    { GLFW_KEY_BACKSLASH,       KEY_BACKSLASH        },
    { GLFW_KEY_RIGHT_BRACKET,   KEY_RIGHT_BRACKET    },
    { GLFW_KEY_GRAVE_ACCENT,    KEY_GRAVE_ACCENT     },
    { GLFW_KEY_WORLD_1,         KEY_WORLD_1          },
    { GLFW_KEY_WORLD_2,         KEY_WORLD_2          },
    { GLFW_KEY_ESCAPE,          KEY_ESCAPE           },
    { GLFW_KEY_ENTER,           KEY_ENTER            },
    { GLFW_KEY_TAB,             KEY_TAB              },
    { GLFW_KEY_BACKSPACE,       KEY_BACKSPACE        },
    { GLFW_KEY_INSERT,          KEY_INSERT           },
    { GLFW_KEY_DELETE,          KEY_DELETE           },
    { GLFW_KEY_RIGHT,           KEY_RIGHT            },
    { GLFW_KEY_LEFT,            KEY_LEFT             },
    { GLFW_KEY_DOWN,            KEY_DOWN             },
    { GLFW_KEY_UP,              KEY_UP               },
    { GLFW_KEY_PAGE_UP,         KEY_PAGE_UP          },
    { GLFW_KEY_PAGE_DOWN,       KEY_PAGE_DOWN        },
    { GLFW_KEY_HOME,            KEY_HOME             },
    { GLFW_KEY_END,             KEY_END              },
    { GLFW_KEY_CAPS_LOCK,       KEY_CAPS_LOCK        },
    { GLFW_KEY_SCROLL_LOCK,     KEY_SCROLL_LOCK      },
    { GLFW_KEY_NUM_LOCK,        KEY_NUM_LOCK         },
    { GLFW_KEY_PRINT_SCREEN,    KEY_PRINT_SCREEN     },
    { GLFW_KEY_PAUSE,           KEY_PAUSE            },
    { GLFW_KEY_F1,              KEY_F1               },
    { GLFW_KEY_F2,              KEY_F2               },
    { GLFW_KEY_F3,              KEY_F3               },
    { GLFW_KEY_F4,              KEY_F4               },
    { GLFW_KEY_F5,              KEY_F5               },
    { GLFW_KEY_F6,              KEY_F6               },
    { GLFW_KEY_F7,              KEY_F7               },
    { GLFW_KEY_F8,              KEY_F8               },
    { GLFW_KEY_F9,              KEY_F9               },
    { GLFW_KEY_F10,             KEY_F10              },
    { GLFW_KEY_F11,             KEY_F11              },
    { GLFW_KEY_F12,             KEY_F12              },
    { GLFW_KEY_F13,             KEY_F13              },
    { GLFW_KEY_F14,             KEY_F14              },
    { GLFW_KEY_F15,             KEY_F15              },
    { GLFW_KEY_F16,             KEY_F16              },
    { GLFW_KEY_F17,             KEY_F17              },
    { GLFW_KEY_F18,             KEY_F18              },
    { GLFW_KEY_F19,             KEY_F19              },
    { GLFW_KEY_F20,             KEY_F20              },
    { GLFW_KEY_F21,             KEY_F21              },
    { GLFW_KEY_F22,             KEY_F22              },
    { GLFW_KEY_F23,             KEY_F23              },
    { GLFW_KEY_F24,             KEY_F24              },
    { GLFW_KEY_F25,             KEY_F25              },
    { GLFW_KEY_KP_0,            KEY_KEY_PAD_0        },
    { GLFW_KEY_KP_1,            KEY_KEY_PAD_1        },
    { GLFW_KEY_KP_2,            KEY_KEY_PAD_2        },
    { GLFW_KEY_KP_3,            KEY_KEY_PAD_3        },
    { GLFW_KEY_KP_4,            KEY_KEY_PAD_4        },
    { GLFW_KEY_KP_5,            KEY_KEY_PAD_5        },
    { GLFW_KEY_KP_6,            KEY_KEY_PAD_6        },
    { GLFW_KEY_KP_7,            KEY_KEY_PAD_7        },
    { GLFW_KEY_KP_8,            KEY_KEY_PAD_8        },
    { GLFW_KEY_KP_9,            KEY_KEY_PAD_9        },
    { GLFW_KEY_KP_DECIMAL,      KEY_KEY_PAD_DECIMAL  },
    { GLFW_KEY_KP_DIVIDE,       KEY_KEY_PAD_DIVIDE   },
    { GLFW_KEY_KP_MULTIPLY,     KEY_KEY_PAD_MULTIPLY },
    { GLFW_KEY_KP_SUBTRACT,     KEY_KEY_PAD_SUBTRACT },
    { GLFW_KEY_KP_ADD,          KEY_KEY_PAD_ADD      },
    { GLFW_KEY_KP_ENTER,        KEY_KEY_PAD_ENTER    },
    { GLFW_KEY_KP_EQUAL,        KEY_KEY_PAD_EQUAL    },
    { GLFW_KEY_LEFT_SHIFT,      KEY_LEFT_SHIFT       },
    { GLFW_KEY_LEFT_CONTROL,    KEY_LEFT_CONTROL     },
    { GLFW_KEY_LEFT_ALT,        KEY_LEFT_ALT         },
    { GLFW_KEY_LEFT_SUPER,      KEY_LEFT_SUPER       },
    { GLFW_KEY_RIGHT_SHIFT,     KEY_RIGHT_SHIFT      },
    { GLFW_KEY_RIGHT_CONTROL,   KEY_RIGHT_CONTROL    },
    { GLFW_KEY_RIGHT_ALT,       KEY_RIGHT_ALT        },
    { GLFW_KEY_RIGHT_SUPER,     KEY_RIGHT_SUPER      },
    { GLFW_KEY_MENU,            KEY_MENU             },
};
// clang-format on

// -------------------------------------------------------------------------------------------------
// WindowEvents
// -------------------------------------------------------------------------------------------------
struct WindowEvents
{
    static std::unordered_map<GLFWwindow*, Application*> sWindows;

    static void MoveCallback(GLFWwindow* window, int event_x, int event_y)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }

        Application* p_application = it->second;
        p_application->MoveCallback(
            static_cast<int32_t>(event_x),
            static_cast<int32_t>(event_y));
    }

    static void ResizeCallback(GLFWwindow* window, int event_width, int event_height)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }

        Application* p_application = it->second;
        p_application->ResizeCallback(
            static_cast<uint32_t>(event_width),
            static_cast<uint32_t>(event_height));
    }

    static void MouseButtonCallback(GLFWwindow* window, int event_button, int event_action, int event_mods)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }

        uint32_t buttons = 0;
        if (event_button == GLFW_MOUSE_BUTTON_LEFT) {
            buttons |= MOUSE_BUTTON_LEFT;
        }
        if (event_button == GLFW_MOUSE_BUTTON_RIGHT) {
            buttons |= MOUSE_BUTTON_RIGHT;
        }
        if (event_button == GLFW_MOUSE_BUTTON_MIDDLE) {
            buttons |= MOUSE_BUTTON_MIDDLE;
        }

        double event_x;
        double event_y;
        glfwGetCursorPos(window, &event_x, &event_y);

        Application* p_application = it->second;
        if (event_action == GLFW_PRESS) {
            p_application->MouseDownCallback(
                static_cast<int32_t>(event_x),
                static_cast<int32_t>(event_y),
                buttons);
        }
        else if (event_action == GLFW_RELEASE) {
            p_application->MouseUpCallback(
                static_cast<int32_t>(event_x),
                static_cast<int32_t>(event_y),
                buttons);
        }
        ImGui_ImplGlfw_MouseButtonCallback(window, event_button, event_action, event_mods);
    }

    static void MouseMoveCallback(GLFWwindow* window, double event_x, double event_y)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }

        uint32_t buttons = 0;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            buttons |= MOUSE_BUTTON_LEFT;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            buttons |= MOUSE_BUTTON_RIGHT;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            buttons |= MOUSE_BUTTON_MIDDLE;
        }

        Application* p_application = it->second;
        p_application->MouseMoveCallback(
            static_cast<int32_t>(event_x),
            static_cast<int32_t>(event_y),
            buttons);
    }

    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }
        ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
    }

    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }

        bool hasKey = (sKeyCodeMap.find(key) != sKeyCodeMap.end());
        if (!hasKey) {
            PPX_LOG_WARN("GLFW key not supported, key=" << key);
        }

        if (hasKey) {
            KeyCode appKey = static_cast<KeyCode>(sKeyCodeMap[key]);

            Application* p_application = it->second;
            if (action == GLFW_PRESS) {
                p_application->KeyDownCallback(appKey);
            }
            else if (action == GLFW_RELEASE) {
                p_application->KeyUpCallback(appKey);
            }
        }

        ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    }

    static void CharCallback(GLFWwindow* window, unsigned int c)
    {
        auto it = sWindows.find(window);
        if (it == sWindows.end()) {
            return;
        }
        ImGui_ImplGlfw_CharCallback(window, c);
    }

    static Result RegisterWindowEvents(GLFWwindow* window, Application* application)
    {
        auto it = sWindows.find(window);
        if (it != sWindows.end()) {
            return ppx::ERROR_WINDOW_EVENTS_ALREADY_REGISTERED;
        }

        glfwSetWindowPosCallback(window, WindowEvents::MoveCallback);
        glfwSetWindowSizeCallback(window, WindowEvents::ResizeCallback);
        glfwSetMouseButtonCallback(window, WindowEvents::MouseButtonCallback);
        glfwSetCursorPosCallback(window, WindowEvents::MouseMoveCallback);
        glfwSetScrollCallback(window, WindowEvents::ScrollCallback);
        glfwSetKeyCallback(window, WindowEvents::KeyCallback);
        glfwSetCharCallback(window, WindowEvents::CharCallback);

        sWindows[window] = application;

        return ppx::SUCCESS;
    }
};

std::unordered_map<GLFWwindow*, Application*> WindowEvents::sWindows;

// -------------------------------------------------------------------------------------------------
// Application
// -------------------------------------------------------------------------------------------------
Application::Application()
{
    InternalCtor();

    mSettings.appName       = kDefaultAppName;
    mSettings.window.width  = kDefaultWindowWidth;
    mSettings.window.height = kDefaultWindowHeight;
}

Application::Application(uint32_t windowWidth, uint32_t windowHeight, const char* windowTitle)
{
    InternalCtor();

    mSettings.appName       = windowTitle;
    mSettings.window.width  = windowWidth;
    mSettings.window.height = windowHeight;
    mSettings.window.title  = windowTitle;
}

Application::~Application()
{
    if (sApplicationInstance == this) {
        sApplicationInstance = nullptr;
    }
}
void Application::InternalCtor()
{
    if (IsNull(sApplicationInstance)) {
        sApplicationInstance = this;
    }

    InitializeAssetDirs();
}

Application* Application::Get()
{
    return sApplicationInstance;
}


void Application::InitializeAssetDirs()
{
    fs::path appPath = GetApplicationPath();
    PPX_LOG_INFO("Application path: " << appPath);
    fs::path baseDir = appPath.parent();
    AddAssetDir(baseDir);
    size_t n = baseDir.part_count();
    for (size_t i = 0; i < n; ++i) {
        fs::path assetDir = baseDir / "assets";
        if (fs::exists(assetDir)) {
            AddAssetDir(assetDir);
            PPX_LOG_INFO("Added asset path: " << assetDir);
        }
        baseDir = baseDir.parent();
    }
}

Result Application::InitializePlatform()
{
    int res = glfwInit();
    if (res != GLFW_TRUE) {
        PPX_ASSERT_MSG(false, "glfwInit failed");
        return ppx::ERROR_GLFW_INIT_FAILED;
    }
    return ppx::SUCCESS;
}

Result Application::InitializeGrfxDevice()
{
    // Instance
    {
        if (mInstance) {
            return ppx::ERROR_SINGLE_INIT_ONLY;
        }

        grfx::InstanceCreateInfo ci = {};
        ci.api                      = mSettings.grfx.api;
        ci.createDevices            = false;
        ci.enableDebug              = mSettings.grfx.enableDebug;
        ci.enableSwapchain          = true;
        ci.applicationName          = mSettings.appName;
        ci.engineName               = mSettings.appName;

        Result ppxres = grfx::CreateInstance(&ci, &mInstance);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::CreateInstance failed");
            return ppxres;
        }
    }

    // Device
    {
        if (mDevice) {
            return ppx::ERROR_SINGLE_INIT_ONLY;
        }

        grfx::GpuPtr gpu;
        Result       ppxres = mInstance->GetGpu(0, &gpu);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Instance::GetGpu failed");
            return ppxres;
        }

        grfx::DeviceCreateInfo ci = {};
        ci.pGpu                   = gpu;
        ci.graphicsQueueCount     = mSettings.grfx.device.graphicsQueueCount;
        ci.computeQueueCount      = mSettings.grfx.device.computeQueueCount;
        ci.transferQueueCount     = mSettings.grfx.device.transferQueueCount;
        ci.vulkanExtensions       = {};
        ci.pVulkanDeviceFeatures  = nullptr;
        ci.enableDXIL             = mSettings.grfx.enableDXIL;

        PPX_LOG_INFO("Creating application graphics device using " << gpu->GetDeviceName());
        PPX_LOG_INFO("   requested graphics queue count : " << mSettings.grfx.device.graphicsQueueCount);
        PPX_LOG_INFO("   requested compute  queue count : " << mSettings.grfx.device.computeQueueCount);
        PPX_LOG_INFO("   requested transfer queue count : " << mSettings.grfx.device.transferQueueCount);

        ppxres = mInstance->CreateDevice(&ci, &mDevice);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Instance::CreateDevice failed");
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

Result Application::InitializeGrfxSurface()
{
    // Surface
    {
        grfx::SurfaceCreateInfo ci = {};
        ci.pGpu                    = mDevice->GetGpu();
#if defined(PPX_GGP)
        // Nothing to do
#elif defined(PPX_LINUX_XCB)
        ci.connection = XGetXCBConnection(glfwGetX11Display());
        ci.window     = glfwGetX11Window(static_cast<GLFWwindow*>(mWindow));
#elif defined(PPX_LINUX_XLIB)
#error "Xlib not implemented"
#elif defined(PPX_LINUX_WAYLAND)
#error "Wayland not implemented"
#elif defined(PPX_MSW)
        ci.hinstance = ::GetModuleHandle(nullptr);
        ci.hwnd      = glfwGetWin32Window(static_cast<GLFWwindow*>(mWindow));
#endif

        Result ppxres = mInstance->CreateSurface(&ci, &mSurface);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Instance::CreateSurface failed");
            return ppxres;
        }
    }

    // Swapchain
    {
        PPX_LOG_INFO("Creating application swapchain");
        PPX_LOG_INFO("   image count : " << mSettings.grfx.swapchain.imageCount);

        const uint32_t surfaceMinImageCount = mSurface->GetMinImageCount();
        if (mSettings.grfx.swapchain.imageCount < surfaceMinImageCount) {
            PPX_LOG_WARN("readjusting swapchain's image count from " << mSettings.grfx.swapchain.imageCount << " to " << surfaceMinImageCount << " to match surface requirements");
            mSettings.grfx.swapchain.imageCount = surfaceMinImageCount;
        }

#if defined(PPX_GGP)
        const uint32_t surfaceMinImageWidth  = mSurface->GetMinImageWidth();
        const uint32_t surfaceMinImageHeight = mSurface->GetMinImageHeight();
        if ((surfaceMinImageWidth > mSettings.window.width) || (surfaceMinImageHeight > mSettings.window.height)) {
            PPX_LOG_WARN("readjusting swapchain/window size from " << mSettings.window.width << "x" << mSettings.window.height << " to " << surfaceMinImageWidth << "x" << surfaceMinImageHeight << " to match surface requirements");
            mSettings.window.width  = surfaceMinImageWidth;
            mSettings.window.height = surfaceMinImageHeight;
        }
#endif

        grfx::SwapchainCreateInfo ci = {};
        ci.pQueue                    = mDevice->GetGraphicsQueue();
        ci.pSurface                  = mSurface;
        ci.width                     = mSettings.window.width;
        ci.height                    = mSettings.window.height;
        ci.colorFormat               = mSettings.grfx.swapchain.colorFormat;
        ci.depthFormat               = mSettings.grfx.swapchain.depthFormat;
        ci.imageCount                = mSettings.grfx.swapchain.imageCount;
        ci.presentMode               = grfx::PRESENT_MODE_IMMEDIATE;

        Result ppxres = mDevice->CreateSwapchain(&ci, &mSwapchain);
        if (Failed(ppxres)) {
            PPX_ASSERT_MSG(false, "grfx::Device::CreateSwapchain failed");
            return ppxres;
        }
    }

    return ppx::SUCCESS;
}

Result Application::InitializeImGui()
{
    switch (mSettings.grfx.api) {
        default: {
            PPX_ASSERT_MSG(false, "[imgui] unknown graphics API");
            return ppx::ERROR_UNSUPPORTED_API;
        } break;

        case grfx::API_VK_1_1:
        case grfx::API_VK_1_2: {
            mImGui = std::unique_ptr<ImGuiImpl>(new ImGuiImplVk());
        } break;

#if defined(PPX_D3D12)
        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            mImGui = std::unique_ptr<ImGuiImpl>(new ImGuiImplDx());
        } break;
#endif // defined(PPX_D3D12)
    }

    Result ppxres = mImGui->Init(this);
    if (Failed(ppxres)) {
        return ppxres;
    }

    return ppx::SUCCESS;
}

void Application::ShutdownImGui()
{
    if (mImGui) {
        mImGui->Shutdown(this);
        mImGui.reset();
    }
}

void Application::StopGrfx()
{
    if (mDevice) {
        mDevice->WaitIdle();
    }
}

void Application::ShutdownGrfx()
{
    if (mInstance) {
        if (mSwapchain) {
            mDevice->DestroySwapchain(mSwapchain);
            mSwapchain.Reset();
        }

        if (mDevice) {
            mInstance->DestroyDevice(mDevice);
            mDevice.Reset();
        }

        if (mSurface) {
            mInstance->DestroySurface(mSurface);
            mSurface.Reset();
        }

        grfx::DestroyInstance(mInstance);
        mInstance.Reset();
    }
}

Result Application::CreatePlatformWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, mSettings.window.resizable ? GLFW_TRUE : GLFW_FALSE);

    // Decorated window title
    std::stringstream windowTitle;
    windowTitle << mSettings.window.title << " | " << ToString(mSettings.grfx.api) << " | " << mDevice->GetDeviceName();

    GLFWwindow* pWindow = glfwCreateWindow(
        static_cast<int>(mSettings.window.width),
        static_cast<int>(mSettings.window.height),
        windowTitle.str().c_str(),
        nullptr,
        nullptr);
    if (IsNull(pWindow)) {
        PPX_ASSERT_MSG(false, "glfwCreateWindow failed");
        return ppx::ERROR_GLFW_CREATE_WINDOW_FAILED;
    }

    // Register window events
    Result ppxres = WindowEvents::RegisterWindowEvents(pWindow, this);
    if (Failed(ppxres)) {
        PPX_ASSERT_MSG(false, "RegisterWindowEvents failed");
        return ppxres;
    }

    mWindow = static_cast<void*>(pWindow);

    return ppx::SUCCESS;
}

void Application::DestroyPlatformWindow()
{
    if (!IsNull(mWindow)) {
        GLFWwindow* pWindow = static_cast<GLFWwindow*>(mWindow);
        glfwDestroyWindow(pWindow);
        mWindow = nullptr;
    }
}

void Application::DispatchConfig()
{
    Config(mSettings);

    if (mSettings.appName.empty()) {
        mSettings.appName = "PPX Application";
    }

    if (mSettings.window.title.empty()) {
        mSettings.window.title = mSettings.appName;
    }

    // Decorate DX's API name with shader bytecode mode
    std::stringstream ss;
    ss << ToString(mSettings.grfx.api);
    switch (mSettings.grfx.api) {
        default: break;
        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            if (mSettings.grfx.enableDXIL) {
                ss << " (DXIL)";
            }
            else {
                ss << " (DXBC)";
            }
        } break;
    }
    mDecoratedApiName = ss.str();
}

void Application::DispatchSetup()
{
    Setup();
}

void Application::DispatchShutdown()
{
    Shutdown();
}

void Application::DispatchMove(int32_t x, int32_t y)
{
    Move(x, y);
}

void Application::DispatchResize(uint32_t width, uint32_t height)
{
    Resize(width, height);
}

void Application::DispatchKeyDown(KeyCode key)
{
    KeyDown(key);
}

void Application::DispatchKeyUp(KeyCode key)
{
    KeyUp(key);
}

void Application::DispatchMouseMove(int32_t x, int32_t y, int32_t dx, int32_t dy, uint32_t buttons)
{
    MouseMove(x, y, dx, dy, buttons);
}

void Application::DispatchMouseDown(int32_t x, int32_t y, uint32_t buttons)
{
    MouseDown(x, y, buttons);
}

void Application::DispatchMouseUp(int32_t x, int32_t y, uint32_t buttons)
{
    MouseUp(x, y, buttons);
}

void Application::DispatchRender()
{
    Render();
}

void Application::MoveCallback(int32_t x, int32_t y)
{
    Move(x, y);
}

void Application::ResizeCallback(uint32_t width, uint32_t height)
{
    bool widthChanged  = (width != mSettings.window.width);
    bool heightChanged = (height != mSettings.window.height);
    if (widthChanged || heightChanged) {
        // Update the configuration's width and height
        mSettings.window.width  = width;
        mSettings.window.height = height;
        mWindowSurfaceInvalid   = ((width == 0) || (height == 0));
    }
}

void Application::MouseDownCallback(int32_t x, int32_t y, uint32_t buttons)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    DispatchMouseDown(x, y, buttons);
}

void Application::MouseUpCallback(int32_t x, int32_t y, uint32_t buttons)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    DispatchMouseUp(x, y, buttons);
}

void Application::MouseMoveCallback(int32_t x, int32_t y, uint32_t buttons)
{
    if (ImGui::GetIO().WantCaptureMouse) {
        return;
    }

    int32_t dx = (mPrevMouseX != INT32_MAX) ? (x - mPrevMouseX) : 0;
    int32_t dy = (mPrevMouseY != INT32_MAX) ? (y - mPrevMouseY) : 0;
    DispatchMouseMove(x, y, dx, dy, buttons);
    mPrevMouseX = x;
    mPrevMouseY = y;
}

void Application::KeyUpCallback(KeyCode key)
{
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    mKeyStates[key].down     = false;
    mKeyStates[key].timeDown = FLT_MAX;
    DispatchKeyUp(key);
}

void Application::KeyDownCallback(KeyCode key)
{
    if (ImGui::GetIO().WantCaptureKeyboard) {
        return;
    }

    mKeyStates[key].down     = true;
    mKeyStates[key].timeDown = GetElapsedSeconds();
    DispatchKeyDown(key);
}

void Application::DrawImGui(grfx::CommandBuffer* pCommandBuffer)
{
    if (!mImGui) {
        return;
    }

    mImGui->Render(pCommandBuffer);
}

bool Application::IsRunning() const
{
    bool isRunning = (glfwWindowShouldClose(static_cast<GLFWwindow*>(mWindow)) == 0);
    return isRunning;
}

int Application::Run(int argc, char** argv)
{
    // Only allow one instance of Application. Since we can't stop
    // the app in the ctor - stop it here.
    //
    if (this != sApplicationInstance) {
        return false;
    }

    // Initialize the platform
    Result ppxres = InitializePlatform();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Call config
    DispatchConfig();

    // Create graphics instance
    ppxres = InitializeGrfxDevice();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Create window
    ppxres = CreatePlatformWindow();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Create surface
    ppxres = InitializeGrfxSurface();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Update the window size if the settings got changed due to surface requiremetns
    {
        int windowWidth = 0;
        int windowHeight = 0;
        glfwGetWindowSize(static_cast<GLFWwindow*>(mWindow), &windowWidth, &windowHeight);
        if ((static_cast<uint32_t>(windowWidth) != mSettings.window.width) || (static_cast<uint32_t>(windowHeight) != mSettings.window.width)) {
           glfwSetWindowSize(
              static_cast<GLFWwindow*>(mWindow),
              static_cast<int>(mSettings.window.width),
              static_cast<int>(mSettings.window.height));
        }
    }

    // Setup ImGui
    ppxres = InitializeImGui();
    if (Failed(ppxres)) {
        return EXIT_FAILURE;
    }

    // Call setup
    DispatchSetup();

    // ---------------------------------------------------------------------------------------------
    // Main loop [BEGIN]
    // ---------------------------------------------------------------------------------------------

    // Initialize and start timer
    ppx::TimerResult tmres = ppx::Timer::InitializeStaticData();
    if (tmres != ppx::TIMER_RESULT_SUCCESS) {
        return EXIT_FAILURE;
    }
    tmres = mTimer.Start();

    mRunning = true;
    while (IsRunning()) {
        // Frame start
        mFrameStartTime = static_cast<float>(mTimer.MillisSinceStart());

        // Poll events
        glfwPollEvents();

        // Start new Imgui frame
        if (mImGui) {
            mImGui->NewFrame();
        }

        // Call render
        DispatchRender();

        // Frame end
        mFrameCount        = mFrameCount + 1;
        mAverageFPS        = static_cast<float>(mFrameCount / mTimer.SecondsSinceStart());
        mFrameEndTime      = static_cast<float>(mTimer.MillisSinceStart());
        mPreviousFrameTime = mFrameEndTime - mFrameStartTime;

        // Pace ffames - if needed
        if (mSettings.grfx.pacedFrameRate > 0) {
            if (mFrameCount > 0) {
                double currentTime  = mTimer.SecondsSinceStart();
                double pacedFPS     = 1.0 / static_cast<double>(mSettings.grfx.pacedFrameRate);
                double expectedTime = mFirstFrameTime + (mFrameCount * pacedFPS);
                double diff         = expectedTime - currentTime;
                if (diff > 0) {
                    Timer::SleepSeconds(diff);
                }
            }
            else {
                mFirstFrameTime = mTimer.SecondsSinceStart();
            }
        }
    }
    // ---------------------------------------------------------------------------------------------
    // Main loop [END]
    // ---------------------------------------------------------------------------------------------

    // Stop graphics first before shutting down to make sure
    // that there aren't any command buffers in flight.
    //
    StopGrfx();

    // Call shutdown
    DispatchShutdown();

    // Shutdown Imgui
    ShutdownImGui();

    // Shutdown graphics
    ShutdownGrfx();

    // Destroy window
    DestroyPlatformWindow();

    // Success
    return EXIT_SUCCESS;
}

grfx::Rect Application::GetScissor() const
{
    grfx::Rect rect = {};
    rect.x          = 0;
    rect.y          = 0;
    rect.width      = GetWindowWidth();
    rect.height     = GetWindowHeight();
    return rect;
}

grfx::Viewport Application::GetViewport(float minDepth, float maxDepth) const
{
    grfx::Viewport viewport = {};
    viewport.x              = 0.0f;
    viewport.y              = 0.0f;
    viewport.width          = static_cast<float>(GetWindowWidth());
    viewport.height         = static_cast<float>(GetWindowHeight());
    viewport.minDepth       = minDepth;
    viewport.maxDepth       = maxDepth;
    return viewport;
}

uint32_t Application::GetProcessId() const
{
    uint32_t pid = UINT32_MAX;
#if defined(PPX_LINUX)
    pid = static_cast<uint32_t>(getpid());
#elif defined(PPX_MSW)
    pid = static_cast<uint32_t>(::GetCurrentProcessId());
#endif
    return pid;
}

fs::path Application::GetApplicationPath() const
{
    fs::path path;
#if defined(PPX_LINUX) || defined(PPX_GGP)
    char buf[PATH_MAX];
    std::memset(buf, 0, PATH_MAX);
    readlink("/proc/self/exe", buf, PATH_MAX);
    path = fs::path(buf);
#elif defined(PPX_MSW)
    HMODULE this_win32_module = GetModuleHandleA(nullptr);
    char buf[MAX_PATH];
    std::memset(buf, 0, MAX_PATH);
    GetModuleFileNameA(this_win32_module, buf, MAX_PATH);
    path = fs::path(buf);
#else
#error "not implemented"
#endif
    return path;
}

void Application::AddAssetDir(const fs::path& path, bool insertAtFront)
{
    auto it = Find(mAssetDirs, path);
    if (it != std::end(mAssetDirs)) {
        return;
    }

    if (!fs::is_directory(path)) {
        return;
    }

    mAssetDirs.push_back(path);

    if (insertAtFront) {
        // Rotate to front
        std::rotate(
            std::rbegin(mAssetDirs),
            std::rbegin(mAssetDirs) + 1,
            std::rend(mAssetDirs));
    }
}

fs::path Application::GetAssetPath(const fs::path& subPath) const
{
    fs::path assetPath;
    for (auto& assetDir : mAssetDirs) {
        fs::path path = assetDir / subPath;
        if (fs::exists(path)) {
            assetPath = path;
            break;
        }
    }
    return assetPath;
}

std::vector<char> Application::LoadShader(const fs::path& baseDir, const std::string& baseName) const
{
    fs::path filePath = baseDir;
    switch (mSettings.grfx.api) {
        default: {
            PPX_ASSERT_MSG(false, "unsupported API");
        } break;

        case grfx::API_VK_1_1:
        case grfx::API_VK_1_2: {
            filePath = (filePath / "spv" / baseName).append_extension(".spv");
        } break;

        case grfx::API_DX_12_0:
        case grfx::API_DX_12_1: {
            if (mSettings.grfx.enableDXIL) {
                filePath = (filePath / "dxil" / baseName).append_extension(".dxil");
            }
            else {
                filePath = (filePath / "dxbc" / baseName).append_extension(".dxbc");
            }
        } break;
    }

    if (!fs::exists(filePath)) {
        PPX_ASSERT_MSG(false, "shader file not found: " << filePath);
    }

    std::vector<char> bytecode = fs::load_file(filePath);
    return bytecode;
}

float Application::GetElapsedSeconds() const
{
    return static_cast<float>(mTimer.SecondsSinceStart());
}

const KeyState& Application::GetKeyState(KeyCode code) const
{
    if ((code < KEY_RANGE_FIRST) || (code >= KEY_RANGE_LAST)) {
        return mKeyStates[0];
    }
    return mKeyStates[static_cast<uint32_t>(code)];
}

void Application::DrawDebugInfo(std::function<void(void)> drawAdditionalFn)
{
    if (!mImGui) {
        return;
    }

    if (ImGui::Begin("Debug Info")) {
        ImGui::Columns(2);

        // Application PID
        {
            ImGui::Text("Application PID");
            ImGui::NextColumn();
            ImGui::Text("%d", GetProcessId());
            ImGui::NextColumn();
        }

        ImGui::Separator();

        // API
        {
            ImGui::Text("API");
            ImGui::NextColumn();
            ImGui::Text("%s", mDecoratedApiName.c_str());
            ImGui::NextColumn();
        }

        // GPU
        {
            ImGui::Text("GPU");
            ImGui::NextColumn();
            ImGui::Text("%s", GetDevice()->GetDeviceName());
            ImGui::NextColumn();
        }

        ImGui::Separator();

        // Average FPS
        {
            ImGui::Text("Frame Count");
            ImGui::NextColumn();
            ImGui::Text("%lu", mFrameCount);
            ImGui::NextColumn();
        }

        // Average FPS
        {
            ImGui::Text("Average FPS");
            ImGui::NextColumn();
            ImGui::Text("%f", mAverageFPS);
            ImGui::NextColumn();
        }

        // Previous frame time
        {
            ImGui::Text("Previous Frame Time");
            ImGui::NextColumn();
            ImGui::Text("%f ms", mPreviousFrameTime);
            ImGui::NextColumn();
        }

        ImGui::Columns(1);

        // Draw additional elements
        if (drawAdditionalFn) {
            drawAdditionalFn();
        }
    }
    ImGui::End();
}

const char* GetKeyCodeString(KeyCode code)
{
    if ((code < KEY_RANGE_FIRST) || (code >= KEY_RANGE_LAST)) {
        return sKeyCodeString[0];
    }
    return sKeyCodeString[static_cast<uint32_t>(code)];
}

} // namespace ppx
