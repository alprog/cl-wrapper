export module arguments;

import std;
import utils;
import rsp;

export class Arguments
{
public:
	Arguments(int count, char** argv)
	{
		for (int i = 1; i < count; i++)
		{
			args.push_back(argv[i]);
		}
	}

	Arguments(std::string commandLine)
	{
		args = split(commandLine, " ");
	}

	int count() const
	{
		return static_cast<int>(args.size());
	}

	std::string& operator[](int index)
	{
		return args[index];
	}

	void expandRsp()
	{
		for (int i = args.size() - 1; i >= 0; --i)
		{
			auto& arg = args[i];
			if (rsp::IsMacroArg(arg))
			{
				std::string commandLine = rsp::Expand(arg);
				auto expandedArgs = split(commandLine, " ");
				
				args.erase(args.begin() + i);
				args.insert(
					args.begin() + i,
					expandedArgs.begin(),
					expandedArgs.end()
				);
			}
		}
	}

	std::string toCommandLine() const
	{
		std::ostringstream os;
		for (const auto& arg : args)
		{
			if (arg.contains(' '))
			{
				os << " " << "\"" << arg << "\"";
			}
			else
			{
				os << " " << arg;
			}
		}
		return os.str();
	}

	std::vector<std::string> args;
};