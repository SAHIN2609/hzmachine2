#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class SISHHIN_HZ_MACHINEAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit SISHHIN_HZ_MACHINEAudioProcessorEditor(SISHHIN_HZ_MACHINEAudioProcessor&);
    ~SISHHIN_HZ_MACHINEAudioProcessorEditor() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    SISHHIN_HZ_MACHINEAudioProcessor& processor;
    juce::WebBrowserComponent browser;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SISHHIN_HZ_MACHINEAudioProcessorEditor)
};
