#ifndef ppx_command_line_parser_h
#define ppx_command_line_parser_h

#include <ios>
#include <set>
#include <string>
#include <sstream>
#include <unordered_map>
#include <type_traits>

namespace ppx {
// -------------------------------------------------------------------------------------------------
// StandardOptions
// -------------------------------------------------------------------------------------------------
struct StandardOptions
{
    // Flags
    bool help;
    bool list_gpus;
    // Options
    int                 gpu_index;
    std::pair<int, int> resolution;
    int                 frame_count;

    StandardOptions();
    void SetDefault();
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
    void AddOption(const std::string& name, const std::string& value)
    {
        options.insert(std::make_pair(name, Option(name, value)));
    }

    bool HasOption(const std::string& option) const
    {
        return options.find(option) != options.end();
    }

    // Get the option value after converting it into the desired integral,
    // floating-point, or boolean type. If the option does not exist or the
    // value fails to be converted, return the specified default value.
    template <typename T>
    T GetOptionValueOrDefault(const std::string& option, const T& defaultValue) const
    {
        if (!HasOption(option)) {
            return defaultValue;
        }
        return options.at(option).GetValueOrDefault<T>(defaultValue);
    }

private:
    std::unordered_map<std::string, Option> options;
};

// -------------------------------------------------------------------------------------------------
// CommandLineParser
// -------------------------------------------------------------------------------------------------
class CommandLineParser
{
public:
    CommandLineParser();
    CommandLineParser(int argc, char* argv[]);
    void              Parse(int argc, char* argv[]);
    StandardOptions   GetOptions() const;
    const CliOptions& GetExtraOptions() const;
    std::string       GetErrorMsgs() const;
    std::string       GetUsageMsg() const;
    bool              IsOK() const;

private:
    StandardOptions   mOpts;
    CliOptions        mExtraOptions;
    bool              mStateOk;
    std::stringstream mErorrMsgs;
    const std::string mUsageMsg = R"(
--help                        Prints this help and exits
--list-gpus                   Prints a list of the available GPUs on the current system with their id  and exits (See --gpu).
--gpu <index>                 Select the gpu with the given index. To determine valid index use --list-gpus
--resolution <Width>x<Height> Specify the main window resolution in pixels. Width and Height must be two positive integers
--frame-count <N>             The app exits after sucessfully rendering N frames
)";
    bool              ExtractGpuIndex(const std::string& str);
    bool              ExtractResolution(const std::string& str);
    bool              ExtractFrameCount(const std::string& str);
    // string related methods
    int         ParseInt(const std::string& str);
    bool        MatchesOption(const std::string& str, const std::string& pattern) const;
    bool        OptionOrFlag(const std::string& s) const;
    void        LTrim(std::string& s) const;
    void        RTrim(std::string& s) const;
    void        Trim(std::string& s) const;
    std::string TrimCopy(std::string s) const;
};
} // namespace ppx

#endif // ppx_command_line_parser_h
