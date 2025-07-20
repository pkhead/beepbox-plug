# About
This is a plugin interface for my [cbeepsynth](https://github.com/pkhead/cbeepsynth) project.

The project's source code is located within two subdirectories of the `src` folder.
- `plugin/`: CLAP audio plugin, written in C.
- `plugin_gui/`: CLAP audio plugin gui, written in C++. (Associated with `gui_libs`)

**Implemented:**
- CLAP plugin, with standalone and VST3 versions supported by [clap-wrapper](https://github.com/free-audio/clap-wrapper)
- FM synthesizer
- User interface
- Envelopes

**To be implemented:**
- Presets
- Themes
- Mac support (?)
- Instruments:
    - Chip wave
    - Custom chip/sampler
    - FM6
    - Noise
    - Pulse width
    - Harmonics
    - Spectrum
    - Picked string
    - Supersaw
- Instrument effects:
    - Transition type
    - Chord type
- Audio effects:
    - Distortion
    - Bitcrusher
    - Panning
    - Chorus (or is this a modifier?)
    - Echo
    - Reverb

# Building
Requirements:
- C11-compliant compiler (for synth/plugin code)
- C++17-compliant compiler (for GUI)
- CMake + build system (Ninja, Makefile, MSVC, e.t.c.)

Checkout the repository with
```bash
git clone --recurse-submodules https://github.com/pkhead/beepbox-plug`
```

Then, build the plugin:
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