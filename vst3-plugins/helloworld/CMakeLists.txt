
set(plug_sources
    include/plugcontroller.h
    include/plugids.h
    include/plugprocessor.h
    include/version.h
    source/plugfactory.cpp
    source/plugcontroller.cpp
    source/plugprocessor.cpp
)

#--- HERE change the target Name for your plug-in (for ex. set(target myDelay))-------
set(target helloworld)

smtg_add_vst3plugin(${target} ${SDK_ROOT} ${plug_sources})
set_target_properties(${target} PROPERTIES ${SDK_IDE_MYPLUGINS_FOLDER})

# PIN: 04.04.2020 - add libraries vstgui and the missing windows libraries
if(MINGW)
    set(VST_LIBS)
    list(APPEND VST_LIBS base sdk)
    list(APPEND VST_LIBS "-static-libstdc++" "-static-libgcc" "-static -lpthread")
    target_link_libraries(${target} ${VST_LIBS})
else()
    target_link_libraries(${target} PRIVATE base sdk)
endif()

if(MAC)
    smtg_set_bundle(${target} INFOPLIST "${CMAKE_CURRENT_LIST_DIR}/resource/Info.plist" PREPROCESS)
elseif(WIN)
    target_sources(${target} PRIVATE resource/plug.rc)
endif()
