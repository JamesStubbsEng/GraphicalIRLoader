
 /*
   ==============================================================================

     AnalyserComponent.h
     Created by James Stubbs on 7/20/20

   ==============================================================================
 */


#pragma once
#include <JuceHeader.h>

//==============================================================================
class AnalyserComponent   : public AudioAppComponent,
                            private Timer
{
public:
    AnalyserComponent()
        : forwardFFT (fftOrder),
          window (fftSize, juce::dsp::WindowingFunction<float>::hann)
    {
        setOpaque (true);
        setAudioChannels (2, 0);  
        startTimerHz (30);
        setSize (700, 500);
    }

    ~AnalyserComponent() override
    {
        shutdownAudio();
    }

    //==============================================================================
    void prepareToPlay (int, double) override {}
    void releaseResources() override          {}

    void getNextAudioBlock (const juce::AudioSourceChannelInfo& bufferToFill) override {}

    void loadIRIntoFFT(File irFile)
    {


        FileInputStream* fileInputStream = irFile.createInputStream();
        ScopedPointer<AudioFormatReader> wavReader;
        WavAudioFormat wavAF;
        if (fileInputStream != nullptr) {
            wavReader = wavAF.createReaderFor(fileInputStream, true);
            ScopedPointer<AudioSampleBuffer> buffer = new AudioSampleBuffer(2, fftSize);
   

            if (wavReader) {

                wavReader->read(buffer, 0, fftSize, 0, true, true);
                auto* channelData = buffer->getReadPointer(0);
                for (auto i = 0; i < buffer->getNumSamples(); ++i)
                    pushNextSampleIntoFifo(channelData[i]);

            }
        }
        else
        {
            DBG("input stream cannot be opened");
        }
    }

    //==============================================================================
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colours::black);
        g.setOpacity (1.0f);
        g.setColour (juce::Colours::white);
        drawFrame (g);
    }

    void timerCallback() override
    {
       if (nextFFTBlockReady)
       {
          drawNextFrameOfSpectrum();
           nextFFTBlockReady = false;
           repaint();
        }
    }

    void pushNextSampleIntoFifo (float sample) noexcept
    {
 
        if (fifoIndex == fftSize-1)               
        {
            if (! nextFFTBlockReady)            
            {
                juce::zeromem (fftData, sizeof (fftData));
                memcpy (fftData, fifo, sizeof (fifo));
                nextFFTBlockReady = true;
              
            }

            fifoIndex = 0;
        }

        fifo[fifoIndex++] = sample;             
    }

    void drawNextFrameOfSpectrum()
    {

        window.multiplyWithWindowingTable (fftData, fftSize);       

        forwardFFT.performFrequencyOnlyForwardTransform (fftData); 

        auto mindB = -100.0f;
        auto maxdB =    0.0f;

        for (int i = 0; i < scopeSize; ++i)                         
        {
            auto skewedProportionX = 1.0f - std::exp (std::log (1.0f - (float) i / (float) scopeSize) * 0.2f);
            auto fftDataIndex = juce::jlimit (0, fftSize / 2, (int) (skewedProportionX * (float) fftSize * 0.5f));
            auto level = juce::jmap (juce::jlimit (mindB, maxdB, juce::Decibels::gainToDecibels (fftData[fftDataIndex])
                                                               - juce::Decibels::gainToDecibels ((float) fftSize)),
                                     mindB, maxdB, 0.0f, 1.0f);

            //scale the dB level by 2 to fill the screen because impulse responses have low levels
            scopeData[i] = level*2;                                   
        }
           
        // smooth the fft using a DIY Lowpass filter 
        smoothedScopeData[0] = scopeData[0];
        for (int i = 1; i < scopeSize; i++)
        {
            smoothedScopeData[i] = SMOOTH_BETA*scopeData[i] + (1- SMOOTH_BETA)* smoothedScopeData[i - 1];
        }

    }

    void drawFrame (juce::Graphics& g)
    {
        for (int i = 1; i < scopeSize; ++i)
        {
            auto width  = getLocalBounds().getWidth();
            auto height = getLocalBounds().getHeight();

            g.drawLine ({ (float) juce::jmap (i - 1, 0, scopeSize - 1, 0, width),
                                  juce::jmap (smoothedScopeData[i - 1], 0.0f, 1.0f, (float) height, 0.0f),
                          (float) juce::jmap (i,     0, scopeSize - 1, 0, width),
                                  juce::jmap (smoothedScopeData[i],     0.0f, 1.0f, (float) height, 0.0f) });
        }
    }

    enum
    {
        fftOrder  = 11,             
        fftSize   = 1 << fftOrder,  
        scopeSize = 512             
    };

private:
    juce::dsp::FFT forwardFFT;                      
    juce::dsp::WindowingFunction<float> window;     

    float fifo [fftSize];                           
    float fftData [2 * fftSize];  
    int fifoIndex = 0;                              
    bool nextFFTBlockReady = false;                 
    float scopeData [scopeSize]; 
    float smoothedScopeData [scopeSize]; 
    float const SMOOTH_BETA = 0.025;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AnalyserComponent)
};
