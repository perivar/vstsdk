// Freeverb3 user interface implementation
// Based on Steinberg VST Development Kit Examples
//
// Written by Jezar at Dreampoint, June 2000
// http://www.dreampoint.co.uk
// This code is public domain

#include "Freeverb.hpp"

Freeverb::Freeverb(audioMasterCallback audioMaster)
    : AudioEffectX(audioMaster, 1, KNumParams) // 1 program
{
  setNumInputs(2);
  setNumOutputs(2);
  setUniqueID('JzR4');
  canProcessReplacing();
  strcpy(programName, "Default Reverb");
}

void Freeverb::suspend()
{
  model.mute();
}

void Freeverb::resume()
{
  model.mute();
}

bool Freeverb::getEffectName(char *name)
{
  strcpy(name, "Freeverb4"); // Change this to what you want!!
  return true;
}

bool Freeverb::getVendorString(char *text)
{
  strcpy(text, "Dreampoint/Wind"); // Change this to what you want!!
  return true;
}

bool Freeverb::getProductString(char *text)
{
  strcpy(text, "Freeverb4"); // Change this to what you want!!
  return true;
}

VstInt32 Freeverb::canDo(char *text)
{
  if (!strcmp(text, "1in1out"))
    return 1;
  if (!strcmp(text, "2in2out"))
    return 1;
  if (!strcmp(text, "1in2out"))
    return 1;
  return -1;
}

void Freeverb::setProgramName(char *name)
{
  strcpy(programName, name);
}

void Freeverb::getProgramName(char *name)
{
  strcpy(name, programName);
}

void Freeverb::setParameter(VstInt32 index, float value)
{
  switch (index)
  {
  case KMode:
    model.setmode(value);
    break;
  case KRoomSize:
    model.setroomsize(value);
    break;
  case KDamp:
    model.setdamp(value);
    break;
  case KWet:
    model.setwet(value);
    break;
  case KDry:
    model.setdry(value);
    break;
  case KWidth:
    model.setwidth(value);
    break;
  }
}

float Freeverb::getParameter(VstInt32 index)
{
  float ret;

  switch (index)
  {
  case KMode:
    ret = model.getmode();
    break;
  case KRoomSize:
    ret = model.getroomsize();
    break;
  case KDamp:
    ret = model.getdamp();
    break;
  case KWet:
    ret = model.getwet();
    break;
  case KDry:
    ret = model.getdry();
    break;
  case KWidth:
    ret = model.getwidth();
    break;
  }
  return ret;
}

void Freeverb::getParameterName(VstInt32 index, char *label)
{
  switch (index)
  {
  case KMode:
    strcpy(label, "Mode");
    break;
  case KRoomSize:
    strcpy(label, "Room size");
    break;
  case KDamp:
    strcpy(label, "Damping");
    break;
  case KWet:
    strcpy(label, "Wet level");
    break;
  case KDry:
    strcpy(label, "Dry level");
    break;
  case KWidth:
    strcpy(label, "Width");
    break;
  }
}

void Freeverb::getParameterDisplay(VstInt32 index, char *text)
{
  switch (index)
  {
  case KMode:
    if (model.getmode() >= freezemode)
      strcpy(text, "Freeze");
    else
      strcpy(text, "Normal");
    break;
  case KRoomSize:
    float2string(model.getroomsize() * scaleroom + offsetroom, text, 5);
    break;
  case KDamp:
    float2string((float)(model.getdamp() * 100), text, 5);
    break;
  case KWet:
    dB2string(model.getwet() * scalewet, text, 5);
    break;
  case KDry:
    dB2string(model.getdry() * scaledry, text, 5);
    break;
  case KWidth:
    float2string((float)(model.getwidth() * 100), text, 5);
    break;
  }
}

void Freeverb::getParameterLabel(VstInt32 index, char *label)
{
  switch (index)
  {
  case KMode:
    strcpy(label, "mode");
    break;
  case KRoomSize:
    strcpy(label, "size");
    break;
  case KDamp:
  case KWidth:
    strcpy(label, "%");
    break;
  case KWet:
  case KDry:
    strcpy(label, "dB");
    break;
  }
}

void Freeverb::setSampleRate(float sampleRate)
{
  model.resetfs((int)sampleRate);
}

void Freeverb::process(float **inputs, float **outputs, VstInt32 sampleFrames)
{
  model.processmix(inputs[0], inputs[1], outputs[0], outputs[1], sampleFrames, 1);
}

void Freeverb::processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames)
{
  model.processreplace(inputs[0], inputs[1], outputs[0], outputs[1], sampleFrames, 1);
}
