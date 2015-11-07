#include <boost/date_time.hpp>
#include <iostream>
#include <iomanip>	

class Logger
{
private:
public:

	enum Level {
		Verbose,
		Info,
		Warning,
		Error,
		Debug
	};

	static void Init() {
	}

	static void Log(Logger::Level level, std::string data) {
		std::cout << std::left << std::setw(20) << ("[" + boost::posix_time::to_iso_string(boost::posix_time::second_clock::local_time()) + "]");
		std::cout << std::left << std::setw(8);
		switch (level) {
			case Verbose: {
				std::cout << "Verbose";
				break;
			}
			case Info: {
				std::cout << "Info";
				break;
			}
			case Warning: {
				std::cout << "Warning";
				break;
			}
			case Error: {
				std::cout << "Error";
				break;
			}
			case Debug: {
				std::cout << "Debug";
				break;
			}
		}
		std::cout << std::left << std::setw(40) << data;
		std::cout << std::endl;
	}
};