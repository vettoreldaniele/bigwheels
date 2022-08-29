#include <gtest/gtest.h>
#include "gmock/gmock.h"

#include "ppx/command_line_parser.h"

namespace ppx {
namespace {

using ::testing::HasSubstr;

TEST(CommandLineParserTest, ZeroArguments)
{
    CommandLineParser parser;
    StandardOptions   defaultOptions;
    EXPECT_FALSE(parser.Parse(0, nullptr));
    EXPECT_EQ(parser.GetOptions().GetStandardOptions(), defaultOptions);
    EXPECT_EQ(parser.GetOptions().GetNumExtraOptions(), 0);
}

TEST(CommandLineParserTest, FirstArgumentIgnored)
{
    CommandLineParser parser;
    StandardOptions   defaultOptions;
    const char*       args[] = {"/path/to/executable"};
    EXPECT_FALSE(parser.Parse(1, args));
    EXPECT_EQ(parser.GetOptions().GetStandardOptions(), defaultOptions);
    EXPECT_EQ(parser.GetOptions().GetNumExtraOptions(), 0);
}

TEST(CommandLineParserTest, StandardOptionsSuccessfullyParsed)
{
    CommandLineParser parser;
    const char*       args[] = {"/path/to/executable", "--help", "--list-gpus", "--gpu", "5", "--resolution", "1920x1080", "--frame-count", "11", "--use-software-renderer", "--screenshot-frame-number", "321", "--screenshot-out-dir", "/path/to/screenshot/dir"};
    EXPECT_FALSE(parser.Parse(14, args));

    StandardOptions wantOptions;
    wantOptions.help                  = true;
    wantOptions.list_gpus             = true;
    wantOptions.gpu_index             = 5;
    wantOptions.resolution            = {1920, 1080};
    wantOptions.frame_count           = 11;
    wantOptions.use_software_renderer = true;
    wantOptions.screenshot_frame_number = 321;
    wantOptions.screenshot_out_dir      = "/path/to/screenshot/dir";

    EXPECT_EQ(parser.GetOptions().GetStandardOptions(), wantOptions);
    EXPECT_EQ(parser.GetOptions().GetNumExtraOptions(), 0);
}

TEST(CommandLineParserTest, ExtraOptionsSuccessfullyParsed)
{
    CommandLineParser parser;
    StandardOptions   defaultOptions;
    const char*       args[] = {"/path/to/executable", "--extra-option-bool", "true", "--extra-option-int", "123", "--extra-option-no-param", "--extra-option-str", "option string value"};
    EXPECT_FALSE(parser.Parse(8, args));

    auto opts = parser.GetOptions();
    EXPECT_EQ(opts.GetStandardOptions(), defaultOptions);
    EXPECT_EQ(opts.GetNumExtraOptions(), 4);
    EXPECT_EQ(opts.GetExtraOptionValueOrDefault("extra-option-bool", false), true);
    EXPECT_EQ(opts.GetExtraOptionValueOrDefault("extra-option-int", 0), 123);
    EXPECT_EQ(opts.GetExtraOptionValueOrDefault<std::string>("extra-option-str", ""), "option string value");
    EXPECT_EQ(opts.GetExtraOptionValueOrDefault<std::string>("extra-option-no-param", ""), "");
    EXPECT_TRUE(opts.HasExtraOption("extra-option-no-param"));
}

TEST(CommandLineParserTest, StandardOptionsParsingErrorMissingParameter)
{
    CommandLineParser parser;
    const char*       args[] = {"/path/to/executable", "--gpu"};
    auto              error  = parser.Parse(2, args);
    EXPECT_TRUE(error.has_value());

    EXPECT_THAT(error->errorMsg, HasSubstr("requires a parameter"));
}

TEST(CommandLineParserTest, StandardOptionsParsingErrorInvalidParameterNegativeInteger)
{
    CommandLineParser parser;
    const char*       args[] = {"/path/to/executable", "--gpu", "-5"};
    auto              error  = parser.Parse(3, args);
    EXPECT_TRUE(error.has_value());

    EXPECT_THAT(error->errorMsg, HasSubstr("requires a positive integer"));
}

TEST(CommandLineParserTest, StandardOptionsParsingErrorInvalidParameterResolution)
{
    CommandLineParser parser;
    const char*       args[] = {"/path/to/executable", "--resolution", "1920-1080"};
    auto              error  = parser.Parse(3, args);
    EXPECT_TRUE(error.has_value());

    EXPECT_THAT(error->errorMsg, HasSubstr("must be in <Width>x<Height> format"));
}

} // namespace
} // namespace ppx
