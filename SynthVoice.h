#pragma once

#include <JuceHeader.h>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

class SynthVoice {
public:
    SynthVoice();
    ~SynthVoice() = default;

    void initializeWavetable();
    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void startNote(int midiNoteNumber, float velocity);
    void stopNote(bool allowTailOff);
    int getNoteNumber() const;
    float getNextSample();
    bool isActive() const;

    void setUnisonSize(int size);
    void setDetuneAmount(float detune);
    void setWavetable(const std::array<std::vector<float>, 4>& newWavetable);
    void updateFilter();
    void clearCurrentNote();

    void handleNoteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff);

        // Declarations for missing identifiers
        int getCurrentlyPlayingNote() const;  // Assuming it returns an int
                  // Assuming no return and no parameters

    // Update the ADSR parameters and re-apply to the ADSR envelope
    void updateADSR(float attack, float decay, float sustain, float release) {
        adsrParams = {attack, decay, sustain, release};
        adsr.setParameters(adsrParams);
    }

    float lfoValue() const;  // Calculates and returns the current LFO value based on the phase

private:
    float midiNoteToFrequency(int midiNoteNumber) const;
    float getSampleRate() const;

    bool active;
    float frequency;
    int midiNoteNumber;
    double currentSampleIndex;
    double increment;
    float velocity;
    float amplitude;
    int currentWaveform;
    std::array<std::vector<float>, 4> wavetable;
    static constexpr size_t wavetableSize = 2048;
    std::vector<float> detuneOffsets;
    int unisonSize;
    float detuneAmount;
    int currentNote;

    // ADSR envelope and parameters
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    // DSP related members
    juce::dsp::IIR::Filter<float> filter;
    float lfoPhase;
    float lfoRate;
    float lfoDepth;
    float baseCutoffFrequency;

    float sampleRate;  // Dynamic sample rate used across the class

    void calculateDetuneOffsets();

    // This ensures that all DSP objects use the most current sample rate
    void updateDSPBlockSizeAndRate();
};
