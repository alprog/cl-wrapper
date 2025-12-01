#define WIN32_LEAN_AND_MEAN

#include <windows.h>

import std;
import arguments;



int main(int argc, char** argv)
{
    Arguments arguments(argc, argv);
    std::string commandLine = arguments.toCommandLine();

    arguments.expandRsp();
	if (arguments.contains("/ifcOutput") && !arguments.contains("/scanDependencies"))
    {
        commandLine = arguments.toCommandLine();
    }

    std::cout << commandLine << std::endl;

    std::string clPath = R"(C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.50.35717\bin\Hostx64\x64\cl.exe)";
    std::string cmd = clPath + " " + commandLine;

    SECURITY_ATTRIBUTES sa{};
    sa.nLength = sizeof(sa);
    sa.bInheritHandle = TRUE;

    HANDLE childInRd, childInWr;
    HANDLE childOutRd, childOutWr;

    CreatePipe(&childInRd, &childInWr, &sa, 0);
    CreatePipe(&childOutRd, &childOutWr, &sa, 0);

    SetHandleInformation(childOutRd, HANDLE_FLAG_INHERIT, 0);
    SetHandleInformation(childInWr, HANDLE_FLAG_INHERIT, 0);

    STARTUPINFOA siStartInfo{};
    siStartInfo.cb = sizeof(siStartInfo);
    siStartInfo.dwFlags = STARTF_USESTDHANDLES;
    siStartInfo.hStdInput = childInRd;
    siStartInfo.hStdOutput = childOutWr;
    siStartInfo.hStdError = childOutWr;

    PROCESS_INFORMATION piProcInfo{};
    CreateProcessA(
        NULL,
        cmd.data(),    // command line 
        NULL,          // process security attributes 
        NULL,          // primary thread security attributes 
        TRUE,          // handles are inherited 
        0,             // creation flags 
        NULL,          // use parent's environment 
        NULL,          // use parent's current directory 
        &siStartInfo,  // STARTUPINFO pointer 
        &piProcInfo);  // receives PROCESS_INFORMATION 

    CloseHandle(childOutWr); // only read
    CloseHandle(childInRd);  // only write

    HANDLE stdoutH = GetStdHandle(STD_OUTPUT_HANDLE);

    std::vector<char> buf(4096);
    DWORD n;

    while (ReadFile(childOutRd, buf.data(), buf.size(), &n, NULL) && n > 0) {
        DWORD w;
        WriteFile(stdoutH, buf.data(), n, &w, NULL);
    }

    CloseHandle(childOutRd);

    //HANDLE stdinH = GetStdHandle(STD_INPUT_HANDLE);
    //while (ReadFile(stdinH, buf.data(), buf.size(), &n, NULL) && n > 0) {
    //    DWORD w;
    //    WriteFile(childInWr, buf.data(), n, &w, NULL);
    //}

    CloseHandle(childInWr);

    std::cout << "UO" << std::endl;
    std::flush(std::cout);

    WaitForSingleObject(piProcInfo.hProcess, INFINITE);

    std::cout << "YU" << std::endl;
    std::flush(std::cout);

    DWORD exitCode;
    GetExitCodeProcess(piProcInfo.hProcess, &exitCode);

    CloseHandle(piProcInfo.hProcess);
    CloseHandle(piProcInfo.hThread);

    return exitCode;
}