#include <algorithm>
#include <iostream>
#include <locale>
#include <vector>
#include <stdexcept>
#include <cctype>

#include "ppx/command_line_parser.h"

namespace ppx {
// -------------------------------------------------------------------------------------------------
// StandarOptions
// -------------------------------------------------------------------------------------------------
StandardOptions::StandardOptions()
{
    SetDefault();
}

void StandardOptions::SetDefault()
{
    help = list_gpus = false;
    gpu_index = frame_count = resolution.first = resolution.second = -1;
}
// -------------------------------------------------------------------------------------------------
// CommandLineParser
// -------------------------------------------------------------------------------------------------
CommandLineParser::CommandLineParser()
{
    mOpts.SetDefault();
    mStateOk = true; // Assume we are good to go
}

CommandLineParser::CommandLineParser(int argc, char* argv[])
{
    mOpts.SetDefault();
    mStateOk = true; // Assume we are good to go
    Parse(argc, argv);
}

void CommandLineParser::Parse(int argc, char* argv[])
{
    // argc is always >= 1 and argv[0] it's the name of the executable
    if (argc < 2) {
        // No argument were given. This is not an error you can exit
        return;
    }

    // Store all arguments in a vector of strings
    std::vector<std::string> args(argv + 1, argv + argc);
    // clear errors
    mErorrMsgs.str("");
    // Loop to procees all the arguments
    for (size_t i = 0; i < args.size(); ++i) {
        std::string arg = TrimCopy(args[i]);
        if (OptionOrFlag(arg)) {
            if (MatchesOption(arg, "help")) {
                mOpts.help = true;
            }
            else if (MatchesOption(arg, "list-gpus")) {
                mOpts.list_gpus = true;
            }
            else if (MatchesOption(arg, "gpu")) {
                if ((i + 1) < args.size()) {
                    std::string parameter = TrimCopy(args[i + 1]);
                    if (!OptionOrFlag(parameter)) {
                        ExtractGpuIndex(parameter);
                        i++; // We just consumed the parameter for this option
                    }
                    else {
                        mErorrMsgs << "Expected parameter after --gpu option" << std::endl;
                        mStateOk = false;
                    }
                }
                else {
                    mErorrMsgs << "Expected parameter after --gpu option" << std::endl;
                    mStateOk = false;
                }
            }
            else if (MatchesOption(arg, "resolution")) {
                if ((i + 1) < args.size()) {
                    std::string parameter = TrimCopy(args[i + 1]);
                    if (!OptionOrFlag(parameter)) {
                        ExtractResolution(TrimCopy(parameter));
                        i++; // We just consumed the parameter for this option
                    }
                    else {
                        mErorrMsgs << "Expected parameter after --resolution option" << std::endl;
                        mStateOk = false;
                    }
                }
                else {
                    mErorrMsgs << "Expected parameter after --resolution option" << std::endl;
                    mStateOk = false;
                }
            }
            else if (MatchesOption(arg, "frame-count")) {
                if ((i + 1) < args.size()) {
                    std::string parameter = TrimCopy(args[i + 1]);
                    if (!OptionOrFlag(parameter)) {
                        ExtractFrameCount(parameter);
                        i++; // We just consumed the parameter for this option
                    }
                    else {
                        mErorrMsgs << "Expected parameter after --frame-count option" << std::endl;
                        mStateOk = false;
                    }
                }
                else {
                    mErorrMsgs << "Expected parameter after --frame-count option" << std::endl;
                    mStateOk = false;
                }
            }
            else if (OptionOrFlag(arg)) { // This is a non-standard option or flag
                std::string optName = arg.substr(2);
                if ((i + 1) < args.size()) {
                    std::string parameter = TrimCopy(args[i + 1]);
                    if (!OptionOrFlag(parameter)) {
                        // It is an option with its corresponding parameter
                         mExtraOptions.AddOption(optName, parameter);
                        i++; // We just consumed the parameter for this option
                    }
                    else { // It is a flag
                        mExtraOptions.AddOption(optName, "1");
                    }
                }
                else {
                    // It is also a flag
                    mExtraOptions.AddOption(optName, "1");
                }
            }
            else {
                mErorrMsgs << "Invalid argument: " << arg << std::endl;
                mStateOk = false;
            }
        }
        else {
            mErorrMsgs << "Parameter: " << arg << " does not match any option" << std::endl;
            mStateOk = false;
        }
    }
}

StandardOptions CommandLineParser::GetOptions() const
{
    return mOpts;
}

const CliOptions& CommandLineParser::GetExtraOptions() const
{
    return mExtraOptions;
}

std::string CommandLineParser::GetErrorMsgs() const
{
    return mErorrMsgs.str();
}

bool CommandLineParser::IsOK() const
{
    return mStateOk;
}

std::string CommandLineParser::GetUsageMsg() const
{
    return mUsageMsg;
}

bool CommandLineParser::ExtractGpuIndex(const std::string& str)
{
    int id = ParseInt(str);
    if (id >= 0) {
        mOpts.gpu_index = id;
        return true;
    }
    else {
        mStateOk = false;
        mErorrMsgs << "Invalid id for GPU: " << str << std::endl;
        return false;
    }
}

bool CommandLineParser::ExtractResolution(const std::string& str)
{
    int width  = -1;
    int height = -1;

    size_t found = str.find("x");
    if (found != std::string::npos) {
        width  = ParseInt(str.substr(0, found));
        height = ParseInt(str.substr(found + 1));
        if (width < 1 || height < 1) {
            mStateOk = false;
            mErorrMsgs << "Invalid resolution: " << str << std::endl;
            return false;
        }
        else {
            mOpts.resolution.first  = width;
            mOpts.resolution.second = height;
            return true;
        }
    }
    else {
        mStateOk = false;
        mErorrMsgs << "Invalid resolution: " << str << std::endl;
        return false;
    }
}

bool CommandLineParser::ExtractFrameCount(const std::string& str)
{
    int n = ParseInt(str);
    if (n > 0) {
        mOpts.frame_count = n;
        return true;
    }
    else {
        mStateOk = false;
        mErorrMsgs << "Invalid frame count parameter: " << str << std::endl;
        return false;
    }
}

int CommandLineParser::ParseInt(const std::string& str)
{
    int value = -1;
    try {
        value = std::stoi(str);
    }
    catch (std::out_of_range&) {
        mStateOk = false;
        mErorrMsgs << "Value: " << str << " is out or range for int" << std::endl;
    }
    catch (std::invalid_argument&) {
        mStateOk = false;
        mErorrMsgs << "Expected int value, got: " << str << std::endl;
    }
    return value;
}

bool CommandLineParser::MatchesOption(const std::string& str, const std::string& pattern) const
{
    if (OptionOrFlag(str)) {
        return str.substr(2) == pattern;
    }

    return false;
}

// trim from start (in place)
void CommandLineParser::LTrim(std::string& s) const
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
void CommandLineParser::RTrim(std::string& s) const
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
            s.end());
}

// trim from both ends (in place)
void CommandLineParser::Trim(std::string& s) const
{
    LTrim(s);
    RTrim(s);
}

// trim from both ends (copying)
std::string CommandLineParser::TrimCopy(std::string s) const
{
    Trim(s);
    return s;
}

bool CommandLineParser::OptionOrFlag(const std::string& s) const
{
    if (s.size() > 3) {
        return s.substr(0, 2) == "--";
    }
    return false;
}
} // namespace ppx