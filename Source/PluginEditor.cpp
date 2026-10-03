#include "PluginEditor.h"

namespace
{
    const char* fallbackHtml =
        "<html><body style='background:#090a0c;color:#e8eaec;font-family:Segoe UI;text-align:center;padding:60px'>"
        "<h1>SISHHIN HZ MACHINE</h1><p>UI resource could not be loaded.</p></body></html>";
}

SISHHIN_HZ_MACHINEAudioProcessorEditor::SISHHIN_HZ_MACHINEAudioProcessorEditor(
    SISHHIN_HZ_MACHINEAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p),
      browser(juce::WebBrowserComponent::Options()
          .withBackend(juce::WebBrowserComponent::Options::Backend::defaultBackend))
{
    addAndMakeVisible(browser);
    setSize(1060, 760);

    // The HTML is shipped as binary data. For the first build we display it
    // through a data URL; the native parameter bridge is the next integration step.
    auto html = juce::String::fromUTF8(
        BinaryData::index_html,
        BinaryData::index_htmlSize);

    browser.goToURL("data:text/html;charset=utf-8," +
                    juce::URL::addEscapeChars(html, true));
}

void SISHHIN_HZ_MACHINEAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(0xff090a0c));
}

void SISHHIN_HZ_MACHINEAudioProcessorEditor::resized()
{
    browser.setBounds(getLocalBounds());
}
