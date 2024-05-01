#include "WavetableSynthesizer.h"
#include "SynthVoice.h"
#include "PluginProcessor.h"
#include "Sampler.h"

WavetableSynthesizer::WavetableSynthesizer()
: masterVolume(1.0f), voices(16) {
    fillWavetable();
    for (auto& voice : voices) {
        voice = std::make_unique<SynthVoice>();
        voice->setWavetable(wavetables);
    }
}

WavetableSynthesizer::~WavetableSynthesizer() {}

void WavetableSynthesizer::prepareToPlay(double sampleRate, int samplesPerBlock) {
    for (auto& voice : voices) {
        voice->prepareToPlay(sampleRate, samplesPerBlock);
    }
}

void WavetableSynthesizer::releaseResources() {
    // Optional: Clean up resources if necessary
}

void WavetableSynthesizer::renderNextBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages, int startSample, int numSamples) {
    buffer.clear();

    for (int sample = 0; sample < numSamples; ++sample) {
        float mix = 0.0f;

        for (auto& voice : voices) {
            if (voice->isActive()) {
                mix += voice->getNextSample();
            }
        }
        
        mix *= masterVolume / static_cast<float>(voices.size());

        for (int channel = 0; channel < buffer.getNumChannels(); ++channel) {
            buffer.addSample(channel, startSample + sample, mix);
        }
    }
}



void WavetableSynthesizer::handleNoteOn(int noteNumber, float velocity) {
    for (auto& voice : voices) {
        if (!voice->isActive()) {
            voice->startNote(noteNumber, velocity);  // Use the simpler overload
            break;
        }
    }
}

void WavetableSynthesizer::handleNoteOff(int noteNumber, float velocity) {
    for (auto& voice : voices) {
        if (voice->getNoteNumber() == noteNumber && voice->isActive()) {
            voice->stopNote(true);  // `true` allows the note to tail off gracefully
        }
    }
}


void WavetableSynthesizer::setUnisonSize(int size) {
    for (auto& voice : voices) {
        voice->setUnisonSize(size);
    }
}

void WavetableSynthesizer::setDetuneAmount(float amount) {
    for (auto& voice : voices) {
        voice->setDetuneAmount(amount);
    }
}

void WavetableSynthesizer::setVolume(float volume) {
    masterVolume = std::clamp(volume, 0.0f, 1.0f);
}

void WavetableSynthesizer::setWaveform(Waveform newWaveform) {
    currentWaveform = newWaveform;
    fillWavetable();
}

void WavetableSynthesizer::fillWavetable() {
    wavetables[Sine].resize(tableSize);
    wavetables[Square].resize(tableSize);
    wavetables[Triangle].resize(tableSize);
    wavetables[Sawtooth].resize(tableSize);

    generateSineWave(wavetables[Sine]);
    generateSquareWave(wavetables[Square]);
    generateTriangleWave(wavetables[Triangle]);
    generateSawtoothWave(wavetables[Sawtooth]);
}

void WavetableSynthesizer::generateSineWave(std::vector<float>& table) {
    const double pi = juce::MathConstants<double>::pi;
    for (int i = 0; i < tableSize; ++i) {
        table[i] = std::sin(2.0 * pi * i / tableSize);
    }
}

void WavetableSynthesizer::generateSquareWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        table[i] = (i < tableSize / 2) ? 1.0f : -1.0f;
    }
}

void WavetableSynthesizer::generateTriangleWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        float phase = static_cast<float>(i) / static_cast<float>(tableSize);
        table[i] = 1.0f - 4.0f * std::abs(std::round(phase) - phase);
    }
}


void WavetableSynthesizer::generateSawtoothWave(std::vector<float>& table) {
    for (int i = 0; i < tableSize; ++i) {
        table[i] = 2.0f * (i / float(tableSize)) - 1.0f;
    }
}
