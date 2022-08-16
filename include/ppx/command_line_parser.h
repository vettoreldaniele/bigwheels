#ifndef ppx_command_line_parser_h
#define ppx_command_line_parser_h

#include <ios>
#include <set>
#include <string>
#include <sstream>
#include <unordered_map>
#include <type_traits>
#include <optional>

namespace ppx {
// -------------------------------------------------------------------------------------------------
// StandardOptions
// -------------------------------------------------------------------------------------------------
struct StandardOptions
{
    // Flags
    bool help                  = false;
    bool list_gpus             = false;
    bool use_software_renderer = false;

    // Options
    int                 gpu_index   = -1;
    std::pair<int, int> resolution  = {-1, -1};
    int                 frame_count = -1;

    bool operator==(const StandardOptions&) const = default;
};

// -------------------------------------------------------------------------------------------------
// CliOptions
// -------------------------------------------------------------------------------------------------
struct CliOptions
{
    struct Option
    {
    public:
        Option(const std::string& name, const std::string& value)
            : name(name), value(value) {}

        const std::string& GetName() const { return name; }
        const bool         HasValue() const { return !value.empty(); }

        // Get the option value after converting it into the desired integral,
        // floating-point, or boolean type. If the value fails to be converted,
        // return the specified default value.
        template <typename T>
        T GetValueOrDefault(const T& defaultValue) const
        {
            static_assert(std::is_integral_v<T> || std::is_floating_point_v<T> || std::is_same_v<T, std::string>, "GetValueOrDefault must be called with an integral, floating-point, boolean, or std::string type");
            if constexpr (std::is_same_v<T, std::string>) {
                return value;
            }
            else if constexpr (std::is_same_v<T, bool>) {
                return GetValueAsBool(defaultValue);
            }

            std::stringstream ss{value};
            T                 val;
            ss >> val;
            if (ss.fail()) {
                return defaultValue;
            }
            return val;
        }

    private:
        // For boolean options, accept "true" and "false" as well as numeric values.
        bool GetValueAsBool(bool defaultValue) const
        {
            std::stringstream ss{value};
            bool              val;
            ss >> val;
            if (ss.fail()) {
                ss.clear();
                ss >> std::boolalpha >> val;
                if (ss.fail()) {
                    return defaultValue;
                }
            }
            return val;
        }

        std::string name;
        std::string value;
    };

public:
    const StandardOptions& GetStandardOptions() const
    {
        return standardOptions;
    }

    bool HasExtraOption(const std::string& option) const
    {
        return extraOptions.find(option) != extraOptions.end();
    }

    size_t GetNumExtraOptions() const { return extraOptions.size(); }

    // Get the option value after converting it into the desired integral,
    // floating-point, or boolean type. If the option does not exist or the
    // value fails to be converted, return the specified default value.
    template <typename T>
    T GetExtraOptionValueOrDefault(const std::string& option, const T& defaultValue) const
    {
        if (!HasExtraOption(option)) {
            return defaultValue;
        }
        return extraOptions.at(option).GetValueOrDefault<T>(defaultValue);
    }

private:
    void AddExtraOption(const Option& opt)
    {
        extraOptions.insert(std::make_pair(opt.GetName(), opt));
    }

    std::unordered_map<std::string, Option> extraOptions;
    StandardOptions                         standardOptions;

    friend class CommandLineParser;
};

// -------------------------------------------------------------------------------------------------
// CommandLineParser
// -------------------------------------------------------------------------------------------------
class CommandLineParser
{
public:
    struct ParsingError
    {
        ParsingError(const std::string& error)
            : errorMsg(error) {}
        std::string errorMsg;
    };

    // Parse the given arguments into options. Return false if parsing
    // succeeded. Otherwise, return true if an error occurred,
    // and write the error to `out_error`.
    std::optional<ParsingError> Parse(int argc, const char* argv[]);
    const CliOptions&           GetOptions() const { return mOpts; }
    std::string                 GetUsageMsg() const { return mUsageMsg; }

private:
    CliOptions        mOpts;
    const std::string mUsageMsg = R"(
--help                        Prints this help message and exits.
--list-gpus                   Prints a list of the available GPUs on the current system with their index and exits (see --gpu).
--gpu <index>                 Select the gpu with the given index. To determine the set of valid indices use --list-gpus.
--resolution <Width>x<Height> Specify the main window resolution in pixels. Width and Height must be two positive integers greater or equal to 1.
--frame-count <N>             Shutdown the application after successfully rendering N frames.
--use-software-renderer       Use a software renderer instead of a hardware device, if available.
)";
};
} // namespace ppx

#endif // ppx_command_line_parser_h
