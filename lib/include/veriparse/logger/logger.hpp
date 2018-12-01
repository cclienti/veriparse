#ifndef VERIPARSE_LOGGER_LOGGER_HPP
#define VERIPARSE_LOGGER_LOGGER_HPP


#include <boost/log/trivial.hpp>
#include <boost/log/sources/global_logger_storage.hpp>
#include <memory>
#include <sstream>
#include <ostream>


#ifdef __GNUC__
#define VERIPARSE_FUNCTION_NAME __PRETTY_FUNCTION__
#else
#define VERIPARSE_FUNCTION_NAME __FUNCSIG__
#endif

// Just a helper macro used by the macros below - don't use it in your code
#define VERIPARSE_LOG(severity) BOOST_LOG_SEV(Veriparse::Logger::logger::get(), boost::log::trivial::severity) \
	<< "[" << Veriparse::Logger::get_class_name(VERIPARSE_FUNCTION_NAME) << "] "

// Log Macros
#define LOG_TRACE            if (Veriparse::Logger::trace_enabled()) VERIPARSE_LOG(trace)
#define LOG_TRACE_F(_1)      LOG_TRACE << "file " << _1 << ", "
#define LOG_TRACE_FL(_1, _2) LOG_TRACE << "file " << _1 << ':' << _2 << ", "
#define LOG_TRACE_N(_1)      if(_1) LOG_TRACE_FL(_1->get_filename(), _1->get_line())

#define LOG_DEBUG            if (Veriparse::Logger::debug_enabled()) VERIPARSE_LOG(debug)
#define LOG_DEBUG_F(_1)      LOG_DEBUG << "file " << _1 << ", "
#define LOG_DEBUG_FL(_1, _2) LOG_DEBUG << "file " << _1 << ':' << _2 << ", "
#define LOG_DEBUG_N(_1)      if(_1) LOG_DEBUG_FL(_1->get_filename(), _1->get_line())

#define LOG_INFO            if (Veriparse::Logger::info_enabled()) VERIPARSE_LOG(info)
#define LOG_INFO_F(_1)      LOG_INFO << "file " << _1 << ", "
#define LOG_INFO_FL(_1, _2) LOG_INFO << "file " << _1 << ':' << _2 << ", "
#define LOG_INFO_N(_1)      if(_1) LOG_INFO_FL(_1->get_filename(), _1->get_line())

#define LOG_WARNING            if (Veriparse::Logger::warning_enabled()) VERIPARSE_LOG(warning)
#define LOG_WARNING_F(_1)      LOG_WARNING << "file " << _1 << ", "
#define LOG_WARNING_FL(_1, _2) LOG_WARNING << "file " << _1 << ':' << _2 << ", "
#define LOG_WARNING_N(_1)      if(_1) LOG_WARNING_FL(_1->get_filename(), _1->get_line())

#define LOG_ERROR             VERIPARSE_LOG(error)
#define LOG_ERROR_F(_1)       LOG_ERROR << "file " << _1 << ", "
#define LOG_ERROR_FL(_1, _2)  LOG_ERROR << "file " << _1 << ':' << _2 << ", "
#define LOG_ERROR_N(_1)       if(_1) LOG_ERROR_FL(_1->get_filename(), _1->get_line())

#define LOG_FATAL            VERIPARSE_LOG(fatal)
#define LOG_FATAL_F(_1)      LOG_FATAL << "file " << _1 << ", "
#define LOG_FATAL_FL(_1, _2) LOG_FATAL << "file " << _1 << ':' << _2 << ", "
#define LOG_FATAL_N(_1)      if(_1) LOG_FATAL_FL(_1->get_filename(), _1->get_line())



namespace Veriparse {
namespace Logger {

// Forward-declares a global logger with a custom initialization
BOOST_LOG_GLOBAL_LOGGER(logger, boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level>)

void add_stdout_sink(void);
void add_stream_sink(std::shared_ptr<std::ostringstream> oss);
void add_text_sink(std::string const &filename);
void flush(void);
void remove_all_sinks(void);

bool warning_enabled(void);
bool info_enabled(void);
bool debug_enabled(void);
bool trace_enabled(void);

std::string get_class_name(const std::string &funcname);

}
}


#endif
