// $Id$

#ifndef _moloteng_h_
#define _moloteng_h_

#define INPUT_DITHER        24      // bit
#define OUTPUT_DITHER       20      // bit

// mono compressor with sidechain
class ChannelCompressor
{
public:
    ChannelCompressor() {}

    static const unsigned int   MAX_OVERSAMPLING = 8;
};

/*
 * StereoCompressor
 */

class StereoCompressor
{
public:
    StereoCompressor() {}
    ~StereoCompressor() {}

    enum stereo_mode_t {
        SL_MONO,
        SL_2MONO,
        SL_R_SCHN,
        SL_M,
        SL_S,
        SL_MS,
        SL_STEREO
    };

    void    setEnvelope(double sample_rate, double attack_s, double release_s, double sidechain_freq, bool sharp_mode) {}
    void    setEnvelopeK(double threshold, double knee, double ratio) {}
    void    setGain(double gain, double dry_mix, double in_gain = 1.0) {}
    void    setSat(double threshold) {}
    void    setMidScoop(double mid_scoop, double sample_rate) {}
    void    setStereoMode(stereo_mode_t sl) {}
    void    setOversampling(unsigned int oversampling) {}
    void    setLimiter(double threshold) {}

    void    processSample(double x1, double x2, double *y1, double *y2) {}
    void    reset() {}

    enum limiter_mode_t {
        LM_PRE = 0,     // limiter -> master gain
        LM_POST,        // master gain -> limiter
        LM_OFF
    };

    void    setUseSat(bool use) {}
    void    setLimiterMode(limiter_mode_t mode) {}
    void    setBypass(bool bypass) {}
    void    setDithering(unsigned int n_bits) {}

    // levels
    void    setVU(bool is_gr) {}
    double  getVU() const { return 0.0; }

    double  getLimiting() const { return 0.0; }

    bool    getClip(bool reset) { return false; }
    bool    isOn() const { return true; }

};

#endif
