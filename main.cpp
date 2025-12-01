
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
    std::string getToolPath() const override
    {
        return R"(C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64\cl.exe)";
	}
};

class IfcPrinter : public CmdTool
{
public:
    std::string getToolPath() const override
    {
        return getAppDirectory() + "\\ifc-printer.exe";
    }
};

struct IfcInfo
{
    std::string path;
	std::string originalSemanticContent;
    std::filesystem::file_time_type originalWriteTime;
};

std::string getIfcSemanticContent(const std::string& ifcPath)
{
    IfcPrinter ifcPrinter;
    ifcPrinter.run(ifcPath);
    return ifcPrinter.outputStream;
}

std::optional<IfcInfo> getOverwrittenIfcInfo(Arguments& arguments)
{
    arguments.expandRsp();
    if (arguments.contains("/ifcOutput") && !arguments.contains("/scanDependencies"))
    {
        int index = arguments.indexOf("/ifcOutput") + 1;
        std::string ifcOutputPath = unquote(arguments[index]);
        if (std::filesystem::exists(ifcOutputPath))
        {
            return IfcInfo
            {
                ifcOutputPath,
                getIfcSemanticContent(ifcOutputPath),
                std::filesystem::last_write_time(ifcOutputPath)
            };
        }
    }

    return {};
}

int main(int argc, char** argv)
{
	appPath = argv[0];

    Arguments arguments(argc, argv);
    std::string commandLine = arguments.toCommandLine();

	// check if we are going to overwrite an existing ifc file
	auto overwrittenIfc = getOverwrittenIfcInfo(arguments);

    // perform original compilation command
    Cl cl;
    cl.run(commandLine);
    std::cout << cl.outputStream;
    std::cerr << cl.errorStream;
    
	// if semantic content of IFC is not changed, restore original write time 
	// to prevent recompilation of dependent files
    if (overwrittenIfc.has_value())
    {
        std::string semanticContent = getIfcSemanticContent(overwrittenIfc->path);
        if (semanticContent == overwrittenIfc->originalSemanticContent)
        {
            std::cout << "ifc semantic was not changed\n";
            std::filesystem::last_write_time(overwrittenIfc->path, overwrittenIfc->originalWriteTime);
        }
    }

    return cl.exitCode;
}