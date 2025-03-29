# About
This is a port of the [BeepBox](https://beepbox.co/) synthesizers. It also implements some additions from mods.

The project itself comes with three modules, located in the src directory:
- `synth/`: The library with the synthesizer implementation, written in C.
- `plugin/`: CLAP audio plugin wrapper
- `plugin_gui/`: CLAP audio plugin gui, written in C++. (Associated with `gui_libs`)

### Implemented:
- CLAP plugin, with standalone and VST3 versions supported by [clap-wrapper](https://github.com/free-audio/clap-wrapper)
- FM synthesizer
- User interface
- Envelopes

### To be implemented:
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
Checkout the repository with
```bash
git clone --recurse-submodules https://github.com/pkhead/beepbox-plug`
```

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
- C11-compliant compiler (for synth/plugin code)
- C++17-compliant compiler (for GUI)
- CMake + build system (Ninja, Makefile, MSVC, e.t.c.)

```bash
mkdir build
cd build

# configure cmake
# possible arguments to pass:
#   -DBUILD_VST3=1
#   -DBUILD_STANDALONE=1
cmake ..

# compile the executable/vst3/clapplugin
cmake --build .
```

## Credits
- [BeepBox](https://beepbox.co), created by John Nesky. Very awesome.
- [imgui](https://github.com/ocornut/imgui) GUI library. Neat.
- [sokol_gfx](https://github.com/floooh/sokol/) graphics library. Quite flexible.
- [clap-wrapper](https://github.com/free-audio/clap-wrapper)

## vst logo for legal reasons
![VST is a registered trademark of Steinberg Media Technologies GmbH](vst_logo.png)