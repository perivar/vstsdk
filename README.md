<div style="text-align:center">
<img src="https://steinbergmedia.github.io/vst3_doc/gfx/vst3_logo.jpg" alt="VST SDK 3" /></div>

# Official VST SDK modified to support MSYS2 MINGW compilation on Windows

## The VST SDK package contains

The full VST 3 SDK is available [here!](https://www.steinberg.net/en/company/developers.html). It contains :

- a VST 3 Plug-in Test Host Application/Validator,
- the **Steinberg VST 3 Plug-In SDK Licensing Agreement** that you have to sign if you want to develop or host VST 3 Plug-Ins.

<div id='200'/>

# Installation of MSYS2 (MINGW)

- Install MSYS2 from http://www.msys2.org/
  
- Install the windows development environment. See example here <https://www.devdungeon.com/content/install-gcc-compiler-windows-msys2-cc>
- Run the following from the msys2 command prompt:
  - Upgrade everything (S = sync, y = download fresh package databases from the server, u = upgrade installed packages)
    - `pacman -Syu`
  - Upgrade everything (S = sync, u = upgrade installed packages)
    - `pacman -Su`
  - Install make, autoconf, etc to C:\msys64\usr\bin
    - `pacman -S base-devel gcc vim cmake`
  - Install gcc in `C:\msys64\mingw64\bin\` directory to go with `C:\msys64\mingw64\include` and `C:\msys64\mingw64\lib`
    - `pacman -S mingw-w64-x86_64-toolchain`
  - Install cmake
    - `pacman -S mingw-w64-x86_64-cmake`
  
- Add `C:\msys64\mingw64\bin` to your PATH environment variable
  - `PATH=C:\msys64\mingw64\bin`
  
# To compile in Visual Studio Code

- Install the official c++ extension for VS Code <https://github.com/microsoft/vscode-cpptools>
- Install the official cmake tools extension for VS Code <https://github.com/microsoft/vscode-cmake-tools>
- Choose a kit and run 'Build'
  - `MSYS2 GCC and G++`
  - or
  - `Visual Studio 16 2019 amd64 VS16`

# To debug in Visual Studio Code

- You can use one of the debug targets in launch.json
- `cmake debug target with argument (gdb)` (working with CMAKE kit: `MSYS2 GCC and G++`)
- or
- `cmake debug target with argument (cppvsdbg)` (working with CMAKE kit: `Visual Studio 16 2019 amd64 VS16`)

- Chose the Debug target in the cmake bottom bar and click F5
- Optionally add a command line argument
- I.e. use `editorhost` as Debug target and the path to the vst as the command line argument and you can set breakpoints in the vst's