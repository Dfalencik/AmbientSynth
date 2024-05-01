#include "SynthVoice.h"
#include <JuceHeader.h>

SynthVoice::SynthVoice()
    : active(false), frequency(440.0f), midiNoteNumber(-1),
      currentSampleIndex(0.0), increment(0.0), velocity(0.0),
      amplitude(1.0), currentWaveform(0), unisonSize(1), detuneAmount(0.0f),
filter(), lfoPhase(0.0f), lfoRate(0.1f), lfoDepth(10.0f),
baseCutoffFrequency(2000.0f) {  // Initialize filter object directly

    juce::dsp::ProcessSpec spec;
        spec.sampleRate = sampleRate;
        spec.maximumBlockSize = 512;
        spec.numChannels = 1;

        filter.prepare(spec);
        updateFilter();

        initializeWavetable();
}

void SynthVoice::initializeWavetable() {
    // Correct initialization for wavetable array
    for (auto& table : wavetable) {
        table.assign(wavetableSize, 0.0f);  // Assign zeroes initially or use a loop to fill with sin values
        for (size_t i = 0; i < wavetableSize; ++i) {
            table[i] = std::sin(2.0 * M_PI * i / wavetableSize);
        }
    }
}

void SynthVoice::setWavetable(const std::array<std::vector<float>, 4>& newWavetable) {
    wavetable = newWavetable;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity) {
    this->midiNoteNumber = midiNoteNumber;
    this->velocity = velocity;
    
    // Assuming `attack` and `release` are class members initialized properly
    // If not, you might need to define them as parameters or fixed values
    float attack = 0.5f;  // Example value for attack
    float release = 0.5f;  // Example value for release

    // Set ADSR parameters
    adsr.setParameters({attack, 0.1f, 0.8f, release}); // Fixed decay of 0.1 and sustain of 0.8

    adsr.noteOn();
    active = true;
}





void SynthVoice::stopNote(bool allowTailOff) {
    if (allowTailOff) {
        // Implement note release logic
    } else {
        this->active = false;
        adsr.reset();
    }
}

int SynthVoice::getNoteNumber() const {
    return midiNoteNumber;
}

void SynthVoice::updateFilter() {
    float modulatedCutoff = baseCutoffFrequency + std::sin(lfoPhase) * lfoDepth;
    modulatedCutoff = std::clamp(modulatedCutoff, 20.0f, 20000.0f);
    auto coeffs = juce::dsp::IIR::Coefficients<float>::makeLowPass(getSampleRate(), modulatedCutoff, 1.0f);
    filter.coefficients = *coeffs;
    lfoPhase += lfoRate * (1.0f / getSampleRate());
    if (lfoPhase > 2.0 * M_PI) {
        lfoPhase -= 2.0 * M_PI;
    }
}


void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock) {
    // Update the internal sample rate stored in the class
    this->sampleRate = sampleRate;  // Assuming you have a member variable 'sampleRate' to store the current rate

    // Setup the process specification with the current host sample rate and block size
    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 1;  // Assuming mono processing, adjust if stereo or more channels are required

    // Prepare the filter with the specified processing configuration
    filter.prepare(spec);

    // Optionally, you could reinitialize or update other processing blocks here
    initializeWavetable();  // Reinitialize the wavetable if needed

    // Debug output to confirm the settings (you can remove this line in production)
    DBG("SynthVoice prepared: Sample Rate = " << sampleRate << ", Max Block Size = " << samplesPerBlock);
}

float SynthVoice::getNextSample() {
    float sample = 0.0f;
    if (active) {
        updateFilter(); // Ensure filter is updated each sample

        for (int i = 0; i < unisonSize; ++i) {
            float detuneFactor = detuneOffsets.size() > i ? detuneOffsets[i] : 1.0f;
            int index = static_cast<int>(fmod(currentSampleIndex * detuneFactor, wavetableSize));
            float rawSample = wavetable[currentWaveform][index];

            // Prepare for processing
            juce::AudioBuffer<float> buffer(1, 1); // Create a buffer with 1 channel and 1 sample
            buffer.setSample(0, 0, rawSample); // Set the sample in the buffer

            // Wrap the buffer into an AudioBlock
            juce::dsp::AudioBlock<float> block(buffer);
            juce::dsp::ProcessContextReplacing<float> context(block);
            filter.process(context);

            // Retrieve the processed sample
            sample += buffer.getSample(0, 0);
            currentSampleIndex += increment;
            if (currentSampleIndex >= wavetableSize) currentSampleIndex -= wavetableSize;
        }
        sample /= unisonSize;
    }
    return sample;
}


bool SynthVoice::isActive() const {
    return active;
}

void SynthVoice::setUnisonSize(int size) {
    unisonSize = size;
    calculateDetuneOffsets();
}

void SynthVoice::setDetuneAmount(float detune) {
    detuneAmount = detune;
    calculateDetuneOffsets();
}

void SynthVoice::calculateDetuneOffsets() {
    detuneOffsets.resize(unisonSize);
    for (int i = 0; i < unisonSize; ++i) {
        float offset = (i - unisonSize / 2) * detuneAmount;
        detuneOffsets[i] = pow(2.0f, offset / 12.0f);
    }
}

float SynthVoice::midiNoteToFrequency(int midiNoteNumber) const {
    return 440.0 * pow(2.0, (midiNoteNumber - 69) / 12.0);
}

float SynthVoice::getSampleRate() const {
    return 48000.0f; // Replace with the actual sample rate from your audio engine context
}

void SynthVoice::clearCurrentNote() {
    midiNoteNumber = -1; // Reset the MIDI note number
}

int SynthVoice::getCurrentlyPlayingNote() const {
    return midiNoteNumber; // Return the currently active MIDI note number
}

void SynthVoice::handleNoteOff(int midiChannel, int midiNoteNumber, float velocity, bool allowTailOff) {
    if (midiNoteNumber == getCurrentlyPlayingNote()) {
        if (allowTailOff) {
            adsr.noteOff();
        } else {
            clearCurrentNote();
            adsr.reset();
        }
    }
}


