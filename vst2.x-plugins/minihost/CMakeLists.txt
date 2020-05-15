cmake_minimum_required (VERSION 3.5)
project (minihost)

message(STATUS  "Building on ${CMAKE_SYSTEM_NAME}")
message(STATUS  "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS  "Configuration types: ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS  "Project source dir: ${CMAKE_SOURCE_DIR}")
message(STATUS  "Current source dir: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS  "Binary dir: ${CMAKE_BINARY_DIR}")
message(STATUS  "Project binary dir: ${PROJECT_BINARY_DIR}")

# Name your plugin here
set(PLUGIN_NAME minihost)

set(CMAKE_VERBOSE_MAKEFILE ON)

# this is the general configuration
# disabled - use flags per target instead by using target_compile_options()
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -Wall")
# use set_target_properties instead of CMAKE_CXX_FLAGS to set the CXX_STANDARD flag

set(PLUGIN_COMPILE_FLAGS)

# flags used for Debug, Release and Tracer builds
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVST_API -DWIN32 -D_WIN32_WINNT=0x0501 -DWINVER=0x0501 -D_CRT_SECURE_NO_DEPRECATE")

# TODO: use generator expressions instead $<$<CONFIG:RELEASE>:-O2->
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    message (STATUS "Building a debug build")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -D_DEBUG")
    list(APPEND PLUGIN_COMPILE_FLAGS "-g" "-D_DEBUG")    
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message (STATUS "Building a release build")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
    list(APPEND PLUGIN_COMPILE_FLAGS "-O2")    
endif()

# instead of global linker flags, set per target using target_link_options()
set(PLUGIN_LINKER_FLAGS)

# compile the runtime as static (this will make sure that LIBGCC_S_SEH-1.DLL et al. won't show up in Dependency Walker)
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
list(APPEND PLUGIN_LINKER_FLAGS "-static" "-static-libgcc" "-static-libstdc++")

# ensure we are using the windows subsystem
# Note! this must be disabled for command line apps (non vsts)
# set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,--subsystem,windows")
# set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--subsystem,windows")
# list(APPEND PLUGIN_LINKER_FLAGS "-Wl,--subsystem,windows")

# export no symbols by default
# instead of global flags, set per target in the set_target_properties() method
# set(CMAKE_C_VISIBILITY_PRESET hidden)
# set(CMAKE_CXX_VISIBILITY_PRESET hidden)
# set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# set(CMAKE_RC_COMPILER_INIT windres)
# ENABLE_LANGUAGE(RC)
# set(CMAKE_RC_COMPILE_OBJECT
# "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")

# Add VST 2.4 SDK
set(VST_SDK_2_4_ROOT "${CMAKE_SOURCE_DIR}/include/VST2_SDK"  CACHE PATH "Path to VST 2.4 SDK")
set(VSTSDK2_4_PLUGIN_SOURCE
    ${VST_SDK_2_4_ROOT}/public.sdk/source/vst2.x/audioeffect.cpp
    ${VST_SDK_2_4_ROOT}/public.sdk/source/vst2.x/audioeffectx.cpp    
    # ${VST_SDK_2_4_ROOT}/public.sdk/source/vst2.x/vstplugmain.cpp # VST2 plugins must implement createEffectInstance and export __declspec(dllexport) VSTPluginMain
    
    ${VST_SDK_2_4_ROOT}/pluginterfaces/vst2.x/aeffect.h
    ${VST_SDK_2_4_ROOT}/pluginterfaces/vst2.x/aeffectx.h
    ${VST_SDK_2_4_ROOT}/pluginterfaces/vst2.x/vstfxstore.h
)

# set plugin root and plugin sources
set(PLUGIN_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/source")
set(PLUGIN_SRCS
  ${PLUGIN_ROOT}/minieditor.cpp
  ${PLUGIN_ROOT}/minihost.cpp
  ${VSTSDK2_4_PLUGIN_SOURCE}
)

# set plugin resources
set(RESOURCE_SRCS
  
)

# for the DEF file check GENERATE_EXPORT_HEADER
# http://gernotklingler.com/blog/creating-using-shared-libraries-different-compilers-different-operating-systems/

# create an executable
add_executable(${PLUGIN_NAME} 
	  ${PLUGIN_SRCS}
	  ${RESOURCE_SRCS}
)

# define output directory
set_target_properties(${PLUGIN_NAME} PROPERTIES
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin # note ! using bin directory for executable
  CXX_STANDARD 17
  CXX_STANDARD_REQUIRED ON
  CXX_EXTENSIONS OFF
)

# ensure we don't add the 'lib' prefix and change the suffix to .vst
set_target_properties(${PLUGIN_NAME} PROPERTIES 
  # stop CMake from prepending `lib` to library names
  PREFIX ""

  # For vst2's keep the dll suffix
  # SUFFIX ".dll"

  # export no symbols by default
  C_VISIBILITY_PRESET hidden
  CXX_VISIBILITY_PRESET hidden
  VISIBILITY_INLINES_HIDDEN 1
)

# Bring the headers, such as aeffeditor.h into the project
target_include_directories(${PLUGIN_NAME} PUBLIC ${VST_SDK_2_4_ROOT})

# link to the sdk and components libraries
# target_link_libraries(${PLUGIN_NAME} 
#   PRIVATE 
#   SDK2_4
# )

# set link flags per target
target_link_options(${PLUGIN_NAME} 
  PRIVATE 
  ${PLUGIN_LINKER_FLAGS}
)

# set compile options per target
target_compile_options(${PLUGIN_NAME} 
  PUBLIC 
  ${PLUGIN_COMPILE_FLAGS}
)