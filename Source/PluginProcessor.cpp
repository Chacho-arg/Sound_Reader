// PluginProcessor.cpp
#include "PluginProcessor.h"
#include "PluginEditor.h"

SoundReaderAudioProcessor::SoundReaderAudioProcessor()
    : AudioProcessor (AudioProcessor::BusesProperties (
        #if ! JucePlugin_IsMidiEffect
         #if ! JucePlugin_IsSynth
          AudioChannelSet::stereo (),
         #else
          AudioChannelSet::mono (),
         #endif
        #else
         false,
        #endif
         {}
       )),
mstate (*this, nullptr, "Parameters", {
        std::make_unique<juce::AudioParameterFloat>("targetLUFS", "Target LUFS", -30.0f, 0.0f, -14.0f),
        std::make_unique<juce::AudioParameterFloat>("toleranceLU", "Tolerance LU", 0.1f, 5.0f, 1.0f),
        std::make_unique<juce::AudioParameterFloat>("plrMin", "PLR Min", 0.0f, 20.0f, 8.0f),
        std::make_unique<juce::AudioParameterFloat>("plrMax", "PLR Max", 0.0f, 20.0f, 12.0f)
    }),
licenseValidator(std::make_unique<DummyLicenseValidator>())
{
    targetPresetManager.loadFactoryPresets();
    targetPresetManager.loadCustomPresets();
    
    licenseResult = licenseValidator->validate();
    licenseeName = licenseValidator->getLicenseeName();
}

SoundReaderAudioProcessor::~SoundReaderAudioProcessor()
{
}

const String SoundReaderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SoundReaderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SoundReaderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SoundReaderAudioProcessor::isMidiInputAvailable() const
{
    #if JucePlugin_IsMidiEffect
     return !MidiInput::getAvailableDevices().isEmpty();
    #else
     return false;
    #endif
}

void SoundReaderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    this->sampleRate = sampleRate;
    this->maxBlockSize = samplesPerBlock;
    
    kWeightingFilter.prepare(sampleRate);
    slidingWindowLUFS.prepare(sampleRate);
    integratedLoudness.prepare(sampleRate);
    truePeakDetector.prepare(sampleRate);
    dynamicRatios.update(-100.0, -70.0, -70.0);
    fftAnalyzer.prepare(sampleRate, 2048, 2048);
    
    targetPresetManager.loadFactoryPresets();
    targetPresetManager.loadCustomPresets();
}

void SoundReaderAudioProcessor::releaseResources()
{
    kWeightingFilter.reset();
    slidingWindowLUFS.reset();
    integratedLoudness.reset();
    truePeakDetector.reset();
    fftAnalyzer.reset();
}

void SoundReaderAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    processAudioBlock(buffer);
}

void SoundReaderAudioProcessor::processAudioBlock(juce::AudioBuffer<float>& buffer)
{
    if (buffer.getNumChannels() == 0) return;
    
    auto numChannels = buffer.getNumChannels();
    auto numSamples = buffer.getNumSamples();
    
    kWeightingFilter.process(buffer);
    
    juce::AudioBuffer<float> monoBuffer(1, numSamples);
    monoBuffer.clear();
    
    for (int ch = 0; ch < numChannels; ++ch)
    {
        const float* channelData = buffer.getReadPointer(ch);
        for (int i = 0; i < numSamples; ++i)
        {
            monoBuffer.addFrom(0, i, channelData + i, 1, 1.0f / numChannels);
        }
    }
    
    float* monoData = monoBuffer.getWritePointer(0);
    
    for (int i = 0; i < numSamples; ++i)
    {
        slidingWindowLUFS.addSample(monoData[i]);
        truePeakDetector.processSample(monoData[i]);
    }
    
    double momentary = slidingWindowLUFS.getMomentaryLUFS();
    double shortTerm = slidingWindowLUFS.getShortTermLUFS();
    
    momentaryLUFS.store(momentary);
    shortTermLUFS.store(shortTerm);
    
    integratedLoudness.processBlock(monoData, numSamples, momentary);
    double integrated = integratedLoudness.getIntegratedLUFS();
    integratedLUFS.store(integrated);
    
    double lra = integratedLoudness.getLRA();
    lraLU.store(lra);
    
    double truePeak = truePeakDetector.getTruePeak();
    truePeakMax.store(truePeak);
    
    double momentaryMaxVal = momentaryMax.load();
    double shortTermMaxVal = shortTermMax.load();
    
    for (int i = 0; i < numSamples; ++i)
    {
        if (monoData[i] > momentaryMaxVal) momentaryMaxVal = monoData[i];
        if (monoData[i] > shortTermMaxVal) shortTermMaxVal = monoData[i];
    }
    
    momentaryMax.store(momentaryMaxVal);
    shortTermMax.store(shortTermMaxVal);
    
    dynamicRatios.update(truePeak, shortTerm, integrated);
    double psr = dynamicRatios.getPSR();
    double plr = dynamicRatios.getPLR();
    psr.store(psr);
    plr.store(plr);
    
    FFTAnalyzer::BandEnergy bands = fftAnalyzer.computeBands(monoData, numSamples);
    bandEnergies[0].store(bands.low);
    bandEnergies[1].store(bands.lowMid);
    bandEnergies[2].store(bands.highMid);
    bandEnergies[3].store(bands.high);
}

SoundReaderAudioProcessor::MetricsSnapshot SoundReaderAudioProcessor::getMetricsSnapshot() const
{
    MetricsSnapshot snap;
    snap.momentaryLUFS = momentaryLUFS.load();
    snap.shortTermLUFS = shortTermLUFS.load();
    snap.integratedLUFS = integratedLUFS.load();
    snap.lraLU = lraLU.load();
    snap.momentaryMax = momentaryMax.load();
    snap.shortTermMax = shortTermMax.load();
    snap.truePeakMax = truePeakMax.load();
    snap.psr = psr.load();
    snap.plr = plr.load();
    snap.bandEnergies[0] = bandEnergies[0].load();
    snap.bandEnergies[1] = bandEnergies[1].load();
    snap.bandEnergies[2] = bandEnergies[2].load();
    snap.bandEnergies[3] = bandEnergies[3].load();
    return snap;
}

juce::AudioProcessorEditor* SoundReaderAudioProcessor::createEditor()
{
    return new PluginEditor (*this);
}

bool SoundReaderAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String SoundReaderAudioProcessor::getProgramName (int index)
{
    return {};
}

void SoundReaderAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

int SoundReaderAudioProcessor::getNumPrograms()
{
    return 1;
}

int SoundReaderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SoundReaderAudioProcessor::setCurrentProgram (int index)
{
}

double SoundReaderAudioProcessor::getTailTime() const
{
    return 0.0;
}

bool SoundReaderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo() ||
           layouts.getMainInputChannelSet() == juce::AudioChannelSet::mono();
}

void SoundReaderAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::MemoryOutputStream mos(destData, false);
    mstate.copyStateToXml();
}

void SoundReaderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    auto xml = juce::XmlDocument::parse(juce::Blob(data, sizeInBytes));
    if (xml != nullptr)
        mstate.replaceStateFor(*xml);
}
