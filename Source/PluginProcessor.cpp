#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    float db(float x) { return juce::Decibels::decibelsToGain(x); }

    void addParam(juce::AudioProcessorValueTreeState::ParameterLayout& p,
                  const juce::String& id, const juce::String& name,
                  float min, float max, float def, float step = 0.01f)
    {
        p.add(std::make_unique<juce::AudioParameterFloat>(
            id, name, juce::NormalisableRange<float>(min, max, step), def));
    }
}

SISHHIN_HZ_MACHINEAudioProcessor::SISHHIN_HZ_MACHINEAudioProcessor()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

SISHHIN_HZ_MACHINEAudioProcessor::APVTS::ParameterLayout
SISHHIN_HZ_MACHINEAudioProcessor::createParameterLayout()
{
    APVTS::ParameterLayout p;

    addParam(p, "ingain", "Input", -12, 12, 0, 0.1f);
    addParam(p, "gate", "Gate Threshold", -80, -20, -55, 1);
    addParam(p, "out", "Output", -24, 12, -6, 0.1f);

    addParam(p, "gain", "Gain", 0, 10, 6, 0.1f);
    addParam(p, "tight", "Tight", 0, 10, 5, 0.1f);
    addParam(p, "bass", "Bass", 0, 10, 5, 0.1f);
    addParam(p, "mid", "Mid", 0, 10, 6, 0.1f);
    addParam(p, "treble", "Treble", 0, 10, 5, 0.1f);
    addParam(p, "pres", "Presence", 0, 10, 5, 0.1f);
    addParam(p, "master", "Master", 0, 10, 6, 0.1f);

    addParam(p, "d_drive", "Drive", 0, 10, 4, 0.1f);
    addParam(p, "d_tone", "Drive Tone", 0, 10, 5, 0.1f);
    addParam(p, "d_level", "Drive Level", 0, 10, 6, 0.1f);
    addParam(p, "d_tight", "Drive Tight", 0, 10, 6, 0.1f);

    addParam(p, "b_gain", "Boost", 0, 10, 5, 0.1f);
    addParam(p, "b_tone", "Boost Tone", 0, 10, 5, 0.1f);
    addParam(p, "b_level", "Boost Level", 0, 10, 5, 0.1f);

    addParam(p, "c_sus", "Compressor Sustain", 0, 10, 5, 0.1f);
    addParam(p, "c_att", "Compressor Attack", 0, 10, 5, 0.1f);
    addParam(p, "c_level", "Compressor Level", 0, 10, 5, 0.1f);

    addParam(p, "t_pitch", "Drop Pitch", -12, 12, -2, 1);
    addParam(p, "t_blend", "Drop Blend", 0, 10, 10, 0.1f);

    addParam(p, "s_buzz", "Sitar Buzz", 0, 10, 6, 0.1f);
    addParam(p, "s_res", "Sitar Sympathy", 0, 10, 5, 0.1f);
    addParam(p, "s_tone", "Sitar Tone", 0, 10, 6, 0.1f);
    addParam(p, "s_mix", "Sitar Mix", 0, 10, 7, 0.1f);

    addParam(p, "hpf", "HPF", 20, 300, 40, 1);
    addParam(p, "lpf", "LPF", 3000, 16000, 12000, 10);

    addParam(p, "e1", "EQ Low", -15, 15, 0, 0.1f);
    addParam(p, "e2", "EQ Low Mid", -15, 15, 0, 0.1f);
    addParam(p, "e3", "EQ Mid", -15, 15, 0, 0.1f);
    addParam(p, "e4", "EQ High Mid", -15, 15, 0, 0.1f);
    addParam(p, "e5", "EQ High", -15, 15, 0, 0.1f);

    p.add(std::make_unique<juce::AudioParameterChoice>(
        "mode", "Amp Mode",
        juce::StringArray{"Clean", "Crunch", "Modern", "Lead", "Sitar"}, 2));

    p.add(std::make_unique<juce::AudioParameterChoice>(
        "cab", "Cabinet",
        juce::StringArray{"1x12", "2x12", "4x12"}, 2));

    p.add(std::make_unique<juce::AudioParameterBool>("gateon", "Gate", true));
    p.add(std::make_unique<juce::AudioParameterBool>("drvon", "Drive On", true));
    p.add(std::make_unique<juce::AudioParameterBool>("bston", "Boost On", false));
    p.add(std::make_unique<juce::AudioParameterBool>("cmpon", "Compressor On", false));
    p.add(std::make_unique<juce::AudioParameterBool>("stron", "Sitar On", false));

    return p;
}

void SISHHIN_HZ_MACHINEAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    spec = { sampleRate, (juce::uint32)samplesPerBlock, 2 };

    chainL.prepare(spec);
    chainR.prepare(spec);

    inputGain.reset(sampleRate, 0.02);
    outputGain.reset(sampleRate, 0.02);

    inputGain.setCurrentAndTargetValue(1.0f);
    outputGain.setCurrentAndTargetValue(db(-6.0f));

    gateEnvL = gateEnvR = 0.0f;
}

bool SISHHIN_HZ_MACHINEAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    auto mainIn = layouts.getChannelSet(true, 0);
    auto mainOut = layouts.getChannelSet(false, 0);

    return mainIn == juce::AudioChannelSet::mono()
        || mainIn == juce::AudioChannelSet::stereo()
        ? mainOut == mainIn
        : false;
}

void SISHHIN_HZ_MACHINEAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                     juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    auto* inGain = apvts.getRawParameterValue("ingain");
    auto* outGain = apvts.getRawParameterValue("out");
    auto* gain = apvts.getRawParameterValue("gain");
    auto* tight = apvts.getRawParameterValue("tight");
    auto* bass = apvts.getRawParameterValue("bass");
    auto* mid = apvts.getRawParameterValue("mid");
    auto* treble = apvts.getRawParameterValue("treble");
    auto* pres = apvts.getRawParameterValue("pres");
    auto* master = apvts.getRawParameterValue("master");
    auto* gate = apvts.getRawParameterValue("gate");

    const int channels = buffer.getNumChannels();
    const int samples = buffer.getNumSamples();

    inputGain.setTargetValue(db(inGain->load()));
    outputGain.setTargetValue(db(outGain->load()));

    // Pre-emphasis / tight low cut.
    auto hp = juce::dsp::IIR::Coefficients<float>::makeHighPass(
        spec.sampleRate, 35.0 + tight->load() * 12.0);
    chainL.get<0>().coefficients = hp;
    chainR.get<0>().coefficients = hp;

    // Main high-gain stage.
    const float drive = 0.7f + gain->load() * 0.62f;
    chainL.get<1>().setGainLinear(drive);
    chainR.get<1>().setGainLinear(drive);

    // Tone shaping after distortion.
    const float lowHz = 90.0f;
    const float highHz = 6500.0f + treble->load() * 900.0f;

    chainL.get<2>().coefficients =
        juce::dsp::IIR::Coefficients<float>::makeLowShelf(
            spec.sampleRate, lowHz, 0.7071f,
            juce::Decibels::decibelsToGain((bass->load() - 5.0f) * 1.5f));
    chainR.get<2>().coefficients = chainL.get<2>().coefficients;

    chainL.get<3>().coefficients =
        juce::dsp::IIR::Coefficients<float>::makePeakFilter(
            spec.sampleRate, 900.0f, 0.8f,
            juce::Decibels::decibelsToGain((mid->load() - 5.0f) * 1.4f));
    chainR.get<3>().coefficients = chainL.get<3>().coefficients;

    chainL.get<4>().setGainLinear(
        db((master->load() - 5.0f) * 1.0f + (pres->load() - 5.0f) * 0.25f));
    chainR.get<4>().setGainLinear(
        db((master->load() - 5.0f) * 1.0f + (pres->load() - 5.0f) * 0.25f));

    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < samples; ++i)
            data[i] *= inputGain.getNextValue();
    }

    // Simple musical gate.
    if (apvts.getRawParameterValue("gateon")->load() > 0.5f)
    {
        const float threshold = db(gate->load());
        const float attack = 0.995f;
        const float release = 0.9995f;

        for (int ch = 0; ch < channels; ++ch)
        {
            float env = ch == 0 ? gateEnvL : gateEnvR;
            auto* data = buffer.getWritePointer(ch);

            for (int i = 0; i < samples; ++i)
            {
                const float a = std::abs(data[i]);
                env = std::max(a, env * (a > env ? attack : release));
                if (env < threshold)
                    data[i] *= 0.05f;
            }

            if (ch == 0) gateEnvL = env; else gateEnvR = env;
        }
    }

    // Soft-clipping amp stage.
    for (int ch = 0; ch < channels; ++ch)
    {
        auto* data = buffer.getWritePointer(ch);
        for (int i = 0; i < samples; ++i)
        {
            const float x = data[i];
            const float mode = apvts.getRawParameterValue("mode")->load();
            float y = std::tanh(x * (1.5f + mode * 0.35f));

            if (apvts.getRawParameterValue("drvon")->load() > 0.5f)
            {
                const float dg = apvts.getRawParameterValue("d_drive")->load();
                y = std::tanh(y * (1.0f + dg * 0.8f));
            }

            if (apvts.getRawParameterValue("bston")->load() > 0.5f)
                y = std::tanh(y * (1.0f + apvts.getRawParameterValue("b_gain")->load() * 0.18f));

            if (apvts.getRawParameterValue("stron")->load() > 0.5f)
            {
                const float buzz = apvts.getRawParameterValue("s_buzz")->load() * 0.015f;
                y += std::sin(y * 55.0f) * buzz;
            }

            data[i] = y * outputGain.getNextValue();
        }
    }

    auto contextL = juce::dsp::ProcessContextReplacing<float>(juce::dsp::AudioBlock<float>(buffer).getSingleChannelBlock(0));
    chainL.process(contextL);

    if (channels > 1)
    {
        auto contextR = juce::dsp::ProcessContextReplacing<float>(
            juce::dsp::AudioBlock<float>(buffer).getSingleChannelBlock(1));
        chainR.process(contextR);
    }
}

void SISHHIN_HZ_MACHINEAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary(*xml, destData);
}

void SISHHIN_HZ_MACHINEAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary(data, sizeInBytes))
        if (xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SISHHIN_HZ_MACHINEAudioProcessor();
}
