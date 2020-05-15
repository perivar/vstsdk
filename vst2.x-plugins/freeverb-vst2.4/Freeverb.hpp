// Freeverb3 user interface declaration
// Based on Steinberg VST Development Kit Examples
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#ifndef __Freeverb_H
#define __Freeverb_H

#include "public.sdk/source/vst2.x/audioeffectx.h"
#include "revmodel.hpp"

enum
{
  KMode,
  KRoomSize,
  KDamp,
  KWidth,
  KWet,
  KDry,
  KNumParams
};

class Freeverb : public AudioEffectX
{
public:
  Freeverb(audioMasterCallback audioMaster);
  virtual void setSampleRate(float sampleRate);
  virtual void process(float **inputs, float **outputs, VstInt32 sampleFrames);
  virtual void processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
  virtual void setProgramName(char *name);
  virtual void getProgramName(char *name);
  virtual void setParameter(VstInt32 index, float value);
  virtual float getParameter(VstInt32 index);
  virtual void getParameterLabel(VstInt32 index, char *label);
  virtual void getParameterDisplay(VstInt32 index, char *text);
  virtual void getParameterName(VstInt32 index, char *text);
  virtual void suspend();
  virtual void resume();
  virtual bool getEffectName(char *name);
  virtual bool getVendorString(char *text);
  virtual bool getProductString(char *text);
  virtual VstInt32 canDo(char *text);

private:
  revmodel model;
  char programName[32];
};

#endif
