
import std;
import arguments;
import utils;
import cmd_tool;

int main(int argc, char** argv)
{
    Arguments arguments(argc, argv);
    std::string commandLine = arguments.toCommandLine();

    arguments.expandRsp();
	if (arguments.contains("/ifcOutput") && !arguments.contains("/scanDependencies"))
    {
		int index = arguments.indexOf("/ifcOutput") + 1;
		std::string ifcOutputPath = unquote(arguments[index]);

		std::cout << ifcOutputPath << std::endl;

    }

    std::string clPath = R"(C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64\cl.exe)";
    CmdTool cl(clPath);

    cl.run(commandLine);
    std::cout << cl.outputStream;

    return cl.exitCode;
}