#pragma once

#include <JuceHeader.h>
#include "WavetableSynthesizer.h"
#include "Sampler.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>


class NewProjectAudioProcessor : public juce::AudioProcessor {
public:
    NewProjectAudioProcessor();
    ~NewProjectAudioProcessor() override;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;
    void handleMidiEvent(const juce::MidiMessage& message);
    void renderAudio(juce::AudioBuffer<float>& synthBuffer, juce::AudioBuffer<float>& sampleBuffer, juce::AudioBuffer<float>& mainBuffer, float mixLevel, juce::MidiBuffer& midiMessages);
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;
    void initializeDSP();

    const juce::String getName() const override;
    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void applyDSP(juce::AudioBuffer<float>& buffer);
    void applyEffects(juce::AudioBuffer<float>& buffer);
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    void triggerNoteOn( int midiNoteNumber, float velocity);  // Make sure the parameters match.
   
    float getParameterValue(const juce::String& paramId) const {
        return *apvts.getRawParameterValue(paramId);
    }
    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

    
    
    // Setters for audio parameters (if needed)
    void setParameterValue(const juce::String& paramId, float value) {
        auto* param = apvts.getParameter(paramId);
        if (param) {
            param->setValueNotifyingHost(value);
        }
    }
    void updateSynthVoiceADSR(float attack, float decay, float sustain, float release);

    void initializeSampleDirectory();
    void scanSamplesDirectory(const juce::String& path);
    void loadSample(const juce::String& path);
    void setVolume(float volume);
    void setWaveform(int type);
    void setSynthVolume(float volume);
    void setSampleVolume(float volume);
    void setUnisonSize(int size);
    void setDetuneAmount(float amount);
    void setFilterCutoff(float cutoff);
    void setFilterResonance(float resonance);
    void setLFORate(float rate);
    void setLFODepth(float depth);
    void setReverbLevel(float level);
    void setChorusRate(float rate);
    float volume;
    juce::Reverb reverb;
    juce::dsp::Chorus<float> chorus;

    std::vector<juce::File> getSampleFiles() const;

private:
    juce::UndoManager undoManager;
    WavetableSynthesizer wavetableSynth;
    Sampler sampler;
    juce::AudioProcessorValueTreeState apvts;
    SynthVoice mySynthVoice;
    std::vector<SynthVoice> synthVoices;
    juce::AudioBuffer<float> sampleBuffer;
    std::vector<juce::File> sampleFiles;
    juce::File currentSampleFile;
    int samplePosition = 0;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessor)
};
