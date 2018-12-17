#ifndef VERIPARSE_LIB_TEST_HELPERS_HPP
#define VERIPARSE_LIB_TEST_HELPERS_HPP

#include <veriparse/generators/yaml_generator.hpp>
#include <veriparse/generators/dot_generator.hpp>
#include <veriparse/generators/verilog_generator.hpp>
#include <veriparse/AST/node.hpp>
#include <veriparse/logger/logger.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <cstdlib>


#define ENABLE_LOGGER	  \
	const ::testing::TestInfo* const test_info = ::testing::UnitTest::GetInstance()->current_test_info(); \
	std::string test_name = test_info->name(); \
	std::string test_string = test_info->test_case_name() + std::string(".") + test_name; \
	  \
	Logger::remove_all_sinks(); \
	Logger::add_text_sink(test_string + ".log"); \
	Logger::add_stdout_sink();


namespace Veriparse {

class TestHelpers {
public:
	TestHelpers(const std::string &testcases_path):
		m_testcases_path (testcases_path)
	{
		const char *veriparse_source_root(std::getenv("VERIPARSE_SOURCE_ROOT"));
		if (veriparse_source_root != nullptr)
		{
			m_testcases_path = boost::filesystem::path(veriparse_source_root) / testcases_path;
		}
		LOG_INFO << "testcases path: " << m_testcases_path;
	}

	std::string get_verilog_filename(const std::string &teststr)
	{
		auto filename = teststr + ".v";
		auto filename_path = m_testcases_path / boost::filesystem::path(filename);
		return filename_path.string();
	}

	std::string get_yaml_filename(const std::string &teststr)
	{
		auto filename = teststr + ".yaml";
		auto filename_path = m_testcases_path / boost::filesystem::path(filename);
		LOG_INFO << filename_path;
		return filename_path.string();
	}

	void render_node_to_yaml_file(AST::Node::Ptr node, const std::string &filename)
	{
		YAML::Node yaml = Generators::YAMLGenerator().render(node);
		YAML::Emitter emitter;
		emitter << YAML::Indent(2) << yaml;
		std::ofstream fout(filename);
		fout << emitter.c_str() << std::endl;
	}

	void render_node_to_dot_file(AST::Node::Ptr node, const std::string &filename)
	{
		std::string dot = Generators::DotGenerator().render(node);
		Generators::DotGenerator::save_dot(dot, filename);
	}

	template<typename TNodeListPtr>
	void render_node_list_to_dot_files(TNodeListPtr nodes,
	                                   const std::string &basename)
	{
		int i=0;
		for(AST::Node::Ptr n: *nodes) {
			std::stringstream ss;
			ss << basename << i++ << ".dot";
			render_node_to_dot_file(AST::to_node(n), ss.str());
		}
	}

	void render_node_to_verilog_file(AST::Node::Ptr node, const std::string &filename)
	{
		std::string str = Generators::VerilogGenerator().render(node);
		std::ofstream fout(filename);
		fout << str << std::endl;
	}

private:
	boost::filesystem::path m_testcases_path;
};

}


#endif
