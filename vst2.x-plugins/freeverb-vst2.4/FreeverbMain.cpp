// Freeverb3 initialisation implementation
// Based on Steinberg VST Development Kit Examples
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "Freeverb.hpp"

#if defined(__GNUC__)
#define VST_EXPORT __attribute__((visibility("default")))
#else
#define VST_EXPORT
#endif

static AudioEffect *effect = NULL;
bool oome = false;

#if MAC
#pragma export on
#endif

extern "C"
{
#if WIN32
  __declspec(dllexport) AEffect *VSTPluginMain(audioMasterCallback audioMaster);
  // Freeverb4.def
  // LIBRARY Freeverb4.dll
  // EXPORTS main=VSTPluginMain
  //__declspec(dllexport)  AEffect* main (audioMasterCallback audioMaster);
#endif

  VST_EXPORT AEffect *VSTPluginMain(audioMasterCallback audioMaster)
  {
    if (!audioMaster(0, audioMasterVersion, 0, 0, 0, 0))
      return 0; // old version

    effect = new Freeverb(audioMaster);
    if (!effect)
      return 0;
    if (oome)
    {
      delete effect;
      return 0;
    }
    return effect->getAeffect();
  }
  // support for old hosts not looking for VSTPluginMain
#if (TARGET_API_MAC_CARBON && __ppc__)
  VST_EXPORT AEffect *main_macho(audioMasterCallback audioMaster)
  {
    return VSTPluginMain(audioMaster);
  }
#elif WIN32
  //VST_EXPORT AEffect* main (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
#elif BEOS
  VST_EXPORT AEffect *main_plugin(audioMasterCallback audioMaster)
  {
    return VSTPluginMain(audioMaster);
  }
#endif
}

#if MAC
#pragma export off
#endif

#if WIN32
#include <windows.h>
void *hInstance;
BOOL WINAPI DllMain(HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
  hInstance = hInst;
  return 1;
}
#endif
