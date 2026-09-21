#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class PanoramizatorAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit PanoramizatorAudioProcessorEditor (PanoramizatorAudioProcessor&);
    ~PanoramizatorAudioProcessorEditor() override = default;
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    PanoramizatorAudioProcessor& processor;
    juce::Slider timeSlider, leftSlider, rightSlider;
    juce::ToggleButton linkButton, randomButton;
    juce::Label title, timeLabel, leftLabel, rightLabel;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    std::unique_ptr<SliderAttachment> timeAttachment, leftAttachment, rightAttachment;
    std::unique_ptr<ButtonAttachment> linkAttachment, randomAttachment;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PanoramizatorAudioProcessorEditor)
};
