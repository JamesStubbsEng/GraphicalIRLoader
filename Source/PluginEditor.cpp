/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

    PluginEditor.cpp
    Created by James Stubbs on 7/20/20
 
  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
GraphicalIrloaderAudioProcessorEditor::GraphicalIrloaderAudioProcessorEditor (GraphicalIrLoaderAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    addAndMakeVisible(analyzer);
    addAndMakeVisible(irLoaderButton);
    if (processor.getCurrentIRName() == "") {
        irLoaderButton.setButtonText("Load IR");
    }
    else
    {
        String msg = "Current IR: " + processor.getCurrentIRName() + "\n Click to load new IR";
        irLoaderButton.setButtonText(msg);
        //load current FFT
        File irFile(processor.getIRFilePath());
        analyzer.loadIRIntoFFT(irFile);

        DirectoryIterator dir_iterator(File(processor.getIRFilePath()).getParentDirectory(), false, "*.WAV");
        while (dir_iterator.next())
        {
            wavFiles.push_back(dir_iterator.getFile());
            if (dir_iterator.getFile() == processor.getIRFilePath())
            {
                wavFileIndex = wavFiles.size() -1;
            }
        }
    }
  
    irLoaderButton.onClick = [this]
    {
        
        FileChooser myChooser("Please select the IR you want to load...",
            File::getSpecialLocation(File::userHomeDirectory),
            "*.WAV");

        if (myChooser.browseForFileToOpen())
        {
            
            File irFile(myChooser.getResult());
            processor.loadIR(irFile);
            String msg = "Current IR: " + processor.getCurrentIRName() + "\n Click to load new IR";
            irLoaderButton.setButtonText(msg);

            //send the IR wav file url to the analyzer
            analyzer.loadIRIntoFFT(irFile);

            wavFiles.clear();
            File debug = File(processor.getIRFilePath()).getParentDirectory();
            DirectoryIterator dir_iterator(File(processor.getIRFilePath()).getParentDirectory(), false, "*.WAV");
            while (dir_iterator.next())
            {
                wavFiles.push_back(dir_iterator.getFile());
                if (dir_iterator.getFile() == processor.getIRFilePath())
                {
                    wavFileIndex = wavFiles.size() -1;
                }
            }
        }

    };
    addAndMakeVisible(nextIRButton);
    nextIRButton.setButtonText("Next IR");
    nextIRButton.onClick = [this]
    {
        if (processor.getCurrentIRName() == "") {
            showBubbleMessage("No IR loaded!", irLoaderButton, 1000);
        }
        else
        {
            if (wavFiles.size() == wavFileIndex + 1)
            {
                showBubbleMessage("This is the last .WAV files in the directory", nextIRButton, 1000);
               
            }
            else
            {
                File irFile(wavFiles[++wavFileIndex]);
                processor.loadIR(irFile);
                String msg = "Current IR: " + processor.getCurrentIRName() + "\n Click to load new IR";
                irLoaderButton.setButtonText(msg);

                //send the IR wav file url to the analyzer
                analyzer.loadIRIntoFFT(irFile);
            }
        }
    };
    addAndMakeVisible(previousIRButton);
    previousIRButton.setButtonText("Previous IR");
    previousIRButton.onClick = [this]
    {
        if (processor.getCurrentIRName() == "") {
            showBubbleMessage("No IR loaded!", irLoaderButton, 1000);
        }
        else
        {
            if (wavFileIndex == 0)
            {
                showBubbleMessage("This file is the first .WAV in the directory", previousIRButton, 1000);

            }
            else
            {
                File irFile(wavFiles[--wavFileIndex]);
                processor.loadIR(irFile);
                String msg = "Current IR: " + processor.getCurrentIRName() + "\n Click to load new IR";
                irLoaderButton.setButtonText(msg);

                //send the IR wav file url to the analyzer
                analyzer.loadIRIntoFFT(irFile);
            }
        }
    };

    setSize(600, 350);
}

GraphicalIrloaderAudioProcessorEditor::~GraphicalIrloaderAudioProcessorEditor()
{
}

//==============================================================================
void GraphicalIrloaderAudioProcessorEditor::paint (Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    
}

void GraphicalIrloaderAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(10, 10);
    auto header = area.removeFromTop(50);
    irLoaderButton.setBounds(header);
    auto nextButtonArea = area.removeFromTop(25);
    auto previousButtonArea = nextButtonArea.removeFromLeft(area.getWidth() / 2);
    previousIRButton.setBounds(previousButtonArea);
    nextIRButton.setBounds(nextButtonArea);
    analyzer.setBounds(area);
}
