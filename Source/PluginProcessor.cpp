#include "PluginProcessor.h"
#include "PluginEditor.h"

PanoramizatorAudioProcessor::PanoramizatorAudioProcessor()
    : AudioProcessor (BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout PanoramizatorAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> p;

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "TIME", "Time",
        juce::NormalisableRange<float> (0.00005f, 3.0f, 0.0f, 0.35f),
        0.10f, "s"));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "LEFT", "Left",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        1.0f));

    p.push_back (std::make_unique<juce::AudioParameterFloat>(
        "RIGHT", "Right",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        1.0f));

    p.push_back (std::make_unique<juce::AudioParameterBool>("LINK", "Link", true));
    p.push_back (std::make_unique<juce::AudioParameterBool>("RANDOM", "Random", false));

    return { p.begin(), p.end() };
}

juce::AudioProcessorEditor* PanoramizatorAudioProcessor::createEditor()
{
    return new PanoramizatorAudioProcessorEditor (*this);
}

void PanoramizatorAudioProcessor::prepareToPlay (double sampleRate, int)
{
    currentSampleRate = sampleRate;
    currentPan = 0.0f;
    startPan = 0.0f;
    targetPan = 0.0f;
    samplesIntoRamp = 0;
    rampLengthSamples = juce::jmax (1, (int) std::round (0.10 * sampleRate));
}

void PanoramizatorAudioProcessor::releaseResources() {}

bool PanoramizatorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    const auto mainIn = layouts.getChannelSet (true, 0);
    const auto mainOut = layouts.getChannelSet (false, 0);
    return mainIn == juce::AudioChannelSet::mono() || mainIn == juce::AudioChannelSet::stereo()
        ? (mainOut == mainIn)
        : false;
}

void PanoramizatorAudioProcessor::chooseNextTarget()
{
    startPan = currentPan;

    const bool link = parameters.getRawParameterValue ("LINK")->load() > 0.5f;
    const float left  = juce::jlimit (0.0f, 1.0f, parameters.getRawParameterValue ("LEFT")->load());
    const float right = juce::jlimit (0.0f, 1.0f, parameters.getRawParameterValue ("RIGHT")->load());
    const bool isRandom = parameters.getRawParameterValue ("RANDOM")->load() > 0.5f;

    float l = left;
    float r = right;
    if (link)
    {
        const float extent = juce::jmax (left, right);
        l = r = extent;
    }

    if (isRandom)
        targetPan = random.nextFloat() * (l + r) - l;
    else
    {
        // Deterministic bounce: travel continuously from one limit to the other.
        targetPan = (currentPan <= 0.0f) ? r : -l;
        if (std::abs (targetPan - currentPan) < 0.0001f)
            targetPan = (targetPan >= 0.0f) ? -l : r;
    }

    const float timeSeconds = juce::jlimit (0.00005f, 3.0f,
                                            parameters.getRawParameterValue ("TIME")->load());
    rampLengthSamples = juce::jmax (1, (int) std::round (timeSeconds * currentSampleRate));
    samplesIntoRamp = 0;
}

void PanoramizatorAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    const int numChannels = buffer.getNumChannels();

    if (numChannels == 0)
        return;

    for (int sample = 0; sample < numSamples; ++sample)
    {
        if (samplesIntoRamp >= rampLengthSamples)
            chooseNextTarget();

        const float progress = juce::jlimit (0.0f, 1.0f,
            (float) samplesIntoRamp / (float) juce::jmax (1, rampLengthSamples));
        currentPan = startPan + (targetPan - startPan) * progress;
        ++samplesIntoRamp;

        // Equal-power stereo pan: -1 = L, +1 = R.
        const float angle = (currentPan + 1.0f) * juce::MathConstants<float>::pi * 0.25f;
        const float gainL = std::cos (angle);
        const float gainR = std::sin (angle);

        if (numChannels == 1)
        {
            // Mono input: duplicate to stereo if the bus is stereo is impossible here;
            // keep mono safely processed with the left gain.
            buffer.setSample (0, sample, buffer.getSample (0, sample) * gainL);
        }
        else
        {
            const float inL = buffer.getSample (0, sample);
            const float inR = buffer.getSample (1, sample);
            const float mono = 0.5f * (inL + inR);
            buffer.setSample (0, sample, mono * gainL);
            buffer.setSample (1, sample, mono * gainR);
        }
    }
}

void PanoramizatorAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PanoramizatorAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PanoramizatorAudioProcessor();
}
