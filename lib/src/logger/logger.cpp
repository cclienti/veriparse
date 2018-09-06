#include <veriparse/logger/logger.hpp>

#include <boost/log/core/core.hpp>
#include <boost/log/expressions/formatters/date_time.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/make_shared.hpp>

#include <fstream>
#include <cstddef>


namespace Veriparse {
	namespace Logger {
		std::string get_class_name(const std::string &funcname)
		{
			size_t paren = funcname.rfind("(");
			std::string reduced1 = funcname.substr(0, paren);

			size_t semi1 = reduced1.rfind("::");
			std::string reduced2 = reduced1.substr(0, semi1);

			size_t semi2 = reduced2.rfind("::");
			std::string reduced3 = reduced2.substr(semi2+2, reduced2.size()-1);

			size_t semi3 = reduced3.rfind(" ");
			if (semi3 != std::string::npos) {
				std::string reduced4 = reduced3.substr(semi3+1, reduced3.size()-1);
				return reduced4;
			}

			return reduced3;
		}

		BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
		BOOST_LOG_ATTRIBUTE_KEYWORD(severity, "Severity", boost::log::trivial::severity_level)

		// specify the format of the log message
		static boost::log::formatter & logger_get_formatter() {
			static boost::log::formatter formatter = boost::log::expressions::stream
				<< boost::log::expressions::format_date_time(timestamp, "[%Y-%m-%d, %H:%M:%S.%f]") << " "
				<< "[" << boost::log::trivial::severity << "] "
				<< boost::log::expressions::smessage;
			return formatter;
		}

		BOOST_LOG_GLOBAL_LOGGER_INIT(logger, boost::log::sources::severity_logger_mt) {
			boost::log::sources::severity_logger_mt<boost::log::trivial::severity_level> logger;

			// add attributes
			logger.add_attribute("TimeStamp", boost::log::attributes::local_clock()); // each log line gets a timestamp

			return logger;
		}

		// add stdout sink
		void add_stdout_sink(void) {
			typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> ostream_sink_t;
			boost::shared_ptr<ostream_sink_t> ostream_sink = boost::make_shared<ostream_sink_t>();
			ostream_sink->locked_backend()->add_stream(boost::shared_ptr<std::ostream>
			                                           (&std::clog, boost::null_deleter()));

			ostream_sink->set_formatter(logger_get_formatter());

#ifdef FULL_DEBUG
			// log all messages
			ostream_sink->set_filter(severity >= boost::log::trivial::trace);
#else
#ifdef DEBUG
			ostream_sink->set_filter(severity >= boost::log::trivial::debug);
#else
			// log all messages except debug and trace
			ostream_sink->set_filter(severity >= boost::log::trivial::info);
#endif
#endif

			// Enable auto-flushing after each log record written
			ostream_sink->locked_backend()->auto_flush(true);

			// register our sinks
			boost::log::core::get()->add_sink(ostream_sink);
		}


		// add stream sink
		void add_stream_sink(std::shared_ptr<std::ostringstream> oss) {
			typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_ostream_backend> ostream_sink_t;
			boost::shared_ptr<ostream_sink_t> ostream_sink = boost::make_shared<ostream_sink_t>();

			boost::shared_ptr<std::ostringstream> boss =
				boost::shared_ptr<std::ostringstream>(oss.get(), [oss](std::ostringstream*) mutable {oss.reset();});

			ostream_sink->locked_backend()->add_stream(boss);
			ostream_sink->set_formatter(logger_get_formatter());

#ifdef FULL_DEBUG
			// log all messages
			ostream_sink->set_filter(severity >= boost::log::trivial::trace);
#else
#ifdef DEBUG
			ostream_sink->set_filter(severity >= boost::log::trivial::debug);
#else
			// log all messages except debug and trace
			ostream_sink->set_filter(severity >= boost::log::trivial::info);
#endif
#endif

			// Enable auto-flushing after each log record written
			ostream_sink->locked_backend()->auto_flush(true);

			// register our sinks
			boost::log::core::get()->add_sink(ostream_sink);
		}


		// add a text file sink
		void add_text_sink(std::string const &filename) {
			typedef boost::log::sinks::synchronous_sink<boost::log::sinks::text_file_backend> file_sink_t;
			boost::shared_ptr<boost::log::sinks::text_file_backend> file_backend =
				boost::make_shared<boost::log::sinks::text_file_backend>
				(boost::log::keywords::file_name = filename.c_str(),
				 boost::log::keywords::rotation_size = 10 * 1024 * 1024);
			boost::shared_ptr<file_sink_t> file_sink = boost::make_shared<file_sink_t>(file_backend);

			file_sink->set_formatter(logger_get_formatter());

#ifdef FULL_DEBUG
			// log all messages
			file_sink->set_filter(severity >= boost::log::trivial::trace);
#else
#ifdef DEBUG
			file_sink->set_filter(severity >= boost::log::trivial::debug);
#else
			// log all messages except debug and trace
			file_sink->set_filter(severity >= boost::log::trivial::info);
#endif
#endif

			// Enable auto-flushing after each log record written
			file_backend->auto_flush(true);

			boost::log::core::get()->add_sink(file_sink);
		}

		void flush(void) {
			boost::log::core::get()->flush();
		}

		void remove_all_sinks(void) {
			flush();
			boost::log::core::get()->remove_all_sinks();
		}


	}
}
