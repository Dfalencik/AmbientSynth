#include "PluginProcessor.h"
#include "PluginEditor.h"

NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor(NewProjectAudioProcessor& p)
    : AudioProcessorEditor(p), audioProcessor(p) {
    setSize(400, 500);
    setupADSRControls();
    setupUnisonControls();
    setupFilterControls();
    setupLFControls();
    setupVolumeSlider();
    setupEffectControls();
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor() {
    // Clean up any resources if necessary
}

std::vector<juce::File> NewProjectAudioProcessorEditor::getSampleFiles() const {
    juce::File sampleDirectory("MacintoshHD/Users/denisfalencik/desktop/XCODE/TRIAL/NewProject/SAMPLES"); // Specify the correct path
    std::vector<juce::File> sampleFiles;
    for (juce::DirectoryIterator iter(sampleDirectory, false, "*.wav", juce::File::findFiles); iter.next();) {
        sampleFiles.push_back(iter.getFile());
    }
    return sampleFiles;
}


void NewProjectAudioProcessorEditor::setupADSRControls() {
    const int startY = 310;  // Starting Y position
    const int spacing = 45;  // Space between controls

    setupSlider(attackSlider, attackLabel, "Attack", " s", 0.01, 5.0, 0.01, startY);
    setupSlider(decaySlider, decayLabel, "Decay", " s", 0.01, 3.0, 0.01, startY + spacing);
    setupSlider(sustainSlider, sustainLabel, "Sustain", "", 0.0, 1.0, 0.01, startY + 2 * spacing);
    setupSlider(releaseSlider, releaseLabel, "Release", " s", 0.01, 5.0, 0.01, startY + 3 * spacing);
}

void NewProjectAudioProcessorEditor::setupEffectControls() {
    const int startY = 350; // Example Y position for these controls
    setupSlider(reverbLevelSlider, reverbLevelLabel, "Reverb Level", "", 0.0, 1.0, 0.01, startY);
    setupSlider(chorusRateSlider, chorusRateLabel, "Chorus Rate", " Hz", 0.1, 5.0, 0.01, startY + 45);
}

void NewProjectAudioProcessorEditor::setupUnisonControls() {
    const int startY = 400;
    setupSlider(unisonSizeSlider, unisonSizeLabel, "Unison Size", " Voices", 1, 8, 1, startY);
    setupSlider(unisonDetuneSlider, unisonDetuneLabel, "Unison Detune", " Semitones", 0.0, 0.5, 0.01, startY + 45);
}

void NewProjectAudioProcessorEditor::setupFilterControls() {
    const int startY = 500;
    setupSlider(filterCutoffSlider, filterCutoffLabel, "Filter Cutoff", " Hz", 20.0, 20000.0, 1.0, startY);
    setupSlider(filterResonanceSlider, filterResonanceLabel, "Filter Resonance", "", 0.1, 10.0, 0.1, startY + 45);
}

void NewProjectAudioProcessorEditor::setupLFControls() {
    const int startY = 600;
    setupSlider(lfoRateSlider, lfoRateLabel, "LFO Rate", " Hz", 0.1, 20.0, 0.1, startY);
    setupSlider(lfoDepthSlider, lfoDepthLabel, "LFO Depth", "", 0.0, 1.0, 0.01, startY + 45);
}

void NewProjectAudioProcessorEditor::setupVolumeSlider() {
    volumeSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 90, 0);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);
}

void NewProjectAudioProcessorEditor::setupSlider(juce::Slider& slider, juce::Label& label, const juce::String& text, const juce::String& suffix, float start, float end, float interval, int yPos) {
    const int startX = 10;  // Adjust positioning as necessary
    slider.setBounds(startX, yPos, getWidth() - 20, 20);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setRange(start, end, interval);
    slider.setTextValueSuffix(suffix);
    slider.addListener(this);
    addAndMakeVisible(slider);

    label.setText(text, juce::dontSendNotification);
    label.attachToComponent(&slider, true);
    label.setBounds(startX, yPos - 20, getWidth() - 20, 20);
    addAndMakeVisible(label);

    auto param = audioProcessor.getAPVTS().getRawParameterValue(text);
    if (param) {
        slider.setValue(*param, juce::dontSendNotification);
    } else {
        // Handle the case where the parameter is not found
        std::cerr << "Parameter " << text << " not found." << std::endl;
    }
}
void NewProjectAudioProcessorEditor::updateSampleList() {
    // Implementation to populate the sample selector with available samples
    auto files = getSampleFiles();
}

void NewProjectAudioProcessorEditor::sliderValueChanged(juce::Slider* slider) {
    if (slider == &volumeSlider) {
        audioProcessor.setVolume(slider->getValue());
    } else if (slider == &reverbLevelSlider) {
        // Example implementation
        audioProcessor.setReverbLevel(slider->getValue());
    } else if (slider == &chorusRateSlider) {
        // Example implementation
        audioProcessor.setChorusRate(slider->getValue());
    }
    // Add further conditions for other sliders as needed
}



void NewProjectAudioProcessorEditor::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::grey);
    g.setFont(juce::Font(15.0f));
    g.setColour(juce::Colours::white);
    g.drawText("Audio Processor Editor", getLocalBounds(), juce::Justification::centred, true);
}

void NewProjectAudioProcessorEditor::resized() {
    // This function can be used to rearrange components when the editor resizes
}
