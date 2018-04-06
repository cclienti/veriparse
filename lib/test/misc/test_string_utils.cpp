#include <gtest/gtest.h>
#include <veriparse/misc/string_utils.hpp>
#include <veriparse/logger/logger.hpp>
#include <vector>
#include <list>

using namespace Veriparse;


TEST(MiscTest, StringUtils_escape) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_escape.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::escape(""), "");
	ASSERT_EQ(Misc::StringUtils::escape("HelloWorld"), "HelloWorld");
	ASSERT_EQ(Misc::StringUtils::escape("\\HelloWorld"), "\\HelloWorld ");
	ASSERT_EQ(Misc::StringUtils::escape("\\HelloWorld "), "\\HelloWorld ");
}

TEST(MiscTest, StringUtils_join) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_join.log");
	Logger::add_stdout_sink();

	std::vector<std::string> vector0;
	std::vector<std::string> vector1 {"helloworld"};
	std::vector<std::string> vector2 {"hello", "world"};

	ASSERT_EQ(Misc::StringUtils::join<std::vector<std::string> >(", ", vector0), "");
	ASSERT_EQ(Misc::StringUtils::join<std::vector<std::string> >(", ", vector1), "helloworld");
	ASSERT_EQ(Misc::StringUtils::join<std::vector<std::string> >(", ", vector2), "hello, world");
}

TEST(MiscTest, StringUtils_delete_surrounding_brackets) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_delete_surrounding_brackets.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::delete_surrounding_brackets(""), "");
	ASSERT_EQ(Misc::StringUtils::delete_surrounding_brackets("(3*2)"), "3*2");
	ASSERT_EQ(Misc::StringUtils::delete_surrounding_brackets("(3*(2+1))"), "3*(2+1)");
	ASSERT_EQ(Misc::StringUtils::delete_surrounding_brackets("3*(2+1)"), "3*(2+1)");

	// The following tests are not properly managed by the function,
	// but these cases should not appear in real life.
	ASSERT_EQ(Misc::StringUtils::delete_surrounding_brackets("(3*2)+1)"), "3*2)+1");
	ASSERT_EQ(Misc::StringUtils::delete_surrounding_brackets("(3*(2+1)"), "3*(2+1");
}

TEST(MiscTest, StringUtils_remove_all_substring) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_all_substring.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_all_sub_string("", ""), "");
	ASSERT_EQ(Misc::StringUtils::remove_all_sub_string("", " "), "");
	ASSERT_EQ(Misc::StringUtils::remove_all_sub_string(" ", ""), " ");
	ASSERT_EQ(Misc::StringUtils::remove_all_sub_string("Hello World", " World"), "Hello");
	ASSERT_EQ(Misc::StringUtils::remove_all_sub_string("Hello World", "Hello "), "World");
	ASSERT_EQ(Misc::StringUtils::remove_all_sub_string("Hello TTTTT World", "T"), "Hello  World");
}

TEST(MiscTest, StringUtils_replace_all) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_replace_all.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::replace_all("My Hello My World My ", "My ", ""), "Hello World ");
	ASSERT_EQ(Misc::StringUtils::replace_all("My Hello My World My", "My", "Your"), "Your Hello Your World Your");
}

TEST(MiscTest, StringUtils_remove_spaces) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_spaces.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_spaces(""), "");
	ASSERT_EQ(Misc::StringUtils::remove_spaces(" Hello World ! "), "HelloWorld!");
	ASSERT_EQ(Misc::StringUtils::remove_spaces("Hello\tWorld!\n"), "Hello\tWorld!\n");
}

TEST(MiscTest, StringUtils_remove_underscore) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_underscore.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_underscore(""), "");
	ASSERT_EQ(Misc::StringUtils::remove_underscore("_Hello_World_!_"), "HelloWorld!");
	ASSERT_EQ(Misc::StringUtils::remove_underscore("Hello\tWorld!\n"), "Hello\tWorld!\n");
}

TEST(MiscTest, StringUtils_remove_whitespace) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_whitespace.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_whitespace(""), "");
	ASSERT_EQ(Misc::StringUtils::remove_whitespace(" Hello\t World ! "), "HelloWorld!");
	ASSERT_EQ(Misc::StringUtils::remove_whitespace("HelloWorld!"), "HelloWorld!");
}

TEST(MiscTest, StringUtils_remove_leading_whitespace) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_leading_whitespace.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_leading_whitespace(""), "");
	ASSERT_EQ(Misc::StringUtils::remove_leading_whitespace("\n Hello\t World ! "), "Hello\t World ! ");
}

TEST(MiscTest, StringUtils_remove_trailing_whitespace) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_trailing_whitespace.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_trailing_whitespace(""), "");
	ASSERT_EQ(Misc::StringUtils::remove_trailing_whitespace("\n Hello\t World ! \n"), "\n Hello\t World !");
	ASSERT_EQ(Misc::StringUtils::remove_trailing_whitespace("\n Hello\n World ! \n"), "\n Hello\n World !");
}

TEST(MiscTest, StringUtils_remove_last_semicolon) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_remove_last_semicolon.log");
	Logger::add_stdout_sink();

	ASSERT_EQ(Misc::StringUtils::remove_last_semicolon(""), "");
	ASSERT_EQ(Misc::StringUtils::remove_last_semicolon("\n Hello\t Wooorld ! ;; \n"), "\n Hello\t Wooorld ! ;");
}

TEST(MiscTest, StringUtils_split) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_split.log");
	Logger::add_stdout_sink();

	std::string to_split("Hello\n\nWorld\n!\n\n");
	std::vector<std::string> ref = {"Hello", "", "World", "!", ""};

	ASSERT_EQ(Misc::StringUtils::split(to_split, '\n'), ref);
}

TEST(MiscTest, StringUtils_indent) {
	Logger::remove_all_sinks();
	Logger::add_text_sink("MiscTest.StringUtils_indent.log");
	Logger::add_stdout_sink();

	std::string to_indent(" Hello\n \n World\n !\n\t\n");
	std::string ref ("     Hello\n\n     World\n     !\n");

	ASSERT_EQ(Misc::StringUtils::indent("", ""), "");
	ASSERT_EQ(Misc::StringUtils::indent(to_indent, "    "), ref);
}
