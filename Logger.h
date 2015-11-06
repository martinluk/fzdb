#include <iostream>
#include <iomanip>	

class Logger
{
private:
public:

	enum Level {
		Info,
		Warning,
		Error,
		Debug
	};

	static void Init() {
	}

	static void Log(Logger::Level level, std::string data) {
		auto t = std::time(nullptr);
		auto tm = *std::localtime(&t);
		std::cout << std::left << std::setw(50) << "Timestamp"
			<< std::left << std::setw(50) << data
			<< std::endl;
	}
};
