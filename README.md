# NOIZZ IMAGER — Windows VST3 source project

A JUCE-based stereo imaging plugin prototype with a black / dark-gray / neon-green resizable interface, NOIZZ STUDIO logo, input/output peak meters, a polar stereo-field display, Width, Space, Shuffle, Input, Output and Mix controls, M/S monitor, built-in category presets, and save-user-preset export.

## Build a Windows 64-bit VST3

Requirements: Windows 10/11 x64, Visual Studio 2022 with **Desktop development with C++**, CMake 3.22+, Git, and an internet connection for the initial JUCE download.

1. Extract this folder.
2. Open **x64 Native Tools Command Prompt for VS 2022**.
3. `cd` into the extracted `noizz_imager` directory.
4. Run:

```bat
cmake -S . -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release --target NOIZZ_IMAGER_VST3
```

The VST3 bundle should be under `build/NOIZZ_IMAGER_artefacts/Release/VST3/NOIZZ IMAGER.vst3` (exact intermediate path can vary slightly by JUCE/CMake version). Copy the `.vst3` bundle into `C:\Program Files\Common Files\VST3`, then rescan plugins in your DAW.

## Notes

- This is original NOIZZ-branded code inspired by general stereo-imaging concepts; it does not copy SSL source code or assets.
- The starter preset menu contains 36 category/variation entries. The current preset values are starting points, not factory-engineer-certified settings.
- The Save User Preset button exports a parameter-state file. Loading exported files back from the GUI and preset-browser management can be added in the next iteration.
- This is an initial prototype and should be auditioned/tested for phase behavior, mono compatibility, and metering before production use. Shuffle uses a simple frequency-dependent side adjustment, not an emulation of SSL's proprietary circuit.
- The supplied logo image is bundled in `Assets/`. The editor looks for it next to the app location; depending on DAW/plugin hosting paths, you may want to convert it to a JUCE BinaryData resource in a release build so it is always embedded.
