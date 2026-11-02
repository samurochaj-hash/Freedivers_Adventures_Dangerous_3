# Freediver's Adventures Dangerous 3 — Wii Stage 3

This is an **early homebrew prototype scaffold**, not a finished game or WBFS image.

## Current prototype
- libogc / GX initialization
- basic third-person follow camera
- low-poly placeholder diver
- simple underwater floor and coral silhouettes
- Wii Remote directional movement

## Controls
- D-pad: swim left/right/forward/back
- A: ascend
- B: descend

## Build prerequisites
1. Install devkitPro and the Wii development group (devkitPPC + libogc).
2. Open the devkitPro MSYS2 shell.
3. From this directory, run `make`.
4. If the build succeeds, the generated `boot.dol` can be tested with a compatible Wii homebrew setup.

The Makefile is a starter template and may need adjustment for the exact devkitPro release. The code has not been compiled or tested on hardware in this delivery.

## WBFS note
WBFS is a Wii disc-image container format, not the native format for a homebrew application. A standalone homebrew `.dol` is normally distributed/launched as an application; producing a WBFS requires a valid Wii disc image layout and appropriate authorized tooling. This scaffold does not create a WBFS.

## Next development tasks
- replace placeholder geometry with original 3D models
- add a compass and oxygen/health HUD
- implement swimming animation and collision
- create the first complete level and objective system
- profile performance on actual Wii hardware
