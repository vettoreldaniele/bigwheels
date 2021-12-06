#ifndef ppx_command_line_parser_h
#define ppx_command_line_parser_h

#include <set>
#include <string>
#include <sstream> 
#include <map>

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
// CommandLineParser
// -------------------------------------------------------------------------------------------------
class CommandLineParser
{
public:
    CommandLineParser();
    CommandLineParser(int argc, char* argv[]);
    void                               Parse(int argc, char* argv[]);
    StandardOptions                    GetOptions() const;
    std::map<std::string, std::string> GetExtraOptions() const;
    std::set<std::string>              GetExtraFlags() const;
    std::string                        GetErrorMsgs() const;
    std::string                        GetUsageMsg() const;
    bool                               IsOK() const;

private:
    StandardOptions                    mOpts;
    std::map<std::string, std::string> mExtraOptions;
    std::set<std::string>              mExtraFlags;
    bool                               mStateOk;
    std::stringstream                  mErorrMsgs;
    const std::string                  mUsageMsg = R"(
--help                        Prints this help and exits
--list-gpus                   Prints a list of the available GPUs on the current system with their id  and exits (See --gpu).
--gpu <index>                 Select the gpu with the given index. To determine valid index use --list-gpus
--resolution <Width>x<Height> Specify the main window resolution in pixels. Width and Height must be two positive integers
--frame-count <N>             The app exits after sucessfully rendering N frames
)";
    bool                               ExtractGpuIndex(const std::string& str);
    bool                               ExtractResolution(const std::string& str);
    bool                               ExtractFrameCount(const std::string& str);
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
