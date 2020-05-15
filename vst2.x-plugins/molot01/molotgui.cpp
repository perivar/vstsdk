// $Id$

#include "system.h"

#include "aeffguieditor.h"
#include "moloteng.h"
#include "molotgui.h"
#include "molotvst.h"
#include "vstparam.h"   // K_TO_DB

static const char   g_version_info[] = " 0.2.168";

static float    g_en_ru = -1.0f;    // default

MolotGUI::MolotGUI(AudioEffect *effect)
:   AEffGUIEditor(effect)
{
    m_bmp_background = new CBitmap(BMP_BACKGROUND);

    rect.left   = 0;
    rect.top    = 0;
    rect.right  = (short)m_bmp_background->getWidth();
    rect.bottom = (short)m_bmp_background->getHeight();

    // levels
    last_meter_ticks = 0;
    last_meter_value = 0.0;
    last_lamp_value = 0.0;
    sum_meter_value = 0.0;
    sum_lamp_value = 0.0;
    n_meter_values = 0;

    // clip
    last_clip_ticks = 0;
    last_lamp_value = false;

    // bottom line controls
    m_d_visible = false;

    frame = NULL;

    m_comp = NULL;
}

MolotGUI::~MolotGUI()
{
    m_bmp_background->forget();
}

void
MolotGUI::setCompressor(StereoCompressor *comp)
{
    m_comp = comp;
}

bool
MolotGUI::open(void *ptr)
{
    CRect size;
    CPoint offset0, xy;
    int i;
    CCoord w, h;

    AEffGUIEditor::open(ptr);

    // bitmaps
    
    m_bmp_english = new CBitmap(BMP_ENGLISH);
    m_bmp_knob = new CBitmap(BMP_KNOB);
    m_bmp_switch = new CBitmap(BMP_SWITCH);
    m_bmp_meter = new CBitmap(BMP_METER);
    m_bmp_lamp = new CBitmap(BMP_LAMP);
    m_bmp_knob7 = new CBitmap(BMP_KNOB7);
    m_bmp_knob4 = new CBitmap(BMP_KNOB4);
    m_bmp_en_ru = new CBitmap(BMP_EN_RU);
    m_bmp_d_arrow = new CBitmap(BMP_D_ARROW);
    m_bmp_d_input = new CBitmap(BMP_D_INPUT);
    m_bmp_d_lim = new CBitmap(BMP_D_LIM);
    m_bmp_d_sat = new CBitmap(BMP_D_SAT);
    m_bmp_d_bypass = new CBitmap(BMP_D_BYPASS);
    m_bmp_d_render = new CBitmap(BMP_D_RENDER);
    m_bmp_d_dither = new CBitmap(BMP_D_DITHER);

    // frame

    size = CRect(0, 0, m_bmp_background->getWidth(), m_bmp_background->getHeight());
    frame = new CFrame(size, ptr, this);
    frame->setBackground(m_bmp_background);

    rect.right  = (short)size.right;
    rect.bottom = (short)size.bottom;

    offset0 = CPoint(0, 0);

    // english layer

    w = m_bmp_english->getWidth();
    h = m_bmp_english->getHeight() / 2;   // 2 subimages (off/on)

    m_ctl_english = new CMovieBitmap(CRect(0, 0, w, h), this, 0, m_bmp_english, offset0);
    frame->addView(m_ctl_english);

    if (g_en_ru == -1.0f) {
        // default value by locale
        g_en_ru = (strncmp(setlocale(LC_CTYPE, ""), "Russian", 7) == 0) ? 0.0f : 1.0f;
    }
    m_ctl_english->setValue(g_en_ru);

    // knobs
    
    w = m_bmp_knob->getWidth();
    h = m_bmp_knob->getHeight() / 103;   // 103 subimages

    i = 0;

#define KNOB(_x, _y, param)                                             \
    xy = CPoint((_x), (_y));                                            \
    m_ctl_knobs[i] = new CAnimKnob(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, (param), m_bmp_knob, offset0);    \
    m_ctl_knobs[i]->setStartAngle((float)(218.75 * M_PI / 180.0));      \
    m_ctl_knobs[i]->setRangeAngle(-(float)(256.32 * M_PI / 180.0));     \
    m_ctl_knobs[i]->setWheelInc(get_param_increment(effect, (param)));  \
    frame->addView(m_ctl_knobs[i++])

    KNOB(55, 35, PARAM_SIDEHP);
    KNOB(56, 148, PARAM_ATTACK);
    KNOB(55, 259, PARAM_RELEASE);
    KNOB(55, 370, PARAM_RATIO);
    KNOB(229, 259, PARAM_THRESHOLD);
    KNOB(229, 371, PARAM_KNEE);
    KNOB(382, 260, PARAM_MAKEUP);
    KNOB(382, 371, PARAM_DRY_MIX);
    KNOB(536, 36, PARAM_LIMITER);
    KNOB(536, 148, PARAM_MID_SCOOP);

#undef KNOB

    h = m_bmp_knob7->getHeight() / 7;   // 7 subimages

    xy = CPoint(537, 260);
    m_ctl_knobs[i] = new CAnimKnob(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, PARAM_STEREO_MODE, m_bmp_knob7, offset0);
    m_ctl_knobs[i]->setStartAngle((float)(168.92 * M_PI / 180.0));
    m_ctl_knobs[i]->setRangeAngle(-(float)(157.2 * M_PI / 180.0)); 
    m_ctl_knobs[i]->setWheelInc(get_param_increment(effect, PARAM_STEREO_MODE));
    frame->addView(m_ctl_knobs[i++]);

    h = m_bmp_knob4->getHeight() / 4;   // 4 subimages

    xy = CPoint(534, 371);
    m_ctl_knobs[i] = new CAnimKnob(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, PARAM_OVERSAMPLING, m_bmp_knob4, offset0);
    m_ctl_knobs[i]->setStartAngle((float)(129.14 * M_PI / 180.0));
    m_ctl_knobs[i]->setRangeAngle(-(float)(74.63 * M_PI / 180.0)); 
    m_ctl_knobs[i]->setWheelInc(get_param_increment(effect, PARAM_OVERSAMPLING));
    frame->addView(m_ctl_knobs[i++]);

    // switches

    w = m_bmp_switch->getWidth();
    h = m_bmp_switch->getHeight() / 2;   // 2 subimages

    i = 0;

#define SWITCH(_x, _y, param)           \
    xy = CPoint((_x), (_y));            \
    m_ctl_switches[i] = new COnOffButton(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, (param), m_bmp_switch);    \
    frame->addView(m_ctl_switches[i++]);

    SWITCH(178, 67, SWITCH_FILTER);
    SWITCH(179, 178, PARAM_ATTACK_MODE);
    SWITCH(463, 178, SWITCH_METER);

#undef SWITCH

    // lamp

    w = m_bmp_lamp->getWidth();
    h = m_bmp_lamp->getHeight() / 11;   // 11 subimages

    xy = CPoint(462, 68);
    m_ctl_limiting_lamp = new CMovieBitmap(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, LAMP_LIMITER, m_bmp_lamp, offset0);
    frame->addView(m_ctl_limiting_lamp);

    // meter animation

    w = m_bmp_meter->getWidth();
    h = m_bmp_meter->getHeight() / 141;     // 141 subimages!

    xy = CPoint(275, 88);
    m_ctl_meter = new CMovieBitmap(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, METER_VU, m_bmp_meter, offset0);
    frame->addView(m_ctl_meter);

    // version label
    size = CRect(1, 1, 100, 10);
    m_ctl_version = new CTextLabel(size, g_version_info);
    m_ctl_version->setBackColor(kTransparentCColor);
    m_ctl_version->setFontColor(kBlackCColor);
    m_ctl_version->setFrameColor(kTransparentCColor);
    m_ctl_version->setHoriAlign(kLeftText);
    m_ctl_version->setTransparency(true);
    m_ctl_version->setFont(kNormalFontVerySmall);
    frame->addView(m_ctl_version);

    // en/ru switch

    w = m_bmp_en_ru->getWidth();
    h = m_bmp_en_ru->getHeight() / 2;       // 2 subimages

    xy = CPoint(613, 481);
    m_ctl_en_ru = new COnOffButton(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, SWITCH_EN_RU, m_bmp_en_ru);
    frame->addView(m_ctl_en_ru);

    m_ctl_en_ru->setValue(m_ctl_english->getValue());   // synchronize with language

    /*
     * down line stuff
     */

    // arrow
    xy = CPoint(3, 488);

    w = m_bmp_d_arrow->getWidth();
    h = m_bmp_d_arrow->getHeight() / 2;     // 2 subimages

    m_ctl_d_arrow = new COnOffButton(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, SWITCH_D_ARROW, m_bmp_d_arrow);
    frame->addView(m_ctl_d_arrow);

    // create all items and reference them

    i = 0;
    xy.x += 16;
    xy.y += 18;

#define N_CONTROL(param, bmp, subimages)    \
    w = (bmp)->getWidth();                  \
    h = (bmp)->getHeight() / (subimages);   \
    xy.y -= h + 4;                          \
    m_ctl_d_controls[i++] = new CNButton(CRect(xy.x, xy.y, xy.x + w, xy.y + h), this, (param), (bmp))

    N_CONTROL(PARAM_DITHERING, m_bmp_d_dither, 2);
    N_CONTROL(PARAM_MAX_ON_RENDER, m_bmp_d_render, 2);

    xy.y -= 10;

    N_CONTROL(PARAM_BYPASS, m_bmp_d_bypass, 2);
    N_CONTROL(PARAM_SAT_MODE, m_bmp_d_sat, 3);
    N_CONTROL(PARAM_LIM_MODE, m_bmp_d_lim, 3);
    N_CONTROL(PARAM_INPUT_GAIN, m_bmp_d_input, 5);

#undef N_CONTROL

    // refresh all parameters
    for (i = 0; i < PARAM__MAX; i++)
        setParameter(i, effect->getParameter(i));

    return true;
}

void
MolotGUI::close()
{
    // destroy controls not in frame
    for (int i = 0; i < MAX_D_CONTROLS; i++)
        m_ctl_d_controls[i]->forget();

    delete frame;
    frame = NULL;
}

// the value changed by automation
void
MolotGUI::setParameter(long index, float value)
{
    CControl *ctl;

    if (!isOpen() || frame == NULL)
        return;     // no GUI to change

    switch (index) {
    case PARAM_SIDEHP:
        if (value != 0.0) {
            // set knob to value; switch to on
            ctl = getControlByTag(index);
            _ASSERT(ctl != NULL);

            ctl->setValue(effect->getParameter(index));

            ctl = getControlByTag(SWITCH_FILTER);
            _ASSERT(ctl != NULL);

            ctl->setValue(1.0);

        } else {
            // switch to off; doesn't touch the knob
            ctl = getControlByTag(SWITCH_FILTER);
            _ASSERT(ctl != NULL);

            ctl->setValue(0.0);

        }
        break;

    case PARAM_THRESHOLD:
    case PARAM_KNEE:
    case PARAM_ATTACK:
    case PARAM_RELEASE:
    case PARAM_RATIO:
    case PARAM_LIMITER:
    case PARAM_MAKEUP:
    case PARAM_MID_SCOOP:
    case PARAM_DRY_MIX:
    case PARAM_STEREO_MODE:
    case PARAM_OVERSAMPLING:
    case PARAM_INPUT_GAIN:
    case PARAM_LIM_MODE:
    case PARAM_SAT_MODE:
    case PARAM_BYPASS:
    case PARAM_MAX_ON_RENDER:
    case PARAM_DITHERING:

        // passthrough
        ctl = getControlByTag(index);
        _ASSERT(ctl != NULL);

        ctl->setValue(effect->getParameter(index));

        break;

    case PARAM_ATTACK_MODE:
        // up: alpha; down: sigma
        ctl = getControlByTag(index);
        _ASSERT(ctl != NULL);

        ctl->setValue(effect->getParameter(index));
        break;
    }
}

// the value changed by user
void
MolotGUI::valueChanged(
#ifndef __mac__     // using patched vstgui 3.0 on pc; using vstgui 3.5 on mac
    CDrawContext *context,
#endif
    CControl *control)
{
    CControl *ctl;
    float value;
    int i;

    switch (control->getTag()) {
    case PARAM_SIDEHP:
        // check "Filter" switch
        ctl = getControlByTag(SWITCH_FILTER);
        _ASSERT(ctl != NULL);

        if (ctl->getValue() == 1.0) {       // up: on; down: off
            // allow

            value = control->getValue();
            if (value == 0.0)
                value = 1e-6;   // 0.0 is -off-

            effect->setParameterAutomated(control->getTag(), value);

        } // else ignore setting
        
        break;
    
    case SWITCH_FILTER:
        if (control->getValue() == 1.0) {   // up: on; down: off
            // on: set SIDEHP value
            ctl = getControlByTag(PARAM_SIDEHP);
            _ASSERT(ctl != NULL);

            value = ctl->getValue();
            if (value == 0.0)
                value = 1e-6;   // 0.0 is -off-

            effect->setParameterAutomated(ctl->getTag(), value);
        
        } else {
            // off: set 0.0
            effect->setParameterAutomated(PARAM_SIDEHP, 0.0);

        }
        break;

    case PARAM_THRESHOLD:
    case PARAM_KNEE:
    case PARAM_ATTACK:
    case PARAM_RELEASE:
    case PARAM_RATIO:
    case PARAM_LIMITER:
    case PARAM_MAKEUP:
    case PARAM_MID_SCOOP:
    case PARAM_DRY_MIX:
    case PARAM_STEREO_MODE:
    case PARAM_OVERSAMPLING:
    case PARAM_INPUT_GAIN:
    case PARAM_LIM_MODE:
    case PARAM_SAT_MODE:
    case PARAM_BYPASS:
    case PARAM_MAX_ON_RENDER:
    case PARAM_DITHERING:
        // passthrough
        effect->setParameterAutomated(control->getTag(), control->getValue());
        break;

    case PARAM_ATTACK_MODE:
        // up: alpha; down: sigma
        effect->setParameterAutomated(control->getTag(), control->getValue());
        break;

    case SWITCH_METER:
        // switch meter mode in compressor (up: input, down: GR)
        lock_effect(effect);    // avoid race conditions with processSample()

        m_comp->setVU(control->getValue() == 0.0);

        unlock_effect(effect);
        break;

    case SWITCH_EN_RU:
        // synchronize with language
        m_ctl_english->setValue(control->getValue());
        m_ctl_english->setDirty(true);
        // set global default
        g_en_ru = control->getValue();
#ifdef __mac__
        frame->invalid();
#endif
        break;

    case SWITCH_D_ARROW:
        if (m_d_visible) {
            // remove controls from main frame
            for (i = 0; i < MAX_D_CONTROLS; i++)
                frame->removeView(m_ctl_d_controls[i]);

#ifdef __mac__
            frame->invalid();
#endif
        }
        
        m_d_visible = (control->getValue() == 1.0f);

        if (m_d_visible) {

            // add controls to main frame
            for (i = 0; i < MAX_D_CONTROLS; i++) {
                frame->addView(m_ctl_d_controls[i]);
                m_ctl_d_controls[i]->remember();
            }

        }

        frame->setDirty(true);
        break;
    }

}

CControl *
MolotGUI::getControlByTag(int tag)
{
    CControl *result = NULL;
    int i;
    
    for (i = 0; i < MAX_KNOBS; i++) {
        if (m_ctl_knobs[i]->getTag() == tag) {
            result = m_ctl_knobs[i];
            break;
        }
    }

    if (result == NULL) {

        for (i = 0; i < MAX_SWITCHES; i++) {
            if (m_ctl_switches[i]->getTag() == tag) {
                result = m_ctl_switches[i];
                break;
            }
        }

    }

    if (result == NULL) {
        // check bottom line controls
        for (i = 0; i < MAX_D_CONTROLS; i++) {
            if (m_ctl_d_controls[i]->getTag() == tag) {
                result = m_ctl_d_controls[i];
                break;
            }
        }
    }

    return result;
}

void
MolotGUI::idle()
{
    double meter_value, lamp_value;

// PIN - disabled 21.04.2020
// #ifndef __mac__
//     // use VSTGUI hacks for this stuff
//     CFrame::CGuard  guard(frame);   // avoid race conditions with WindowProc
// #endif

    if (m_comp == NULL) {
        AEffGUIEditor::idle();

        return;     // nothing to do
    }

    // accumulate for average

    sum_meter_value += m_comp->getVU();
    sum_lamp_value += m_comp->getLimiting();

    n_meter_values++;

    if (getTicks() - last_meter_ticks >= TICKS_TO_REFRESH_METER) {

        /*
         * refresh meter
         */

        static const double max_level_k = DB_TO_K(3.0);     // VU meter up to +3 db
        static const double min_level_k = 0.2;              // VU meter linearity from 20% to 100%
        static const double min_meter = 13.0 / 140.0;       // for 20% image #13; for max image #140

        meter_value = sum_meter_value / n_meter_values;

        if (meter_value > max_level_k)
            meter_value = max_level_k;

        if (meter_value > min_level_k) { 

            // make it 0..1 (20%..+3 db)
            meter_value = (meter_value - min_level_k) / (max_level_k - min_level_k) * (1.0 - min_meter) + min_meter;

        } else {

            if (meter_value  < 0.0)
                meter_value = 0.0;  // sanity checking

            meter_value = meter_value / min_level_k * min_meter;

        }


        // change visible meter value
        m_ctl_meter->setValue((float)meter_value);
        m_ctl_meter->setDirty(true);

        last_meter_value = meter_value;

        /*
         * refresh lamp
         */

        lamp_value = sum_lamp_value / n_meter_values;

        // change visible lamp value
        m_ctl_limiting_lamp->setValue((float)lamp_value);
        m_ctl_limiting_lamp->setDirty(true);

        last_lamp_value = lamp_value;

        last_meter_ticks = getTicks();

        // reset accumulating sums

        sum_meter_value = 0.0;
        sum_lamp_value = 0.0;
        
        n_meter_values = 0;
    }

    /*
     * refresh clip
     */
    
    if (m_comp->getClip(true)) {

        if (!last_clip_value) {
            // set version to Red
            m_ctl_version->setFontColor(kRedCColor);
            m_ctl_version->setDirty(true);
        }

        // in clip!
        last_clip_value = true;
        last_clip_ticks = getTicks();

    } else if (last_clip_value && getTicks() - last_clip_ticks >= TICKS_TO_REFRESH_CLIP) {

        // clear clip
        last_clip_value = false;

        // set version to Black
        m_ctl_version->setFontColor(kBlackCColor);
        m_ctl_version->setDirty(true);

    }

    // process setDirty()-ies
    AEffGUIEditor::idle();

}

/*
 * CNButton control
 */

CNButton::CNButton(const CRect &size, CControlListener *listener, long tag,
    CBitmap *background, long style)
:   CControl(size, listener, tag, background),
    style(style)
{
    subPixmaps = background->getHeight() / size.getHeight();
    defaultValue = 0.0f;
}

CNButton::~CNButton()
{
}

void
CNButton::draw(CDrawContext *pContext)
{
    CCoord off;

    if (pBackground != NULL) {
        off = (CCoord)(value * (subPixmaps - 1)) *  (pBackground->getHeight() / subPixmaps);

        if (bTransparencyEnabled)
            pBackground->drawTransparent(pContext, size, CPoint(0, off));
        else
            pBackground->draw(pContext, size, CPoint(0, off));
    }
    
    setDirty(false);
}

#ifndef __mac__     // using patched vstgui 3.0 on pc; using vstgui 3.5 on mac

void
CNButton::mouse(CDrawContext *pContext, CPoint &where, long button)
{
    if (!bMouseEnabled)
        return;

    if (button == -1)
        button = pContext->getMouseButtons();
    
    if (!(button & kLButton) && !(button & kRButton))
        return;

    if (listener != NULL && button & (kAlt | kShift | kControl | kApple)) {
        if (listener->controlModifierClicked(
            pContext,
            this, button) != 0) {
            return;
        }
    }
    
    if (!checkDefaultValue(pContext, button)) {

        if (button & kLButton) {
            // +1
            value += 1.0f / (subPixmaps - 1);

            if (value > 1.0f)
                value = 0.0f;

        } else {
            // -1
            value -= 1.0f / (subPixmaps - 1);

            if (value < 0.0f)
                value = 1.0f;

        }

    }
    
    if (listener != NULL && style == COnOffButton::kPostListenerUpdate) {
        // begin of edit parameter
        beginEdit();
    
        listener->valueChanged (pContext, this);
    
        // end of edit parameter
        endEdit();
    }
    
    doIdleStuff();
    
    if (listener != NULL && style == COnOffButton::kPreListenerUpdate) {
        // begin of edit parameter
        beginEdit();
    
        listener->valueChanged (pContext, this);
        
        // end of edit parameter
        endEdit();
    }
}

#else /* __mac__ */

CMouseEventResult
CNButton::onMouseDown(CPoint& where, const long& buttons)
{
    if (!(buttons & kLButton) && !(buttons & kRButton))
        return kMouseEventNotHandled;
    
    if (!checkDefaultValue(buttons)) {
        
        if (buttons & kLButton) {
            // +1
            value += 1.0f / (subPixmaps - 1);
            
            if (value > 1.0f)
                value = 0.0f;
            
        } else {
            // -1
            value -= 1.0f / (subPixmaps - 1);
            
            if (value < 0.0f)
                value = 1.0f;
            
        }
        
    }
    
    invalid();
    
    if (listener != NULL && style == COnOffButton::kPostListenerUpdate) {
        // begin of edit parameter
        beginEdit();
        
        listener->valueChanged(this);
        
        // end of edit parameter
        endEdit();
    }
    
    doIdleStuff();
    
    if (listener != NULL && style == COnOffButton::kPreListenerUpdate) {
        // begin of edit parameter
        beginEdit();
        
        listener->valueChanged(this);
        
        // end of edit parameter
        endEdit();
    }
    
    return kMouseDownEventHandledButDontNeedMovedOrUpEvents;
}

#endif /* __mac__ */
