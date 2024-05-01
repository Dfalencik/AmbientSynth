#ifndef PLUGINEDITOR_H_INCLUDED
#define PLUGINEDITOR_H_INCLUDED

#include <JuceHeader.h>
#include "PluginProcessor.h"


class NewProjectAudioProcessor; // Forward declaration

class NewProjectAudioProcessorEditor : public juce::AudioProcessorEditor, public juce::Slider::Listener {
public:
    explicit NewProjectAudioProcessorEditor(NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;


    void setWaveform(int type);
    void setSynthVolume(float volume);
    void setSampleVolume(float volume);
    std::vector<juce::File> getSampleFiles() const;

    void updateSampleList(); // Function to populate sample choices
    void updateSynthWaveformList(); // Function to populate waveform choices

    // Setup functions for UI controls
    void setupUnisonControls();
    void setupFilterControls();
    void setupLFControls();
    void setupADSRControls();
    void setupEffectControls();
    void setupVolumeSlider();
    
    void setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text,
                         const juce::String& suffix, float start, float end, float interval, int yPos);

    const juce::AudioProcessorValueTreeState& getAPVTS() const;

private:
    NewProjectAudioProcessor& audioProcessor;

    // UI components
    juce::ComboBox sampleSelector;
    juce::ComboBox waveformSelector;
    juce::Slider synthVolumeSlider;
    juce::Slider sampleVolumeSlider;
    juce::Slider unisonSizeSlider;
    juce::Slider unisonDetuneSlider;
    juce::Slider filterCutoffSlider;
    juce::Slider filterResonanceSlider;
    juce::Slider lfoRateSlider;
    juce::Slider lfoDepthSlider;
    juce::Slider reverbLevelSlider;
    juce::Slider chorusRateSlider;

    // ADSR sliders
    juce::Slider attackSlider;
    juce::Slider decaySlider;
    juce::Slider sustainSlider;
    juce::Slider releaseSlider;
    juce::Slider volumeSlider;

    // Labels for sliders
    juce::Label unisonSizeLabel;
    juce::Label unisonDetuneLabel;
    juce::Label filterCutoffLabel;
    juce::Label filterResonanceLabel;
    juce::Label lfoRateLabel;
    juce::Label lfoDepthLabel;
    juce::Label attackLabel;
    juce::Label decayLabel;
    juce::Label sustainLabel;
    juce::Label releaseLabel;
    juce::Label reverbLevelLabel;
    juce::Label chorusRateLabel;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NewProjectAudioProcessorEditor)
};

#endif // PLUGINEDITOR_H_INCLUDED
