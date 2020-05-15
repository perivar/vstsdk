// $Id$

#ifndef _vstparam_h_
#define _vstparam_h_

#define DB_TO_K(db)     (pow(10.0, (double)(db) / 20.0))
#define K_TO_DB(k)      (20.0 * log10(k))

/*
 * VstParam: helper to map VST values (0..1) to real parameters
 */

class VstParam {
public:
    enum Log {
        L_NO,   // linear
        L_HZ,   // hz logarithmic scale
        L_KHZ,  // store value in kHz but convert to Hz on getParam()
        L_DB,   // linear value but convert to K on getParam()
        L_BOOL  // the parameter is boolean
    };

    VstParam(const char *name,     // "Freq"
        const char *label,      // "Hz"
        double min,             // 100 hz
        double max,             // 10000 hz
        double def,             // default
        VstParam::Log is_log,            // logarithmic scale
        const char *min_name,   // not NULL - "min_name" for min value
        const char *max_name,   // not NULL - "max_name" for max value
        int precision,          // 0 - integer; 1 - 0.1; 2 - 0.02 etc.
        const char *labels[]);  // if not null got labels for (int)min .. (int)max

    // for VST side:

    void    setValue(float value);
    float   getValue() const;

    void    getName(char *buf, size_t buf_size) const;
    void    getLabel(char *buf, size_t buf_size) const;
    void    getStrValue(char *buf, size_t buf_size) const;

    void    setEnable(bool enable); // if disabled - got "N/A" value

    float   getIncrement() const;

    // for processing side:

    double  getParam() const;
    int     getParamInt() const;

    bool    isMin() const;
    bool    isMax() const;

private:
    const char  *m_name;
    const char  *m_label;

    double  m_min;
    double  m_max;
    double  m_value;

    VstParam::Log  m_is_log;

    const char  *m_min_name;
    const char  *m_max_name;
    const char  **m_labels;

    int     m_precision;

    bool    m_enabled;

    static const double     m_lg_hz;
};

#endif
