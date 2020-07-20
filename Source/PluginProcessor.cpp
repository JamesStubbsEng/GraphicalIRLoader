/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.


    PluginProcessor.cpp
    Created by James Stubbs on 7/20/20

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicalIrLoaderAudioProcessor::GraphicalIrLoaderAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, Identifier("GraphicalIRLoader"),
                           {
                               //put parameters that require automation here
                           })
#endif
{
   // irFilePath = parameters.state.getPropertyAsValue("IR_FILEPATH", nullptr, true);
    
}

GraphicalIrLoaderAudioProcessor::~GraphicalIrLoaderAudioProcessor()
{
}

//==============================================================================
const String GraphicalIrLoaderAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool GraphicalIrLoaderAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool GraphicalIrLoaderAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool GraphicalIrLoaderAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double GraphicalIrLoaderAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int GraphicalIrLoaderAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int GraphicalIrLoaderAudioProcessor::getCurrentProgram()
{
    return 0;
}

void GraphicalIrLoaderAudioProcessor::setCurrentProgram (int index)
{
}

const String GraphicalIrLoaderAudioProcessor::getProgramName (int index)
{
    return {};
}

void GraphicalIrLoaderAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void GraphicalIrLoaderAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = getTotalNumOutputChannels();

    fxChain.prepare(spec);
    updateParameters();
}

void GraphicalIrLoaderAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool GraphicalIrLoaderAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void GraphicalIrLoaderAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    auto block = juce::dsp::AudioBlock<float>(buffer);
    auto context = juce::dsp::ProcessContextReplacing<float>(block);
    updateParameters();
    fxChain.process(context);
}

//==============================================================================
bool GraphicalIrLoaderAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* GraphicalIrLoaderAudioProcessor::createEditor()
{
    return new GraphicalIrloaderAudioProcessorEditor (*this);
}

//==============================================================================
void GraphicalIrLoaderAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void GraphicalIrLoaderAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState.get() != nullptr)
        if (xmlState->hasTagName(parameters.state.getType()))
            parameters.replaceState(ValueTree::fromXml(*xmlState));
            //this is called after the Value Treee is replaced!!
            irFilePath = parameters.state.getPropertyAsValue("IR_FILEPATH", nullptr, true);
            // could put listeners here 
}

String GraphicalIrLoaderAudioProcessor::getCurrentIRName()
{
    File tmp(irFilePath.getValue().toString());
    return tmp.getFileName();
}

String GraphicalIrLoaderAudioProcessor::getIRFilePath()
{
    return irFilePath.getValue();
}

void GraphicalIrLoaderAudioProcessor::loadIR(File file)
{
    
    irFilePath.setValue(file.getFullPathName());
    // to prevent Logic parameter only recalled when plugin is opened bug
    irNameFlag = file.getFileName();
    fxChain.template get<cabSimulatorIndex>().loadImpulseResponse(file, false, true, 0);
    
}

void GraphicalIrLoaderAudioProcessor::updateParameters()
{
    if (getCurrentIRName() != irNameFlag)
    {
        File tmp(irFilePath.getValue());
        fxChain.template get<cabSimulatorIndex>().loadImpulseResponse(tmp, false, true, 0);
    }
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new GraphicalIrLoaderAudioProcessor();
}
