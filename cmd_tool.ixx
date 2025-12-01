export module cmd_tool;

import std;
import child_process;

export class CmdTool
{
public:
	int run(const std::string& args)
	{
		reset();

		auto cmdLine = getToolPath() + " " + args;
		ChildProcess process(cmdLine);
		outputStream = process.readPipe();
		exitCode = process.waitExit();
		return exitCode;
	}

protected:
	virtual std::string getToolPath() const = 0;
	
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