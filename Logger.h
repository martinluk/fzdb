#include <boost/date_time.hpp>
#include <iostream>
#include <iomanip>	

class Logger
{
public:
	enum Level {
		Verbose,
		Info,
		Warning,
		Error,
		Debug
	};

public:
	static void Init() {
	}

	static void Log(Logger::Level level, const std::string &data) {
		LogBegin(level);
		std::cout << data << std::endl;
	}

	// http://stackoverflow.com/questions/1134388/stdendl-is-of-unknown-type-when-overloading-operator
	class Helper
	{
	public:
		Helper(Logger::Level l) : level(l) {}
		const Logger::Level level;

		template <typename T>
	    Helper& operator<<(const T& x)
	    {
	        std::cout << x;

	        return *this;
	    }

	    typedef Helper& (*HelperFunc)(Helper&);

	    Helper& operator<<(HelperFunc manip)
	    {
	        return manip(*this);
	    }

	    static Helper& endl(Helper& stream)
	    {
	        std::cout << std::endl;
	        return stream;
	    }

	    typedef std::basic_ostream<char, std::char_traits<char> > CoutType;
	    typedef CoutType& (*StandardEndLine)(CoutType&);

	    Helper& operator<<(StandardEndLine manip)
	    {
	        manip(std::cout);
	        return *this;
	    }
	};

	static Helper Log(Level l)
	{
		LogBegin(l);
		return Helper(l);
	}

	// Convenience
	static Helper Log()
	{
		return Log(Info);
	}

private:

	static void LogBegin(Level level)
	{
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
		std::cout << std::left << std::setw(0);
	}
};