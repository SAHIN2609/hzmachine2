# SISHHIN HZ MACHINE — Cloud VST3 Build

This repository is designed so you **do not need to install any development tools on Windows**.

GitHub Actions performs the entire Windows VST3 build on a GitHub-hosted Windows machine.

## What you do

1. Create a GitHub repository.
2. Upload this entire project.
3. Push to `main`.
4. Open the **Actions** tab.
5. Wait for **Build Windows VST3** to finish.
6. Download the VST3 ZIP from the workflow's **Artifacts** section.

Every push to `main` also creates a GitHub Release containing the built VST3 ZIP.

## No local compiler required

You do NOT need:

- Visual Studio
- CMake
- JUCE installed locally
- Windows SDK installed locally
- Git installed locally
- Any C++ compiler locally

GitHub's Windows runner supplies the build environment.

## Manual build

You can also start a build without changing any code:

GitHub → Actions → Build Windows VST3 → Run workflow.

## Output

The workflow produces:

`SISHHIN_HZ_MACHINE_Windows_VST3.zip`

Inside the ZIP is the Windows `.vst3` plugin bundle.

## Project

The project uses JUCE fetched automatically by CMake from the JUCE Git repository. The supplied SISHHIN HZ MACHINE HTML interface is included under `Assets/index.html`.

The current native DSP is a foundation. The UI-to-native parameter bridge and more advanced amp/cab DSP can be developed in subsequent commits.
