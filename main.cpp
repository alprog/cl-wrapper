
import std;
import arguments;
import utils;
import cmd_tool;

std::string appPath;

std::string getAppDirectory()
{
	int index = appPath.find_last_of("\\/");
    if (index != std::string::npos)
    {
		return appPath.substr(0, index);
    }
    return "";
}

class Cl : public CmdTool
{
public:
    Cl() : CmdTool(R"(C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64\cl.exe)") {}
};

class IfcPrinter : public CmdTool
{
public:
    IfcPrinter() : CmdTool(getAppDirectory() + "\\ifc-printer.exe") {}
};

int main(int argc, char** argv)
{
	appPath = argv[0];

    Arguments arguments(argc, argv);
    std::string commandLine = arguments.toCommandLine();


    IfcPrinter ifcPrinter;

    std::string ifcOutputPath;
    std::string oldIfcContent;

    arguments.expandRsp();
	if (arguments.contains("/ifcOutput") && !arguments.contains("/scanDependencies"))
    {
		int index = arguments.indexOf("/ifcOutput") + 1;
        ifcOutputPath = unquote(arguments[index]);
        if (std::filesystem::exists(ifcOutputPath))
        {
            ifcPrinter.run(ifcOutputPath);
            oldIfcContent = ifcPrinter.outputStream;
        }
    }

    Cl cl;
    cl.run(commandLine);
    std::cout << cl.outputStream;

    if (!oldIfcContent.empty())
    {
        ifcPrinter.run(ifcOutputPath);
        std::string newIfcContent = ifcPrinter.outputStream;
        if (oldIfcContent != newIfcContent)
        {
            std::cout << "ifc changed\n";
        }
        else
        {
            std::cout << "IFC NOT CHANGED!\n";
        }
    }

    return cl.exitCode;
}