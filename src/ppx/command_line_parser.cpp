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
StandarOptions::StandarOptions()
{
    setDefault();
}

void StandarOptions::setDefault()
{
    help = list_gpus = false;
    gpu_index = frame_count = resolution.first = resolution.second = -1;
}
// -------------------------------------------------------------------------------------------------
// CommandLineParser
// -------------------------------------------------------------------------------------------------
CommandLineParser::CommandLineParser()
{
    mOpts.setDefault();
    mStateOk = true; //Assume we are good to go
}

CommandLineParser::CommandLineParser(int argc, char* argv[])
{
    mOpts.setDefault();
    mStateOk = true; //Assume we are good to go
    parse(argc, argv);
}

void CommandLineParser::parse(int argc, char* argv[])
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
        std::string arg = trim_copy(args[i]);
        if (optionOrFlag(arg)) {
            if (matchesOption(arg, "help")) {
                mOpts.help = true;
            }
            else if (matchesOption(arg, "list-gpus")) {
                mOpts.list_gpus = true;
            }
            else if (matchesOption(arg, "gpu")) {
                if ((i + 1) < args.size()) {
                    std::string parameter = trim_copy(args[i + 1]);
                    if (!optionOrFlag(parameter)) {
                        extractGpuIndex(parameter);
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
            else if (matchesOption(arg, "resolution")) {
                if ((i + 1) < args.size()) {
                    std::string parameter = trim_copy(args[i + 1]);
                    if (!optionOrFlag(parameter)) {
                        extractResolution(trim_copy(parameter));
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
            else if (matchesOption(arg, "frame-count")) {
                if ((i + 1) < args.size()) {
                    std::string parameter = trim_copy(args[i + 1]);
                    if (!optionOrFlag(parameter)) {
                        extractFrameCount(parameter);
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
            else if (optionOrFlag(arg)) { // This is a non-standar option or flag
                if ((i + 1) < args.size()) {
                    std::string parameter = trim_copy(args[i + 1]);
                    if (!optionOrFlag(parameter)) {
                        // It is an option with his corresponding parameter
                        mExtraOptions[arg.substr(2)] = parameter;
                        i++; // we just consume the parameter for this option
                    }
                    else { // It is a flag
                        mExtraFlags.insert(arg.substr(2));
                    }
                }
                else {
                    // It is also a flag
                    mExtraFlags.insert(arg.substr(2));
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

StandarOptions CommandLineParser::getOptions() const
{
    return mOpts;
}

std::map<std::string, std::string> CommandLineParser::getExtraOptions() const
{
    return mExtraOptions;
}

std::set<std::string> CommandLineParser::getExtraFlags() const
{
    return mExtraFlags;
}

std::string CommandLineParser::getErrorMsgs() const
{
    return mErorrMsgs.str();
}

bool CommandLineParser::isOK() const
{
    return mStateOk;
}

bool CommandLineParser::extractGpuIndex(const std::string& str)
{
    int id = parseInt(str);
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

bool CommandLineParser::extractResolution(const std::string& str)
{
    int width  = -1;
    int height = -1;

    size_t found = str.find("x");
    if (found != std::string::npos) {
        width  = parseInt(str.substr(0, found));
        height = parseInt(str.substr(found + 1));
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

bool CommandLineParser::extractFrameCount(const std::string& str)
{
    int n = parseInt(str);
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

int CommandLineParser::parseInt(const std::string& str)
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

bool CommandLineParser::matchesOption(const std::string& str, const std::string& pattern) const
{
    if (optionOrFlag(str)) {
        return str.substr(2) == pattern;
    }

    return false;
}

// trim from start (in place)
void CommandLineParser::ltrim(std::string& s) const
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
void CommandLineParser::rtrim(std::string& s) const
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
            s.end());
}

// trim from both ends (in place)
void CommandLineParser::trim(std::string& s) const
{
    ltrim(s);
    rtrim(s);
}

// trim from start (copying)
std::string CommandLineParser::ltrim_copy(std::string s) const
{
    ltrim(s);
    return s;
}

// trim from end (copying)
std::string CommandLineParser::rtrim_copy(std::string s) const
{
    rtrim(s);
    return s;
}

// trim from both ends (copying)
std::string CommandLineParser::trim_copy(std::string s) const
{
    trim(s);
    return s;
}

bool CommandLineParser::optionOrFlag(const std::string& s) const
{
    if (s.size() > 3) {
        return s.substr(0, 2) == "--";
    }
    return false;
}
} // namespace ppx