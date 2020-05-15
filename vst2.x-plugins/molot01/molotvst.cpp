// $Id$

#include "system.h"

#include "public.sdk/source/vst2.x/audioeffectx.h"
#ifndef NO_GUI
# include "aeffguieditor.h"
#endif

/*
 * -MOLOT- VST plugin
 */

#include "moloteng.h"
#include "molotvst.h"
#include "vstparam.h"

#ifndef NO_GUI
# include "molotgui.h"
#endif

#define MAX_PROGRAMS    16

class plugin : public AudioEffectX
{
public:
    plugin(audioMasterCallback audioMaster);
    ~plugin();

    // State
    virtual void    resume();

    // Processing
    virtual void    processReplacing(float **inputs, float **outputs, VstInt32 sampleFrames);
    virtual void    processDoubleReplacing(double **inputs, double **outputs, VstInt32 sampleFrames);

    // Program
    virtual void    setProgram(VstInt32 program);
    virtual void    setProgramName(char *name);
    virtual void    getProgramName(char *name);
    virtual bool    getProgramNameIndexed(VstInt32 category, VstInt32 index, char *text);

    // Parameters
    virtual void    setParameter(VstInt32 index, float value);
    virtual float   getParameter(VstInt32 index);
    virtual void    getParameterLabel(VstInt32 index, char *label);
    virtual void    getParameterDisplay(VstInt32 index, char *text);
    virtual void    getParameterName(VstInt32 index, char *text);

    virtual void    setSampleRate(float sampleRate);

    virtual bool    setProcessPrecision(VstInt32 precision);

    virtual bool    getEffectName(char *name);
    virtual bool    getVendorString(char *text);
    virtual bool    getProductString(char *text);
    virtual VstInt32    getVendorVersion();

    virtual VstPlugCategory getPlugCategory () { return kPlugCategEffect; }

    float   getParamIncrement(int param) const;

    void    lock();     // lock "in processing"
    void    unlock();   // unlock "in processing"

private:
    char    m_programName[kVstMaxProgNameLen + 1];

    // params
    VstParam   *m_params[PARAM__MAX];

    struct {
        char        names[MAX_PROGRAMS][kVstMaxProgNameLen + 1];
        float       values[MAX_PROGRAMS][PARAM__MAX];
    } m_program;

    unsigned int        m_current_program;

    StereoCompressor    m_comp;

#ifdef WIN32
    CRITICAL_SECTION    m_in_processing;
#else
    pthread_mutex_t     m_in_processing;
#endif

    VstInt32    m_level;        // current processing level

    double      m_sample_rate;

    void        check_offline_mode(void);

#ifndef NO_GUI
    MolotGUI    *m_gui;
#endif
};

static const char   *g_program_names[MAX_PROGRAMS] = {
    "Default",
    "kick",
    "snare",
    "overheads",
    "room",
    "DRUM BUSS",
    "bass",
    "vocal (ins.1)",
    "vocal (ins.2)",
    "back vocal",
    "funky dbl.guitars",
    "solo guitar",
    "acoustic guitar",
    "keyboards",
    "Master BUSS",
    "premaster"
};

static const float  g_program_values[MAX_PROGRAMS][PARAM__MAX] = {
#   include "molot.dat"
};

AudioEffect *
createEffectInstance(audioMasterCallback audioMaster)
{
    return new plugin(audioMaster);
}

plugin::plugin(audioMasterCallback audioMaster)
:   AudioEffectX(audioMaster, MAX_PROGRAMS, PARAM__MAX)
{
    static const char *stereo_mode[] = {"Mono-L", "2 Mono", "R-S.chn", "Mid.", "Side", "M/S", "Stereo"};
    static const char *oversampling[] = {"1x", "2x", "4x", "8x"};
    static const char *attack_mode[] = {"sigma", "alpha"};
    static const char *input_gain[] = {"0", "+6", "+12", "-12", "-6"};
    static const char *lim_mode[] = {"Pre", "Post", "Off"};
    static const char *sat_mode[] = {"Ratio", "Off", "On"};
    static const char *off_on[] = {"Off", "On"};
    static const char *dithering[] = {"High", "Low"};

    unsigned int i;

    // init params
    m_params[PARAM_SIDEHP] = new VstParam("Filter", "Hz", 40, 260, 40, VstParam::L_NO, "Off", NULL, 0, NULL);
    m_params[PARAM_THRESHOLD] = new VstParam("Thresh.", "dB", -40, 0, -10, VstParam::L_DB, NULL, NULL, 1, NULL);
    m_params[PARAM_KNEE] = new VstParam("Knee", NULL, 0, 1, 1, VstParam::L_NO, "Softer", "Harder", 1, NULL);
    m_params[PARAM_ATTACK] = new VstParam("Attack", "ms", 1, 100, 10, VstParam::L_NO, NULL, NULL, 0, NULL);
    m_params[PARAM_RELEASE] = new VstParam("Release", "ms", 50, 1000, 100, VstParam::L_NO, NULL, NULL, 0, NULL);
    m_params[PARAM_RATIO] = new VstParam("Ratio", ": 1", 1.1, 11, 4, VstParam::L_NO, NULL, "inf", 1, NULL);
    m_params[PARAM_LIMITER] = new VstParam("Limiter", "dB", -12, 0, 0, VstParam::L_DB, NULL, NULL, 1, NULL);
    m_params[PARAM_MAKEUP] = new VstParam("Makeup", "dB", -12, 12, 0, VstParam::L_DB, NULL, NULL, 1, NULL);
    m_params[PARAM_MID_SCOOP] = new VstParam("MidScoop", "dB", -12, 12, 0, VstParam::L_DB, NULL, NULL, 1, NULL);
    m_params[PARAM_DRY_MIX] = new VstParam("Dry/Wet", "%", 0, 99, 0, VstParam::L_NO, NULL, NULL, 0, NULL);
    m_params[PARAM_STEREO_MODE] = new VstParam("St.mode", NULL, StereoCompressor::SL_MONO, StereoCompressor::SL_STEREO,
        StereoCompressor::SL_STEREO, VstParam::L_NO, NULL, NULL, 0, stereo_mode);
    m_params[PARAM_OVERSAMPLING] = new VstParam("Oversmpl", NULL, 0, 3, 2, VstParam::L_NO, NULL, NULL, 0, oversampling);
    m_params[PARAM_ATTACK_MODE] = new VstParam("Att.mode", NULL, 0, 1, 0, VstParam::L_NO, NULL, NULL, 0, attack_mode);
    // new params
    m_params[PARAM_INPUT_GAIN] = new VstParam("Input", "dB", 0, 4, 0, VstParam::L_NO, NULL, NULL, 0, input_gain);
    m_params[PARAM_LIM_MODE] = new VstParam("Lim.mode", NULL, 0, 2, 0, VstParam::L_NO, NULL, NULL, 0, lim_mode);
    m_params[PARAM_SAT_MODE] = new VstParam("Sat.mode", NULL, 0, 2, 0, VstParam::L_NO, NULL, NULL, 0, sat_mode);
    m_params[PARAM_BYPASS] = new VstParam("Bypass", NULL, 0, 1, 0, VstParam::L_NO, NULL, NULL, 0, off_on);
    m_params[PARAM_MAX_ON_RENDER] = new VstParam("Render8x", NULL, 0, 1, 0, VstParam::L_NO, NULL, NULL, 0, off_on);
    m_params[PARAM_DITHERING] = new VstParam("Dither", NULL, 0, 1, 0, VstParam::L_NO, NULL, NULL, 0, dithering);
    // reserved
    m_params[PARAM_RESERVED1] = new VstParam("n/a", NULL, 0, 1, 0, VstParam::L_NO, NULL, NULL, 1, NULL);
    m_params[PARAM_RESERVED2] = new VstParam("n/a", NULL, 0, 1, 0, VstParam::L_NO, NULL, NULL, 1, NULL);

    m_current_program = 0;

    m_level = 0;

    // default
    m_sample_rate = 44100.0;

#if 1
    // init programs from data
    _ASSERT(sizeof(m_program.values) == sizeof(g_program_values));

    memcpy(m_program.values, g_program_values, sizeof(m_program.values));

    for (i = 0; i < MAX_PROGRAMS; i++)
        strcpy(m_program.names[i], g_program_names[i]);

    for (i = 0; i < PARAM__MAX; i++)
        m_params[i]->setValue(m_program.values[m_current_program][i]);

#else
    // init programs from defaults
    for (i = 0; i < MAX_PROGRAMS; i++) {
        strcpy(m_program.names[i], "Default");
        
        for (unsigned int j = 0; j < PARAM__MAX; j++)
            m_program.values[i][j] = m_params[j]->getValue();
    }

#endif

#ifdef WIN32
    InitializeCriticalSection(&m_in_processing);
#else
    pthread_mutexattr_t ma;
    pthread_mutexattr_init(&ma);
    pthread_mutexattr_settype(&ma, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init(&m_in_processing, &ma);
    pthread_mutexattr_destroy(&ma);
#endif

    // VST stuff

    setNumInputs(2);
    setNumOutputs(2);
    setUniqueID('MLT2');	// identify
    canProcessReplacing();	// supports replacing output
    canDoubleReplacing();	// supports double precision processing
    
    DECLARE_VST_DEPRECATED(canMono)();				      

    setInitialDelay(2);     // oversampling delay

    vst_strncpy(m_programName, m_program.names[m_current_program], kVstMaxProgNameLen);	// default program name

#ifndef NO_GUI
    m_gui = new MolotGUI(this);

    // refresh values
    for (i = 0; i < PARAM__MAX; i++)
        setParameter(i, m_program.values[m_current_program][i]);

    m_gui->setCompressor(&m_comp);
#endif /* NO_GUI */
}

plugin::~plugin()
{
#ifdef _DEBUG
    // dump all params and programs
    FILE *f = fopen("molot.dat", "w");
    if (f != NULL) {
        for (unsigned int i = 0; i < MAX_PROGRAMS; i++) {

            fprintf(f, "{");

            for (unsigned int j = 0; j < PARAM__MAX; j++) {
                fprintf(f, "%f", m_program.values[i][j]);
                
                if (j != PARAM__MAX - 1)
                    fprintf(f, ", ");
            }
            
            fprintf(f, "},\n");

        }
        fclose(f);
    }
#endif

    unsigned int i;

    for (i = 0; i < PARAM__MAX; i++)
        delete m_params[i];

#ifdef WIN32
    DeleteCriticalSection(&m_in_processing);
#else
    pthread_mutex_destroy(&m_in_processing);
#endif

#ifndef NO_GUI
    // detach compressor
    m_gui->setCompressor(NULL);
#endif
}

void
plugin::setParameter(VstInt32 index, float value)
{
    static const double gain_values[5] = {1.0, 2.0, 4.0, 0.25, 0.5};        // must match input_gain[]

    if (index < PARAM__MAX) {
        m_params[index]->setValue(value);

        lock();

        switch (index) {
            case PARAM_ATTACK:
            case PARAM_RELEASE:
            case PARAM_SIDEHP:
            case PARAM_ATTACK_MODE:
                // refresh objects
                m_comp.setEnvelope(
                    m_sample_rate,
                    m_params[PARAM_ATTACK]->getParam() / 1000.0,
                    m_params[PARAM_RELEASE]->getParam() / 1000.0,
                    m_params[PARAM_SIDEHP]->isMin() ? 0.0 : m_params[PARAM_SIDEHP]->getParam(),
                    m_params[PARAM_ATTACK_MODE]->isMax());  // sharp mode

                break;

            case PARAM_THRESHOLD:
            case PARAM_KNEE:
            case PARAM_RATIO:
            case PARAM_SAT_MODE:
                m_comp.setEnvelopeK(
                    m_params[PARAM_THRESHOLD]->getParam(),
                    m_params[PARAM_KNEE]->getParam(),
                    m_params[PARAM_RATIO]->isMax() ? 100 : m_params[PARAM_RATIO]->getParam());

                switch (m_params[PARAM_SAT_MODE]->getParamInt()) {
                case 0:     // "auto"
                    // set Sat from Ratio (always on!); min Ratio, 6 dB sat, max Ratio, 0 dB sat
                    m_comp.setSat(DB_TO_K(6.0) - m_params[PARAM_RATIO]->getValue());
                    m_comp.setUseSat(true);
                    break;
                case 1:     // "off"
                    m_comp.setUseSat(false);
                    break;
                default:    // "on"
                    m_comp.setSat(DB_TO_K(24.0));   // for +24dB Kirr = 4% 
                    m_comp.setUseSat(true);
                }

                break;

            case PARAM_LIMITER:
                m_comp.setLimiter(m_params[PARAM_LIMITER]->getParam());
                break;

            case PARAM_MID_SCOOP:
                m_comp.setMidScoop(m_params[PARAM_MID_SCOOP]->getParam(), m_sample_rate);
                break;

            case PARAM_INPUT_GAIN:
            case PARAM_MAKEUP:
            case PARAM_DRY_MIX:
                m_comp.setGain(
                    m_params[PARAM_MAKEUP]->getParam(),
                    m_params[PARAM_DRY_MIX]->getParam() / 100.0,
                    gain_values[m_params[PARAM_INPUT_GAIN]->getParamInt()]);
                break;

            case PARAM_STEREO_MODE:
                m_comp.setStereoMode((StereoCompressor::stereo_mode_t)m_params[PARAM_STEREO_MODE]->getParamInt());
                break;

            case PARAM_OVERSAMPLING:
            case PARAM_MAX_ON_RENDER:
                if (m_level == kVstProcessLevelOffline &&
                    m_params[PARAM_MAX_ON_RENDER]->getParamInt() != 0) {
                    // max on render
                    m_comp.setOversampling(ChannelCompressor::MAX_OVERSAMPLING);

                } else {
                    // setup oversampling
                    m_comp.setOversampling(1 << m_params[PARAM_OVERSAMPLING]->getParamInt());   // 1, 2, 4, 8

                }

                // refresh samplerate related parameters
                setSampleRate(getSampleRate());
                break;
            
            case PARAM_LIM_MODE:
                m_comp.setLimiterMode((StereoCompressor::limiter_mode_t)m_params[PARAM_LIM_MODE]->getParamInt());
                break;
            
            case PARAM_BYPASS:
                m_comp.setBypass(m_params[PARAM_BYPASS]->getParamInt() != 0);   // off|on
                break;

            case PARAM_DITHERING:
                m_comp.setDithering(m_params[PARAM_DITHERING]->getParamInt() == 0 ? OUTPUT_DITHER : INPUT_DITHER);
                break;
        }

#ifndef NO_GUI
        m_gui->setParameter(index, value);
#endif

        unlock();
    }
}

float
plugin::getParameter(VstInt32 index)
{
    if (index < PARAM__MAX)
        return m_params[index]->getValue();
    else
        return 0;
}

void
plugin::getParameterName(VstInt32 index, char *label)
{
    if (index < PARAM__MAX)
        m_params[index]->getName(label, kVstMaxParamStrLen + 1);
}

void
plugin::getParameterDisplay(VstInt32 index, char *text)
{
    if (index < PARAM__MAX)
        m_params[index]->getStrValue(text, kVstMaxParamStrLen + 1);
}

void
plugin::getParameterLabel(VstInt32 index, char *label)
{
    if (index < PARAM__MAX)
        m_params[index]->getLabel(label, kVstMaxParamStrLen + 1);
}

void
plugin::setSampleRate(float sampleRate)
{
    m_sample_rate = sampleRate;

    // refresh attack & related
    setParameter(PARAM_ATTACK, m_params[PARAM_ATTACK]->getValue());
    setParameter(PARAM_MID_SCOOP, m_params[PARAM_MID_SCOOP]->getValue());
}

bool
plugin::setProcessPrecision(VstInt32 precision)
{
    return true;
}

bool
plugin::getEffectName(char *name)
{
    vst_strncpy(name, "Molot", kVstMaxEffectNameLen);
    return true;
}

bool
plugin::getProductString(char *text)
{
    vst_strncpy(text, "Molot", kVstMaxProductStrLen);
    return true;
}

bool
plugin::getVendorString(char *text)
{
    vst_strncpy(text, "Vladislav Goncharov", kVstMaxVendorStrLen);
    return true;
}

VstInt32
plugin::getVendorVersion()
{ 
    // 0.2
    return 200; 
}

// convert to double & process in temporary buffers
void
plugin::processReplacing(float **inputs, float **outputs, VstInt32 n)
{
    int i;
    float *in1 = inputs[0], *in2 = inputs[1], *out1 = outputs[0], *out2 = outputs[1];
    double x1, x2, y1, y2;

    check_offline_mode();

    lock();

    // stereo mode
    for (i = 0; i < n; i++) {
        x1 = in1[i];
        x2 = in2[i];

        m_comp.processSample(x1, x2, &y1, &y2);

        out1[i] = (float)y1;
        out2[i] = (float)y2;
    }

    unlock();
}

void
plugin::processDoubleReplacing(double **inputs, double **outputs, VstInt32 n)
{
    int i;
    double *in1 = inputs[0], *in2 = inputs[1], *out1 = outputs[0], *out2 = outputs[1];
    double x1, x2, y1, y2;

    check_offline_mode();

    lock();

    // stereo mode
    for (i = 0; i < n; i++) {
        x1 = in1[i];
        x2 = in2[i];

        m_comp.processSample(x1, x2, &y1, &y2);

        out1[i] = y1;
        out2[i] = y2;
    }

    unlock();
}

void
plugin::resume()
{
    m_comp.reset();

    AudioEffectX::resume();
}

void
plugin::setProgram(VstInt32 program)
{
    unsigned int i;

    lock();

    // save values in program
    for (i = 0; i < PARAM__MAX; i++)
        m_program.values[m_current_program][i] = m_params[i]->getValue();

    m_current_program = program;

    // load values from program
    for (i = 0; i < PARAM__MAX; i++)
        setParameter(i, m_program.values[m_current_program][i]);

    unlock();

    AudioEffectX::setProgram(program);
}

void
plugin::setProgramName(char *name)
{
    vst_strncpy(m_program.names[m_current_program], name, kVstMaxProgNameLen);
}

void
plugin::getProgramName(char *name)
{
    vst_strncpy(name, m_program.names[m_current_program], kVstMaxProgNameLen);
}

bool
plugin::getProgramNameIndexed(VstInt32 category, VstInt32 index, char *text)
{
    if (index >= MAX_PROGRAMS)
        return false;

    vst_strncpy(text, m_program.names[index], kVstMaxProgNameLen);

    return true;
}

float
plugin::getParamIncrement(int param) const
{
    if (param < 0 || param >= PARAM__MAX) {
        _ASSERT(FALSE);
        return 0;
    }

    return m_params[param]->getIncrement();
}

void
plugin::lock()
{
#ifdef WIN32
    EnterCriticalSection(&m_in_processing);
#else
    pthread_mutex_lock(&m_in_processing);
#endif
}

void
plugin::unlock()
{
#ifdef WIN32
    LeaveCriticalSection(&m_in_processing);
#else
    pthread_mutex_unlock(&m_in_processing);
#endif
}

/*
 * offline mode stuff
 */

void
plugin::check_offline_mode(void)
{
    int level, old_level;

    level = getCurrentProcessLevel();

    if (level != m_level) {
        // the level was changed
        old_level = m_level;
        m_level = level;

        if (m_params[PARAM_MAX_ON_RENDER]->getParamInt() != 0 &&
            ((old_level != kVstProcessLevelOffline && m_level == kVstProcessLevelOffline) ||
            (old_level == kVstProcessLevelOffline && m_level != kVstProcessLevelOffline))) {

            // refresh oversampling
            setParameter(PARAM_OVERSAMPLING, m_params[PARAM_OVERSAMPLING]->getValue());
        }
    }
}

/*
 * global stuff
 */

float
get_param_increment(AudioEffect *effect, int param)
{
    return ((plugin *)effect)->getParamIncrement(param);
}

void
lock_effect(AudioEffect *effect)
{
    ((plugin *)effect)->lock();
}

void
unlock_effect(AudioEffect *effect)
{
    ((plugin *)effect)->unlock();
}
