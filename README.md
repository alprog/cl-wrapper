
# cl-wrapper

Simple wrapper over MSVC compiler (cl.exe) to improve incremental builds for C++ modules.

# Problem

Currently MSVC compiler generates new version of BMI-file (*.ifc) after any change of module source file (*.ixx).
This means that any source change triggers a recompile of all dependencies, even if the actual interface has not changed.
It makes modules behave almost identical to headers (and impose separation of interface and implementation).

However, with C++ modules we can do a better job.
We can skip recompilation of dependencies if only implementation/formating part of the file was changed, but not the interface.
It will make possible to use single-file modules without sacrificing incremental builds.

# How it works

cl-wrapper is replacement of MSVC cl.exe. It simply redirect all the commands to the original compiler but also do additional logic:

- it checks, are we going to overwrite an existing IFC file
- if so, compare semantic content of original and overwritten files (using ifc-printer from [IFC SDK](https://github.com/microsoft/ifc))
- if they are identical, restore original last_write_time

# Usage

1. Build cl-wrapper.sln (it will produce bin/cl.exe)
2. Add ifc-printer.exe from [IFC SDK](https://github.com/microsoft/ifc) to the same bin directory
3. Add bin folder to "VC++ Directories >> "Executable Directories" of your project
4. Enjoy improved incremental builds!