module;
#include <windows.h>
export module child_process;

import std;

export class ChildProcess
{
public:
    ChildProcess(std::string cmd)
    {
        SECURITY_ATTRIBUTES sa{};
        sa.nLength = sizeof(sa);
        sa.bInheritHandle = TRUE;
        
        CreatePipe(&outPipeReadEnd, &outPipeWriteEnd, &sa, 0);
        SetHandleInformation(outPipeReadEnd, HANDLE_FLAG_INHERIT, 0);

        STARTUPINFOA siStartInfo{};
        siStartInfo.cb = sizeof(siStartInfo);
        siStartInfo.dwFlags = STARTF_USESTDHANDLES;
        siStartInfo.hStdInput = NULL;
        siStartInfo.hStdOutput = outPipeWriteEnd;
        siStartInfo.hStdError = outPipeWriteEnd;

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

        CloseHandle(outPipeWriteEnd); // we only read
    }

    std::string readPipe()
    {
        std::ostringstream outStream;
      
        std::vector<char> buf(4096);

        DWORD readCount;
        while (ReadFile(outPipeReadEnd, buf.data(), buf.size(), &readCount, NULL) && readCount > 0)
        {
            outStream.write(buf.data(), readCount);
        }

        CloseHandle(outPipeReadEnd);

        return outStream.str();
    }

    unsigned long waitExit()
    {
        WaitForSingleObject(piProcInfo.hProcess, INFINITE);

        unsigned long exitCode;
        GetExitCodeProcess(piProcInfo.hProcess, &exitCode);

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);

        return exitCode;
    }

private:
    HANDLE outPipeReadEnd, outPipeWriteEnd;
    PROCESS_INFORMATION piProcInfo;
};