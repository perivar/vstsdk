@echo off

:: this script requires 3 parameters
:: '${workspaceFolder}' 
:: '${fileBasenameNoExtension}' 
:: '${fileBasename}'

:: set the path to your visual studio vcvars script, it is different for every version of Visual Studio.
set VS2019TOOLS="C:\Program Files (x86)\Microsoft Visual Studio\2019\BuildTools\VC\Auxiliary\Build\vcvarsall.bat"

:: make sure we found them
if not exist %VS2019TOOLS% (
    echo VS 2019 Build Tools are missing!
    exit
)

:: call that script, which essentially sets up the VS Developer Command Prompt
call %VS2019TOOLS% x64

:: run the compiler with your arguments
REM /W4 displays level 1, level 2, and level 3 warnings, and all level 4 (informational) warnings that aren't off by default
REM /EHsc catches C++ exceptions only and tells the compiler to assume that functions declared as extern "C" never throw a C++ exception.
REM /Zi option produces a separate PDB file that contains all the symbolic debugging information for use with the debugger. The debugging information is not included in the object files or executable, which makes them much smaller.
REM /Od Turns off all optimizations in the program and speeds compilation. Helpful for debugging.
set compilerflags=/W4 /Od /Zi /EHsc /MP

set linkerflags=/OUT:%2.exe user32.lib gdi32.lib shell32.lib ole32.lib oleaut32.lib d2d1.lib dwrite.lib windowscodecs.lib
REM /SUBSYSTEM:WINDOWS
REM /SUBSYSTEM:CONSOLE

set includepath=%1/include/VST3_SDK

cl.exe %compilerflags% /I %includepath% %3 /link %linkerflags%

exit