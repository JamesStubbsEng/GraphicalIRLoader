# GraphicalIRLoader
A personal JUCE project that loads impulse response .WAV files and displays their frequency curves.

This is useful for guitarists and bassists who use amp simulators and IRs because this plugin shows the frequency curve of the IRs.  You can load in an IR and see if it is balanced, has a mid cut, has a lot of low end, etc.  

![Alt Text](https://media.giphy.com/media/kD5cQ2uVFMma8PSlFW/giphy.gif)

Techniques used include:
- convolution
- fft smoothing
- fft graphics
- file reading
- implementation for next file and previous file buttons using STL vector data structure
