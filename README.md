# SimpleEQ

A simple parametric EQ audio plugin built with **C++**, **JUCE** and **CMake**.

SimpleEQ features a real-time EQ response curve and FFT spectrum analyzer, with support for VST3, Audio Unit and Standalone formats.

## Download

🎛️ **[Download the latest release](../../releases/latest)**

Pre-built plugin versions are available through GitHub Releases.

## Installation

Download the latest MacOS release from Releases page and extract the ZIP file. Add the .component and .vst3 files to the appropriate audio plugin directories on your system.

## Features

- Parametric EQ
- Low-pass filter
- High-pass filter
- Real-time EQ response curve
- FFT spectrum analyzer
- VST3
- Audio Unit (macOS)
- Standalone application
- CMake-based build system

## Building from Source

### Requirements

- C++ compiler
- CMake
- Git
- JUCE

JUCE is included as a Git submodule.

### Clone the repository

Make sure to clone the repository together with its JUCE submodule:

```bash
git clone --recurse-submodules https://github.com/IljaKoreaus/SimpleEQ.git
cd SimpleEQ
```

If you have already cloned the repository without the submodule, run:

```bash
git submodule update --init --recursive
```

### Build

Configure the project with CMake:

```bash
cmake -B build
```

Then build the project:

```bash
cmake --build build --config Release
```

The generated plugin and standalone application can be found in the `build` directory.

## Project Structure

```text
SimpleEQ/
├── source/          # Plugin source code
├── modules/JUCE/    # JUCE framework (Git submodule)
├── assets/          # Plugin assets
├── CMakeLists.txt   # CMake configuration
└── README.md
```

## Credits

This project was developed by **Ilja Koreaus** and was created while following the **Simple EQ Plugin** tutorial by **Matkat Music**.

The tutorial was used as a learning resource and starting point for the project. The code has been developed and adapted as part of my own learning and experimentation with C++, JUCE and audio plugin development.

📺 **Tutorial:**  
[Matkat Music – Simple EQ Plugin](https://www.youtube.com/watch?v=i_Iq4_Kd7Rc)

All credit for the original tutorial and its teaching material goes to Matkat Music.

## Technologies

- **C++**
- **JUCE**
- **CMake**
- **VST3**
- **Audio Unit**

## License

This project is provided for educational and development purposes.

Please see the project's license and the JUCE licensing terms before redistributing or using the project commercially.
