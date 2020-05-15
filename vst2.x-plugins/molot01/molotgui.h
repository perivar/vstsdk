// $Id$

#ifndef _molotgui_h_
#define _molotgui_h_

class StereoCompressor;
class CNButton;

class MolotGUI : public AEffGUIEditor, CControlListener {
public:
    MolotGUI(AudioEffect *effect);
    ~MolotGUI();

    virtual void    setParameter(long index, float value);

    void    setCompressor(StereoCompressor *comp);

protected:
    virtual bool    open(void *ptr);
    virtual void    close();

    virtual void    valueChanged(
#ifndef __mac__     // using patched vstgui 3.0 on pc (using vstgui 3.5 on mac)
        CDrawContext *context,
#endif
        CControl *control);

    virtual void    idle();

private:
    static const int    MAX_KNOBS = 12;
    static const int    MAX_SWITCHES = 3;
    static const int    MAX_D_CONTROLS = 6;

    enum {
        BMP_BACKGROUND = 100,
        BMP_ENGLISH,
        BMP_KNOB,
        BMP_LAMP,
        BMP_METER,
        BMP_SWITCH,
        BMP_KNOB7,
        BMP_KNOB4,
        BMP_EN_RU,
        // down line elements:
        BMP_D_ARROW,
        BMP_D_INPUT,
        BMP_D_LIM,
        BMP_D_SAT,
        BMP_D_BYPASS,
        BMP_D_RENDER,
        BMP_D_DITHER
    };

    // bitmaps
    CBitmap     *m_bmp_background;
    CBitmap     *m_bmp_english;
    CBitmap     *m_bmp_knob;
    CBitmap     *m_bmp_switch;
    CBitmap     *m_bmp_meter;
    CBitmap     *m_bmp_lamp;
    CBitmap     *m_bmp_knob7;
    CBitmap     *m_bmp_knob4;
    CBitmap     *m_bmp_en_ru;
    CBitmap     *m_bmp_d_arrow;
    CBitmap     *m_bmp_d_input;
    CBitmap     *m_bmp_d_lim;
    CBitmap     *m_bmp_d_sat;
    CBitmap     *m_bmp_d_bypass;
    CBitmap     *m_bmp_d_render;
    CBitmap     *m_bmp_d_dither;

    enum {
        SWITCH_FILTER = 100,
        SWITCH_METER,
        LAMP_LIMITER,
        METER_VU,
        SWITCH_EN_RU,
        SWITCH_D_ARROW
    };

    // controls
    CAnimKnob       *m_ctl_knobs[MAX_KNOBS];
    COnOffButton    *m_ctl_switches[MAX_SWITCHES];
    CMovieBitmap    *m_ctl_meter;
    CMovieBitmap    *m_ctl_limiting_lamp;
    CMovieBitmap    *m_ctl_english;
    COnOffButton    *m_ctl_en_ru;
    // down line controls
    COnOffButton    *m_ctl_d_arrow;
    CNButton        *m_ctl_d_controls[MAX_D_CONTROLS];
    bool            m_d_visible;

    // labels
    CTextLabel  *m_ctl_version;
    CTextLabel  *m_ctl_clip;

    static const unsigned int   TICKS_TO_REFRESH_METER = 10;        // no more than 100 fps
    static const unsigned int   TICKS_TO_REFRESH_CLIP = 1000 / 2;   // 0.5 sec to autoreset clip indicator

    // refresh GUI
    unsigned int    last_meter_ticks;
    double          last_meter_value;
    double          last_lamp_value;
    double          sum_meter_value;
    double          sum_lamp_value;
    unsigned int    n_meter_values;

    bool            last_clip_value;
    unsigned int    last_clip_ticks;

    // the main subject
    StereoCompressor    *m_comp;

    CControl    *getControlByTag(int tag);
};

// button with any states given (left click - next state; right click - prev state)
class CNButton : public CControl
{
public:
    CNButton(const CRect &size, CControlListener *listener, long tag,
        CBitmap *background, long style = COnOffButton::kPreListenerUpdate);
    virtual ~CNButton();

    virtual void    draw(CDrawContext*);

#ifndef __mac__     // using patched vstgui 3.0 on pc; using vstgui 3.5 on mac
    virtual void    mouse(CDrawContext *pContext, CPoint &where, long button = -1);
#else
    virtual CMouseEventResult	onMouseDown(CPoint& where, const long& buttons);
#endif

    virtual long    getStyle () const { return style; }
    virtual void    setStyle (long newStyle) { style = newStyle; }

    CLASS_METHODS(CNButton, CControl)

protected:
    long    style;
    long    subPixmaps;
};

#endif
