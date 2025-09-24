# About
This is a CLAP plugin library, with VST3 and standalone versions supported by [clap-wrapper](https://github.com/free-audio/clap-wrapper), which includes ports of [BeepBox](https://beepbox.co) synthesizers, with additions/enhancements from several community-made mods.

[BeepBox](https://beepbox.co) is a website created by John Nesky originally for creating chiptunes. Over time, though, BeepBox has increased in scope and capability in no small part due to the modding community that surrounds it.

This plugin library includes (or will include) several synthesizer plugins:
| | |
|-|-|
| Chip | A chiptune synthesizer from preset wavetables |
| Noise *(planned)* | A generator for several types of noise |
| Custom Chip *(planned)* | A chiptune synthesizer from a configurable wavetable |
| Pulse Width | Generates pulse waves of a freely configurable duty cycle |
| Supersaw *(planned)* | Sawtooth/pulse waves with a more powerful unison effect |
| Harmonics | An additive synthesizer |
| Picked String *(planned)* | Harmonics + an algorithm based off Karplus-Strong |
| Spectrum | A noise-based spectral synthesizer |
| FM | A four-operator FM synthesizer |
| FM6 *(planned)* | A six-operator FM synthesizer |
| Drumset *(planned)* | A Spectrum + filter envelope for each of the 12 keys in one octave |

This library also includes these audio effects bundled into the synthesizer:
- Distortion
- Bitcrusher
- Chorus
- Echo (aka delay)
- Reverb

Note that these effects have very simple parameters (only one or two).

The project's source code is located within three subdirectories of the `src` folder.
- `cbeepsynth/`: The library which contains the C port of BeepBox's synthesizers and effects.
- `plugin/`: CLAP audio plugin, written in C.
- `plugin_gui/`: CLAP audio plugin gui, written in C++.

**To be implemented:**
- Presets
- Themes
- Mac support (?)
- Instruments:
    - Custom chip
    - FM6
    - Noise
    - Picked string
    - Supersaw

# Building
Requirements:
- C99-compliant compiler (for synth/plugin code)
- C++17-compliant compiler (for GUI)
- CMake + build system (Ninja, Makefile, MSVC, e.t.c.)

Checkout the repository with
```bash
git clone --recurse-submodules https://github.com/pkhead/beepbox-plug
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
- [clap-wrapper](https://github.com/free-audio/clap-wrapper)
- [pugl](https://gitlab.com/lv2/pugl/) windowing library.
- [stb_image](https://github.com/nothings/stb) for loading the VST trademark logo.

## vst logo for legal reasons
![VST is a registered trademark of Steinberg Media Technologies GmbH](vst_logo.png)