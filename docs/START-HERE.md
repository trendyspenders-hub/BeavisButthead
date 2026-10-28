# Beavis And Butthead by I.R.Evan — 0.3.0 test edition

## Install

Open the DMG and run the PKG. It installs universal Apple Silicon/Intel AU and VST3 plug-ins and the factory model library. Restart your DAW and rescan plug-ins if needed. Pro Tools/AAX is not included.

The plug-in binaries have ad-hoc signatures. This test installer is unsigned and not Apple-notarized. Native DAW listening and GUI checks on your Mac remain necessary; automated AU validation does not establish compatibility with every host.

## Play

New instances open on **AB763 Preamp** with a cabinet selected. The **AB763 Preamp** and **2203 Preamp** tabs run actual component-network preamp calculations without requiring NAM files. Gain and Bass/Mid/Treble alter the modeled circuits. AB763 has Bright; 2203 has Master. Input Trim changes the level feeding the circuit; Post EQ is downstream.

These two experimental preamps are not complete amplifier replicas: power valves, phase inverter, transformer, supply sag and feedback are omitted. Tube parameters, rails and some loading are approximations. No physical-amplifier accuracy claim is made.

For the six included NAM captures, select a factory amp button; it switches to NAM automatically. Choose one of three cabinet buttons, or browse for your own model/IR. Older saved sessions retain NAM mode. Do not move factory content from `/Library/Application Support/I.R.Evan/Beavis And Butthead/models`, because sessions save file paths.

Open the pedal drawer for gate, overdrive, echo and cabinet controls. The signal path is internally mono; stereo input is summed and output duplicated. The overdrive is a generic pedal, not a measured hardware clone. There is no reverb, chorus or tempo sync.

## Test scope

The circuit suite covers 32 sample-rate/control/overload cases, convergence, reset, silence, block independence and processing allocations. Six ngspice comparisons at three input levels check numerical agreement with the same component ledger and device equations; they do not validate a physical amplifier. Previous NAM-chain testing covered 288 capture/cabinet/rate/buffer combinations.

Current build status, test evidence, circuit sources and limitations:
https://github.com/trendyspenders-hub/BeavisButthead

## Credits

Based on NeuralAmpModelerPlugin, pinned at 16be869746b8915885c7a35bafdcc0061faeb50e. Software licenses, model provenance and notices are retained. The themed artwork is unofficial; character rights remain with their owners and are not granted by NAM's software license.
