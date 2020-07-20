/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.


    PluginEditor.h
    Created by James Stubbs on 7/20/20

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "AnalyserComponent.h"

//==============================================================================
/**
*/
class GraphicalIrloaderAudioProcessorEditor  : public AudioProcessorEditor
                                                
{
public:
    GraphicalIrloaderAudioProcessorEditor (GraphicalIrLoaderAudioProcessor&);
    ~GraphicalIrloaderAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

    void showBubbleMessage(const String& text, Component& target, int time)
    {
        bubble.reset(new BubbleMessageComponent(500));
        addChildComponent(bubble.get());

        AttributedString attString;
        attString.append(text, Font(16.0f), Colours::white);

        bubble->showAt(getLocalArea(&target, target.getLocalBounds()),
            attString, time,  // numMillisecondsBeforeRemoving
            true,  // removeWhenMouseClicked
            false); // deleteSelfAfterUse
    }

private:
    TextButton irLoaderButton;
    AnalyserComponent analyzer;
    TextButton nextIRButton;
    TextButton previousIRButton;
    std::unique_ptr<BubbleMessageComponent> bubble;
    std::vector<File> wavFiles;
    int wavFileIndex = 0;
    GraphicalIrLoaderAudioProcessor& processor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (GraphicalIrloaderAudioProcessorEditor)
};
