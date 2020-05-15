// $Id$

#include "system.h"

#include "vstparam.h"

/*
 * VstParam
 */

const double VstParam::m_lg_hz = 1000.0;

VstParam::VstParam(const char *name,
    const char *label,
    double min,
    double max,
    double def,
    enum VstParam::Log is_log,
    const char *min_name,
    const char *max_name,
    int precision,
    const char *labels[])
{
    m_name = name;
    m_label = label;
    m_min = min;
    m_max = max;
    m_value = def;
    m_is_log = is_log;
    m_min_name = min_name;
    m_max_name = max_name;
    m_precision = precision;
    m_labels = labels;

    m_enabled = true;
}

void
VstParam::setValue(float value)
{
    double k;

    k = (double)value;

    if (k == 0.0)
        m_value = m_min;
    else if (k == 1.0)
        m_value = m_max;
    else {

        if (m_is_log == L_HZ || m_is_log == L_KHZ)
            k = pow(m_lg_hz, k) / m_lg_hz;
        else if (m_is_log == L_BOOL)
            k = k < 0.5 ? 0.0 : 1.0;

        // convert k [0..1] to our m_value
        m_value = k * (m_max - m_min) + m_min;

    }
}

float
VstParam::getValue() const
{
    double k;

    // convert our m_value to k [0..1]
    k = (m_value - m_min) / (m_max - m_min);

    if (m_is_log == L_HZ || m_is_log == L_KHZ)
        k = log(k * m_lg_hz) / log(m_lg_hz);

    return (float)k;
}

void
VstParam::getName(char *buf, size_t buf_size) const
{
    strncpy(buf, m_name, buf_size);
    buf[buf_size - 1] = '\0';
}

void
VstParam::getLabel(char *buf, size_t buf_size) const
{
    if (m_label != NULL) {
        strncpy(buf, m_label, buf_size);
        buf[buf_size - 1] = '\0';
    
    } else
        buf[0] = '\0';
}

void
VstParam::getStrValue(char *buf, size_t buf_size) const
{
    if (m_enabled) {
        if (m_labels != NULL) {
            // use labels
            strncpy(buf, m_labels[(int)floor(m_value - m_min)], buf_size);

        } else {
            if (m_value == m_min && m_min_name != NULL) {
                // use min_name
                strncpy(buf, m_min_name, buf_size);

            } else if (m_value == m_max && m_max_name != NULL) {
                // use max_name
                strncpy(buf, m_max_name, buf_size);

            } else {
                // use value with given precision
                if (m_precision == 0)
                    snprintf(buf, buf_size, "%d", (int)floor(m_value + 0.5));
                else {
                    snprintf(buf, buf_size, "%.*f", m_precision, m_value);
                }
            }
        }
    } else {
        // disabled
        strncpy(buf, "N/A", buf_size);
    }

    buf[buf_size - 1] = '\0';
}

void
VstParam::setEnable(bool enable)
{
    m_enabled = enable;
}

double
VstParam::getParam() const
{
    double param = m_value;

    if (m_is_log == L_DB) {
        // param in dB: convert dB to K
        param = DB_TO_K(param);
    
    } else if (m_is_log == L_KHZ) {
        // convert to hz
        param *= 1000.0;
    }

    return param;
}

int
VstParam::getParamInt() const
{
    // as is
    return (int)floor(m_value);
}

bool
VstParam::isMin() const
{
    return (m_value <= m_min);
}

bool
VstParam::isMax() const
{
    return (m_value >= m_max);
}

float
VstParam::getIncrement() const
{
    double n_steps = (m_max - m_min) * pow(10.0, (double)m_precision) + 1.0;

    while (n_steps > 100.0)
        n_steps /= 2.0;

    return (float)(1.0 / n_steps);
}
