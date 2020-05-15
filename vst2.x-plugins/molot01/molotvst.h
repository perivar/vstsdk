#ifndef _molotvst_h_
#define _molotvst_h_

enum {
    // knob params
    PARAM_SIDEHP,
    PARAM_THRESHOLD,
    PARAM_KNEE,
    PARAM_ATTACK,
    PARAM_RELEASE,
    PARAM_RATIO,
    PARAM_LIMITER,
    PARAM_MAKEUP,
    PARAM_MID_SCOOP,
    PARAM_DRY_MIX,
    PARAM_STEREO_MODE,

    // switch params
    PARAM_OVERSAMPLING,
    PARAM_ATTACK_MODE,

    // added for > 0.2.145
    PARAM_INPUT_GAIN,
    PARAM_LIM_MODE,
    PARAM_SAT_MODE,
    PARAM_BYPASS,

    PARAM_MAX_ON_RENDER,
    PARAM_DITHERING,

    // reserved for future use
    PARAM_RESERVED1,
    PARAM_RESERVED2,

    PARAM__MAX
};

float   get_param_increment(AudioEffect *effect, int param);

void    lock_effect(AudioEffect *effect);
void    unlock_effect(AudioEffect *effect);

#endif
