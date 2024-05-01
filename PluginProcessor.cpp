#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "Sampler.h"
#include "SynthVoice.h"
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>

// Constructor
NewProjectAudioProcessor::NewProjectAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, &undoManager, "Parameters", createParameterLayout())
{
    initializeDSP();
}
// Parameter layout setup
juce::AudioProcessorValueTreeState::ParameterLayout NewProjectAudioProcessor::createParameterLayout() {
    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("filterCutoff", "Filter Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f), 2000.0f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("filterResonance", "Filter Resonance", juce::NormalisableRange<float>(0.1f, 10.0f), 1.0f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("lfoRate", "LFO Rate", juce::NormalisableRange<float>(0.1f, 20.0f), 5.0f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("lfoDepth", "LFO Depth", juce::NormalisableRange<float>(0.0f, 1.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("attack", "Attack", juce::NormalisableRange<float>(0.1f, 5.0f), 0.5f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("decay", "Decay", juce::NormalisableRange<float>(0.1f, 5.0f), 1.0f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("sustain", "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f), 0.8f));
    layout.add(std::make_unique<juce::AudioProcessorValueTreeState::Parameter>("release", "Release", juce::NormalisableRange<float>(0.1f, 5.0f), 1.5f));

    return layout;
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
    NewProjectAudioProcessor* processor = nullptr;
    try {
        processor = new NewProjectAudioProcessor();
        if (processor) {
            // Optionally, perform any immediate initialization checks here
            DBG("Plugin created successfully.");
        } else {
            DBG("Failed to create plugin processor.");
        }
    } catch (const std::exception& e) {
        DBG("Exception during plugin creation: " + juce::String(e.what()));
        // Clean up if necessary
        delete processor;
        processor = nullptr;
    } catch (...) {
        DBG("Unknown exception during plugin creation.");
        // Ensure no partial objects are left
        delete processor;
        processor = nullptr;
    }
    return processor; // This could return nullptr if initialization fails.
}
// Initial setup for sample directories
void NewProjectAudioProcessor::initializeSampleDirectory() {
    juce::File appDataDir = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    juce::File sampleDir = appDataDir.getChildFile("NewProject/SAMPLES");
    if (!sampleDir.exists()) {
        sampleDir.createDirectory();
    }
}

// Destructor
NewProjectAudioProcessor::~NewProjectAudioProcessor() {
    // Cleanup if necessary
}

// Implementation of virtual methods from AudioProcessor
const juce::String NewProjectAudioProcessor::getName() const {
    return "NewProjectAudioProcessor";
}

bool NewProjectAudioProcessor::acceptsMidi() const {
    return true;
}

bool NewProjectAudioProcessor::producesMidi() const {
    return false;
}

bool NewProjectAudioProcessor::isMidiEffect() const {
    return false;
}

double NewProjectAudioProcessor::getTailLengthSeconds() const {
    return 0.0;
}

int NewProjectAudioProcessor::getNumPrograms() {
    return 1;
}

int NewProjectAudioProcessor::getCurrentProgram() {
    return 0;
}

void NewProjectAudioProcessor::setCurrentProgram(int index) {
}

const juce::String NewProjectAudioProcessor::getProgramName(int index) {
    return {};
}

void NewProjectAudioProcessor::changeProgramName(int index, const juce::String& newName) {
}

bool NewProjectAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
    if (layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() &&
        (layouts.getMainInputChannelSet() == juce::AudioChannelSet::disabled() ||
         layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo())) {
        return true;
    }
    return false;
}

void NewProjectAudioProcessor::getStateInformation(juce::MemoryBlock& destData) {
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void NewProjectAudioProcessor::setStateInformation(const void* data, int sizeInBytes) {
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType())) {
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
    }
}







void NewProjectAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Assuming chorus and reverb are class members of type juce::dsp::Processor or similar.
    chorus.prepare({sampleRate, (juce::uint32)samplesPerBlock, 2}); // Assuming stereo processing
    reverb.setSampleRate(sampleRate);
    wavetableSynth.prepareToPlay(sampleRate, samplesPerBlock);
    sampler.prepareToPlay(sampleRate, samplesPerBlock);
}


void NewProjectAudioProcessor::releaseResources() {
    wavetableSynth.releaseResources();
    sampler.releaseResources();
}

void NewProjectAudioProcessor::setWaveform(int type) {
    if (type >= 0 && type < static_cast<int>(WavetableSynthesizer::Waveform::NumWaveforms)) {
        wavetableSynth.setWaveform(static_cast<WavetableSynthesizer::Waveform>(type));
    } else {
        DBG("Invalid waveform type specified");
    }
}

void NewProjectAudioProcessor::setSynthVolume(float volume) {
    wavetableSynth.setVolume(volume);
}

void NewProjectAudioProcessor::setSampleVolume(float volume) {
    sampler.setVolume(volume);
}

void NewProjectAudioProcessor::setUnisonSize(int size) {
    wavetableSynth.setUnisonSize(size);  
}

void NewProjectAudioProcessor::setDetuneAmount(float amount) {
    wavetableSynth.setDetuneAmount(amount);
}

// Ensure all parameters are retrieved safely.
void NewProjectAudioProcessor::setFilterCutoff(float cutoff) {
    auto* param = apvts.getParameter("filterCutoff");
    if (param) {
        const float normalizedCutoff = param->convertTo0to1(juce::jlimit(20.0f, 20000.0f, cutoff));
        param->setValueNotifyingHost(normalizedCutoff);
    } else {
        DBG("Filter Cutoff parameter not found!");
    }
}


void NewProjectAudioProcessor::setLFORate(float rate) {
    auto& parameter = *apvts.getParameter("lfoRate");
    parameter.setValueNotifyingHost(parameter.convertTo0to1(rate));
}

void NewProjectAudioProcessor::setLFODepth(float depth) {
    // Retrieve the parameter from the AudioProcessorValueTreeState by ID
    auto* param = apvts.getParameter("lfoDepth");
    if (param) {
        // Convert the actual value to a normalized range (0 to 1) if necessary and set it
        param->setValueNotifyingHost(param->convertTo0to1(depth));
    }
}

void NewProjectAudioProcessor::setFilterResonance(float resonance) {
    // Retrieve the parameter from the AudioProcessorValueTreeState by ID
    auto* param = apvts.getParameter("filterResonance");
    if (param) {
        // Convert the actual value to a normalized range (0 to 1) if necessary and set it
        param->setValueNotifyingHost(param->convertTo0to1(resonance));
    }
}


// Implementation
void NewProjectAudioProcessor::triggerNoteOn(int noteNumber, float velocity) {
    for (auto& voice : synthVoices) {
        if (!voice.isActive()) {
            voice.startNote(noteNumber, velocity); // Example parameters for attack and release
            break;
        }
    }
}


void NewProjectAudioProcessor::updateSynthVoiceADSR(float attack, float decay, float sustain, float release) {
    // Loop through each voice and update ADSR settings
    for (auto& voice : synthVoices) {  // Use reference to avoid pointer if not needed
        voice.updateADSR(attack, decay, sustain, release);
    }
}

std::vector<juce::File> NewProjectAudioProcessor::getSampleFiles() const {
    return sampleFiles;
}

// This function processes the audio block
void NewProjectAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) {
    // Retrieve mix parameter value safely
    auto* mixParam = apvts.getRawParameterValue("mix");
    float mixLevel = mixParam ? mixParam->load() : 0.5f;

    buffer.clear();
    
    
    // Create additional buffers for synthesizer and sampler output
    juce::AudioBuffer<float> synthBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> sampleBuffer(buffer.getNumChannels(), buffer.getNumSamples());

    // Clear the additional buffers to ensure they start with a clean state
    synthBuffer.clear();
    sampleBuffer.clear();

    // Process MIDI messages and render audio from synth and sampler
    for (const auto midiMessage : midiMessages) {
        const auto message = midiMessage.getMessage();
        handleMidiEvent(message);
    }

    wavetableSynth.renderNextBlock(synthBuffer, midiMessages, 0, synthBuffer.getNumSamples());
    sampler.renderNextBlock(sampleBuffer, midiMessages, 0, sampleBuffer.getNumSamples());

    // Mix down synth and sample buffers to the main buffer using the mix level
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        buffer.addFrom(channel, 0, synthBuffer, channel, 0, buffer.getNumSamples(), mixLevel);
        buffer.addFrom(channel, 0, sampleBuffer, channel, 0, buffer.getNumSamples(), 1.0f - mixLevel);
    }

    // Apply DSP effects like chorus and reverb
    applyDSP(buffer);
}

void NewProjectAudioProcessor::applyDSP(juce::AudioBuffer<float>& buffer) {
    // Apply chorus
    juce::dsp::AudioBlock<float> block(buffer);
    chorus.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Apply reverb
    if (buffer.getNumChannels() >= 2) {
        // Assuming stereo processing is appropriate
        for (int channel = 0; channel < buffer.getNumChannels(); channel += 2) {
            reverb.processStereo(buffer.getWritePointer(channel), buffer.getWritePointer(channel + 1), buffer.getNumSamples());
        }
    } else {
        // For mono compatibility
        reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
    }
}
    

void NewProjectAudioProcessor::applyEffects(juce::AudioBuffer<float>& buffer) {
    // Process block with chorus effect
    juce::dsp::AudioBlock<float> block(buffer);
    chorus.process(juce::dsp::ProcessContextReplacing<float>(block));

    // Apply reverb to each channel individually
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
        float* channelData = buffer.getWritePointer(channel);
        reverb.processStereo(channelData, channelData, buffer.getNumSamples());
    }
}



void NewProjectAudioProcessor::handleMidiEvent(const juce::MidiMessage& message) {
    if (message.isNoteOn()) {
        int noteNumber = message.getNoteNumber();
        float velocity = message.getFloatVelocity();
        wavetableSynth.handleNoteOn(noteNumber, velocity);
        sampler.handleNoteOn(noteNumber, velocity);
    } else if (message.isNoteOff()) {
        int noteNumber = message.getNoteNumber();
        float velocity = message.getFloatVelocity();
        wavetableSynth.handleNoteOff(noteNumber, velocity);
        sampler.handleNoteOff(noteNumber, velocity);
    }
    // Add additional handling for other types of MIDI messages if needed
}



void NewProjectAudioProcessor::scanSamplesDirectory(const juce::String& path) {
    juce::File directory(path);
    if (directory.exists() && directory.isDirectory()) {
        // Initialize the ranged directory iterator to iterate through WAV files
        juce::RangedDirectoryIterator iter(directory, true, "*.wav");

        // Use a range-based for loop to process each file
        for (auto& entry : iter) {
            const auto& file = entry.getFile();  // Access the file from the DirectoryEntry object
            if (file.existsAsFile()) { // Ensure the item is a file
                sampleFiles.push_back(file); // Add the file to the list
            }
        }
    }
}

void NewProjectAudioProcessor::loadSample(const juce::String& path) {
    juce::File file(path);
    if (!file.existsAsFile()) {
        DBG("File does not exist: " + path);
        return;
    }

    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();
    std::unique_ptr<juce::AudioFormatReader> reader(formatManager.createReaderFor(file));

    if (reader) {
        sampleBuffer.setSize(reader->numChannels, static_cast<int>(reader->lengthInSamples));
        reader->read(&sampleBuffer, 0, static_cast<int>(reader->lengthInSamples), 0, true, true);
        samplePosition = 0;
    } else {
        DBG("Failed to read sample");
    }
}


juce::AudioProcessorEditor* NewProjectAudioProcessor::createEditor() {
    return new NewProjectAudioProcessorEditor(*this);
}

void NewProjectAudioProcessor::setVolume(float volume) {
    // Implementation might involve setting a volume parameter or directly adjusting an audio buffer
    this->volume = volume;  // Assume 'volume' is a float member variable of NewProjectAudioProcessor
}

void NewProjectAudioProcessor::setReverbLevel(float level) {
    // Get a modifiable copy of the current reverb parameters
    juce::Reverb::Parameters params = reverb.getParameters();

    // Modify the roomSize parameter
    params.roomSize = level;

    // Set the modified parameters back to the reverb
    reverb.setParameters(params);
}


void NewProjectAudioProcessor::setChorusRate(float rate) {
    chorus.setRate(rate);
}

bool NewProjectAudioProcessor::hasEditor() const {
    return true;
}

void NewProjectAudioProcessor::initializeDSP() {
    // Example setup of your DSP components, such as initializing your filters, reverbs, etc.
    DBG("DSP components are initialized.");
}



