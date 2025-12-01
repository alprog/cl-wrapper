export module cmd_tool;

import std;
import child_process;

export class CmdTool
{
public:
	CmdTool(const std::string& fullPath)
		: fullPath{ fullPath }
		, exitCode{ -1 }
	{
	}

	int run(const std::string& args)
	{
		reset();

		auto cmdLine = fullPath + " " + args;
		ChildProcess process(cmdLine);
		outputStream = process.readPipe();
		exitCode = process.waitExit();
		return exitCode;
	}

private:
	void reset()
	{
		exitCode = -1;
		outputStream.clear();
		errorStream.clear();
	}

public:
	std::string fullPath;
	int exitCode;
	std::string outputStream;
	std::string errorStream;
};