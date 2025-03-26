# About
This is a port of the [BeepBox](https://beepbox.co/) synthesizers. It also implements some additions from mods.

The project itself comes with two modules:
- `synth/`: The library with the synthesizer implementation, written in C. Designed to be reusable in other projects.
- `plugin/`: Audio plugin (VST3, CLAP) written in C++

### Implemented:
- Standalone program and VST3 support (CLAP possible but untested)
- FM synthesizer
- User interface

### To be implemented:
- Envelopes
- Presets
- Themes
- Linux/Mac support
- dynamic linking for synth lib
- Instruments:
    - Chip wave
    - FM6 (from mods)
    - Noise
    - Pulse width
    - Harmonics
    - Spectrum
    - Picked string
    - Supersaw
- Modifiers:
    - Transition type
    - Chord type?
    - Pitch shift/detune
    - Vibrato
    - Note filter
- Effects:
    - Distortion
    - Bitcrusher
    - Panning
    - Chorus (or is this a modifier?)
    - Echo
    - Reverb

# Building
## Building the synth code
Requirements:
- C11-compliant compiler
- CMake + build system (Ninja, Makefile, MSVC, e.t.c.)

```bash
cd synth
mkdir build
cd build

# configure cmake
cmake ..

# compile into a static library
cmake --build .
```

## Building the plugin code
Requirements:
- C11-compliant compiler (for synth code)
- C++ compiler
- CMake + build system (Ninja, Makefile, MSVC, e.t.c.)

```bash
mkdir build
cd build

# configure cmake
# possible arguments to pass:
#   -DBUILD_STANDALONE=1
#   -DBUILD_VST3=1
#   -DBUILD_CLAP=1
cmake ..

# compile the executable/vst3/clapplugin
cmake --build .
```

## Credits
- [BeepBox](https://beepbox.co), created by John Nesky. Very awesome.
- [imgui](https://github.com/ocornut/imgui) GUI library. Neat.
- [CPLUG](https://github.com/Tremus/CPLUG) plugin framework. Thank God I found this.
- [sokol_gfx](https://github.com/floooh/sokol/) graphics library. Quite flexible.

## vst logo for legal reasons
![VST is a registered trademark of Steinberg Media Technologies GmbH](vst_logo.png)