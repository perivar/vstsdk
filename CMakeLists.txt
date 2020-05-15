# https://github.com/steinbergmedia/vst3sdk/blob/master/CMakeLists.txt
cmake_minimum_required (VERSION 3.4.3)

# set verbose
set(CMAKE_VERBOSE_MAKEFILE ON)

#-------------------------------------------------------------------------------
# Add VST 2 Plugins
# Note! The Vst2 Plugins currently only support being compiled using the msys2 cmake kit
#-------------------------------------------------------------------------------

# # call the CMakeLists.txt file in the freeverb directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/molot01")

# # call the CMakeLists.txt file in the minihost directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/minihost")

# # call the CMakeLists.txt file in the again directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/again")

# # call the CMakeLists.txt file in the adelay directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/adelay")

# # call the CMakeLists.txt file in the minimal host directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/minimal_vst2x_host")

# # call the CMakeLists.txt file in the vstxsynth directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/vstxsynth")

# # call the CMakeLists.txt file in the Revitar directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/vst2.x-plugins/Revitar")

# # call the CMakeLists.txt file in the vstgui.sf drawtest directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/VST2_SDK/vstgui.sf/drawtest")

# # call the CMakeLists.txt file in the vstgui_360_rc2 tutorial directory
# add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/VST2_SDK/vstgui_360_rc2/vstgui/tutorial")

# return()


#-------------------------------------------------------------------------------
# Config
#-------------------------------------------------------------------------------

# change the name of the default myplugin folder
set(SMTG_MYPLUGINS_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/vst3-plugins" CACHE STRING "Here you can add Your VST3 plug-ins folder")

# clear the path to the DEFAULT_VST3_FOLDER to avoid an access error when no administrator rights  
set(SMTG_PLUGIN_TARGET_PATH "" CACHE PATH "Here you can redefine the VST3 plug-ins folder")

#-------------------------------------------------------------------------------
# Includes
#-------------------------------------------------------------------------------

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/VST3_SDK/cmake/modules")

include(SMTG_VST3_SDK)

#-------------------------------------------------------------------------------
# SDK Project
#-------------------------------------------------------------------------------
project(vstsdk)

setupPlatformToolset()

set(ROOT "${CMAKE_CURRENT_SOURCE_DIR}/VST3_SDK")

# Here you can define where the VST 3 SDK is located
set(SDK_ROOT "${ROOT}")
set(public_sdk_SOURCE_DIR ${SDK_ROOT}/public.sdk)
set(pluginterfaces_SOURCE_DIR ${SDK_ROOT}/pluginterfaces)

# Here you can enable/disable VST GUI elements
# Note! This must be done before the setupVstGuiSupport macro is called

# disable deprecated methods
set(VSTGUI_ENABLE_DEPRECATED_METHODS OFF)

# disable all vstgui tools and examples
# set(VSTGUI_STANDALONE OFF)
# set(VSTGUI_STANDALONE_EXAMPLES OFF)
# set(VSTGUI_TOOLS OFF)
# set(VSTGUI_DISABLE_UNITTESTS ON)

# enable vstgui examples and tools (but not the unittests)
set(VSTGUI_STANDALONE ON)
set(VSTGUI_STANDALONE_EXAMPLES ON)
# set(VSTGUI_TOOLS ON) # don't set this variable to avoid CMP0077 policy error
set(VSTGUI_DISABLE_UNITTESTS ON)

# Here you can define where the VSTGUI is located
if(SMTG_ADD_VSTGUI)
    set(VSTGUI_ROOT "${ROOT}")
    setupVstGuiSupport()
endif()

include_directories(${ROOT} ${SDK_ROOT})

#-------------------------------------------------------------------------------
# From Here this is optional...

#-------------------------------------------------------------------------------
# CORE AUDIO SDK, AAX SDK
#-------------------------------------------------------------------------------
setupCoreAudioSupport()
setupAaxSupport()


#-------------------------------------------------------------------------------
# Projects
#-------------------------------------------------------------------------------
set(SDK_IDE_LIBS_FOLDER FOLDER "Libraries")

#---Add base libraries---------------------------
set(VST_SDK TRUE) # used for Base module which provides only a subset of Base for VST-SDK
add_subdirectory(${SDK_ROOT}/pluginterfaces)
add_subdirectory(${SDK_ROOT}/base)
add_subdirectory(${SDK_ROOT}/public.sdk)
add_subdirectory(${SDK_ROOT}/public.sdk/source/vst/interappaudio)

#---Add Wrappers (AU, AAX)-----------------------
if (SMTG_COREAUDIO_SDK_PATH)
    add_subdirectory(public.sdk/source/vst/auwrapper)
endif()

if(SMTG_AAX_SDK_PATH)
    add_subdirectory(public.sdk/source/vst/aaxwrapper)
    set_target_properties(aaxwrapper PROPERTIES ${SDK_IDE_LIBS_FOLDER})
endif()

#-------------------------------------------------------------------------------
# Here is added your own plugins folder location
#-------------------------------------------------------------------------------
set(SDK_IDE_MYPLUGINS_FOLDER FOLDER "MyPlugIns")
if(EXISTS ${SMTG_MYPLUGINS_SRC_PATH})
    set(SMTG_MYPLUGINS_BIN_PATH "${SMTG_MYPLUGINS_SRC_PATH}/build")
    add_subdirectory(${SMTG_MYPLUGINS_SRC_PATH} ${SMTG_MYPLUGINS_BIN_PATH})
endif()


#---Add VST3 examples (plug-ins and hosting)-----
if(SMTG_ADD_VST3_PLUGINS_SAMPLES)
    set(SDK_IDE_PLUGIN_EXAMPLES_FOLDER FOLDER "PlugInExamples")
    add_subdirectory(${SDK_ROOT}/public.sdk/samples/vst)
    add_subdirectory(${SDK_ROOT}/public.sdk/source/vst/auwrapper/again)
    add_subdirectory(${SDK_ROOT}/public.sdk/source/vst/auv3wrapper)
endif()

# Add hosting examples, it includes the validator
set(SDK_IDE_HOSTING_EXAMPLES_FOLDER FOLDER "HostingExamples")
add_subdirectory(${SDK_ROOT}/public.sdk/samples/vst-hosting)


#-------------------------------------------------------------------------------
# IDE sorting
#-------------------------------------------------------------------------------
set_target_properties(sdk PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(base PROPERTIES ${SDK_IDE_LIBS_FOLDER})
set_target_properties(pluginterfaces PROPERTIES ${SDK_IDE_LIBS_FOLDER})
if(TARGET base_ios)
    set_target_properties(base_ios PROPERTIES ${SDK_IDE_LIBS_FOLDER})
endif()

if(SMTG_ADD_VSTGUI)
    set_target_properties(vstgui PROPERTIES ${SDK_IDE_LIBS_FOLDER})
    set_target_properties(vstgui_support PROPERTIES ${SDK_IDE_LIBS_FOLDER})
    set_target_properties(vstgui_uidescription PROPERTIES ${SDK_IDE_LIBS_FOLDER})

    if(TARGET vstgui_standalone)
        set_target_properties(vstgui_standalone PROPERTIES ${SDK_IDE_LIBS_FOLDER})
    endif()
endif()


#-------------------------------------------------------------------------------
# Output useful variables
#-------------------------------------------------------------------------------

if(SMTG_ADD_VST3_HOSTING_SAMPLES)
    message("-- SMTG_ADD_VST3_HOSTING_SAMPLES is set.")
else()
    message("-- SMTG_ADD_VST3_HOSTING_SAMPLES is not set.")
endif()

if(SDK_ROOT)
    message("-- SDK_ROOT is set to : ${SDK_ROOT}")
endif()

message("-- Using CMAKE_GENERATOR: ${CMAKE_GENERATOR}")

if(MINGW)
    message("-- MINGW is set to ${MINGW}.")
else()
    message("-- MINGW is NOT set!")
endif()

if(MSVC)
    message("-- MSVC is set to ${MSVC}.")
else()
    message("-- MSVC is NOT set!")
endif()

message("-- CMAKE_CXX_PLATFORM_ID: ${CMAKE_CXX_PLATFORM_ID}")
message("-- CMAKE_SIZEOF_VOID_P: ${CMAKE_SIZEOF_VOID_P}")

if(MSVC_TOOLSET_VERSION)
    message("-- MSVC_TOOLSET_VERSION: ${MSVC_TOOLSET_VERSION}")
endif()