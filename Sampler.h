#pragma once

#include <JuceHeader.h>
#include <atomic>

class Sampler {
public:
    Sampler();
    ~Sampler();

    void prepareToPlay(double sampleRate, int samplesPerBlock);
    void releaseResources();
    void loadSample(const juce::String& path);
    void setVolume(float newVolume);
    void handleNoteOn(int midiNoteNumber, float velocity);
    void handleNoteOff(int midiNoteNumber, float velocity);
    void renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples);

private:
    juce::Synthesiser synth;
    juce::MidiMessageCollector midiCollector;
    std::unique_ptr<juce::AudioFormatReader> formatReader;
    juce::AudioFormatManager formatManager;
    std::atomic<float> volume{1.0f};  // Initialize volume to default
};
