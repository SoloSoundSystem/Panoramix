#pragma once
#include <JuceHeader.h>

class PanoramizatorAudioProcessor : public juce::AudioProcessor
{
public:
    PanoramizatorAudioProcessor();
    ~PanoramizatorAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return "Panoramizator"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void chooseNextTarget();

    double currentSampleRate = 44100.0;
    float currentPan = 0.0f;
    float startPan = 0.0f;
    float targetPan = 0.0f;
    int samplesIntoRamp = 0;
    int rampLengthSamples = 44100;
    juce::Random random;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanoramizatorAudioProcessor)
};
