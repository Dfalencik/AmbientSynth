#pragma once

#include <JuceHeader.h>
#include "SynthVoice.h"

class WavetableSynthesizer {
public:
    enum Waveform {
        Sine, Square, Triangle, Sawtooth, NumWaveforms
    };

    WavetableSynthesizer();
    ~WavetableSynthesizer();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    void renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples);
    void handleNoteOn(int noteNumber, float velocity);
    void setVolume(float volume);
    void setWaveform(Waveform newWaveform);
    float getNextSample();
    void setUnisonSize(int size);
    void setDetuneAmount(float amount);
    void handleNoteOff(int noteNumber, float velocity);
    
private:
    float masterVolume;
    double currentSampleRate;
    std::vector<std::unique_ptr<SynthVoice>> voices;
    std::array<std::vector<float>, NumWaveforms> wavetables;
    Waveform currentWaveform;

    void fillWavetable();
    void generateSineWave(std::vector<float>& table);
    void generateSquareWave(std::vector<float>& table);
    void generateTriangleWave(std::vector<float>& table);
    void generateSawtoothWave(std::vector<float>& table);

    static constexpr int tableSize = 1024;  // Size of each waveform table
};
