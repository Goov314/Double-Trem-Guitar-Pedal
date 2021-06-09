#include "daisy_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;

bool second = false;
bool single = false;


class Terrarium
{
    public:
	enum Sw
	    {
	     FOOTSWITCH_1 = 4,
	     FOOTSWITCH_2 = 5,
	     SWITCH_1 = 2,
	     SWITCH_2 = 1,
	     SWITCH_3 = 0,
	     SWITCH_4 = 6
	    };
	
	enum Knob
	    {
	     KNOB_1 = 0,
	     KNOB_2 = 2,
	     KNOB_3 = 4,
	     KNOB_4 = 1,
	     KNOB_5 = 3,
	     KNOB_6 = 5
	    };
	enum LED
	    {
	     LED_1 = 22,
	     LED_2 = 23
	    };
};

static Parameter depth1, depth2, freq1, freq2, vol;

float sample_rate;

DaisyPetal hw;
Led led_1, led_2;
Oscillator osc;
Tremolo    trem;
Tremolo    trem2;

void AudioCallback(float **in, float **out, size_t size)
{
    hw.ProcessAllControls();
    trem.SetFreq(freq1.Process());
    trem.SetDepth(depth1.Process());

    trem2.SetFreq(freq2.Process());
    trem2.SetDepth(depth2.Process());
   
    if (hw.switches[Terrarium::SWITCH_1].Pressed()) {
        trem.SetWaveform(Oscillator::WAVE_SQUARE);
    } else {
        if (hw.switches[Terrarium::SWITCH_2].Pressed()) {
            trem.SetWaveform(Oscillator::WAVE_SAW);
        } else {
            trem.SetWaveform(Oscillator::WAVE_SIN);
        }
    }

    if (hw.switches[Terrarium::SWITCH_3].Pressed()) {
        trem2.SetWaveform(Oscillator::WAVE_SQUARE);
    } else {
        if (hw.switches[Terrarium::SWITCH_4].Pressed()) {
            trem2.SetWaveform(Oscillator::WAVE_SAW);
        } else {
            trem2.SetWaveform(Oscillator::WAVE_SIN);
        }
    }

    if (hw.switches[Terrarium::FOOTSWITCH_2].RisingEdge())
    {
        second = !second;
        led_2.Set(second? 1.0f : 0.0f);
    } 

    if (hw.switches[Terrarium::FOOTSWITCH_1].RisingEdge())
    {
        single = !single;
        led_1.Set(single? 1.0f : 0.0f);
    }
    for(size_t i = 0; i < size; i++)
    {
        float value = in[0][i];
        if (single) {
           value = trem.Process(value);
        }
        if (second){
           value = trem2.Process(value);
        }
        value = value * vol.Process();
        out[0][i] = out[1][i] = value;
    }

    led_1.Update();
    led_2.Update();
}

int main(void)
{
    hw.Init();
    hw.SetAudioBlockSize(12);
    float sample_rate = hw.AudioSampleRate();

    depth1.Init(hw.knob[Terrarium::KNOB_1], 0.f, 1.f, depth1.LINEAR);
    depth2.Init(hw.knob[Terrarium::KNOB_4], 0.f, 1.f, depth2.LINEAR);
    freq1.Init(hw.knob[Terrarium::KNOB_2], 0.5f, 10.f, freq1.LINEAR);
    freq2.Init(hw.knob[Terrarium::KNOB_5], 0.5f, 10.f, freq1.LINEAR);
    vol.Init(hw.knob[Terrarium::KNOB_3], 0.5f, 2.f, vol.LINEAR);

    trem.Init(sample_rate);
    trem2.Init(sample_rate);

    
    
    trem.SetFreq(2.f);
    trem.SetDepth(1.f);

    trem2.SetFreq(5.f);
    trem2.SetDepth(1.f);

    led_1.Init(hw.seed.GetPin(Terrarium::LED_1), false);
    led_2.Init(hw.seed.GetPin(Terrarium::LED_2), false);

    hw.StartAdc();
    hw.StartAudio(AudioCallback);
    while(1) {}
}
