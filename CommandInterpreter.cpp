#define BOOST_SPIRIT_UNICODE

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

#include <boost/fusion/adapted/struct/adapt_struct.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "./CommandInterpreter.h"

#include "Job.h"
#include "JobFactory.h"
#include "JobQueue.h"

struct command_struct
{
	std::string command;
	std::string arg1;
};

BOOST_FUSION_ADAPT_STRUCT(
	command_struct,
	(std::string, command)
	(std::string, arg1)
	)

namespace client
{
	namespace qi = boost::spirit::qi;

	template <typename Iterator>
	struct command_parser : qi::grammar<Iterator, command_struct(), boost::spirit::unicode::space_type>
	{
		command_parser() : command_parser::base_type(start)
		{
			using qi::int_;
			using qi::lit;
			using qi::double_;
			using qi::lexeme;
			using boost::spirit::unicode::char_;

			string %= lexeme[+(char_ - ' ') | ('"' >> +(char_ - '"') >> '"')];

			start %=
				string
				>> -string
				;
		}

		qi::rule<Iterator, std::string(), boost::spirit::unicode::space_type> string;
		qi::rule<Iterator, command_struct(), boost::spirit::unicode::space_type> start;
	};
}

void CommandInterpreter::ProcessCommand(TCPSession* session, std::string command) {

	client::command_parser<std::string::iterator> g;
	command_struct cmd;
	bool r = boost::spirit::qi::phrase_parse(command.begin(), command.end(), g, boost::spirit::unicode::space, cmd);
	Job* job;
	if (r) {
		if (cmd.arg1.length() == 0) {
			job = JobFactory::createJob(session, cmd.command);
		}
		else {
			job = JobFactory::createJob(session, cmd.command, cmd.arg1);
		}
	}
	else {
		job = JobFactory::createUnknownJob(session, cmd.command);
	}
  //simple commands
   
  JobQueue::AddJob(job);
}