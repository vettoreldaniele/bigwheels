#ifndef ppx_command_line_parser_h
#define ppx_command_line_parser_h

#include <set>
#include <string>
#include <sstream> 
#include <map>

namespace ppx {
// -------------------------------------------------------------------------------------------------
// StandarOptions
// -------------------------------------------------------------------------------------------------
struct StandarOptions
{
    // Flags
    bool help;
    bool list_gpus;
    // Options
    int                 gpu_index;
    std::pair<int, int> resolution;
    int                 frame_count;

    StandarOptions();
    void setDefault();
};
// -------------------------------------------------------------------------------------------------
// CommandLineParser
// -------------------------------------------------------------------------------------------------
class CommandLineParser
{
public:
    CommandLineParser();
    CommandLineParser(int argc, char* argv[]);
    void                               parse(int argc, char* argv[]);
    StandarOptions                     getOptions() const;
    std::map<std::string, std::string> getExtraOptions() const;
    std::set<std::string>              getExtraFlags() const;
    std::string                        getErrorMsgs() const;
    std::string                        getUsageMsg() const;
    bool                               isOK() const;

private:
    StandarOptions                     mOpts;
    std::map<std::string, std::string> mExtraOptions;
    std::set<std::string>              mExtraFlags;
    bool                               mStateOk;
    std::stringstream                  mErorrMsgs;
    const std::string                  mUsageMsg = R"(
--help                        Prints this help and exits
--list-gpus                   Prints a list of the available GPUs on the current system with their id  and exits (See --gpu).
--gpu <index>                 Select the gpu with the given index. To determine valid index use --list-gpus
--resolution <Width>x<Height> Specify the main window resolution in pixels. Width and Height must be two positive integers
--frame-count <N>             The app exits after sucessfully render N frames
)";
    bool                               extractGpuIndex(const std::string& str);
    bool                               extractResolution(const std::string& str);
    bool                               extractFrameCount(const std::string& str);
    // string related methods
    int         parseInt(const std::string& str);
    bool        matchesOption(const std::string& str, const std::string& pattern) const;
    bool        optionOrFlag(const std::string& s) const;
    void        ltrim(std::string& s) const;
    void        rtrim(std::string& s) const;
    void        trim(std::string& s) const;
    std::string ltrim_copy(std::string s) const;
    std::string rtrim_copy(std::string s) const;
    std::string trim_copy(std::string s) const;
};
} // namespace ppx

#endif // ppx_command_line_parser_h
