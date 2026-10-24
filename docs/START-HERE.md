# Beavis And Butthead by I.R.Evan — 0.2.0

This package contains a native audio plug-in source prototype, six playable NAM amp captures, three cabinet IRs, an interactive silent interface preview, research and audit evidence. It does not contain compiled AU/VST3/AAX plug-ins or an installer.

## Preview the interface

Open `preview/Beavis-And-Butthead-Preview.html` in Safari or Chrome. Drag across Drive to move through four character reactions. Select **Open Pedal Drawer** for gate, overdrive, echo and cabinet controls; Close or Escape returns to the amp panel. The preview produces no audio. The design PNG shows full Drive with the drawer open.

The truncated maximum-Drive artwork was rebuilt. Every reaction PNG and every embedded preview image is now fully decoded by the package checks.

## Native audio

The source retains NAM inference, model/IR browsing, gate, Post EQ, cabinet bypass, calibration, normalization and meters. The new overdrive and echo run in the native processing chain and default off. The drawer exposes their parameters and shares gate/cabinet bypass with the main panel. New pedal parameters follow the original thirteen parameter indices.

The amp path is internally mono, including a stereo input summed to mono and output duplicated when the host uses stereo. Echo is mono. There is no reverb, chorus or tempo sync in this version. The generic soft-clip pedal is not a measured clone of a named hardware pedal.

Read `models/README.md`, then load a `.nam` from `models/` and a cabinet `.wav` from `models/cabinets/` in the built native plug-in. The six captures cover Twin, AC15, JVM, 5150, Mark IV and SLO100. Model weights are unchanged; hashes, provenance and license notices are included. Keep model files somewhere permanent before saving a session. The plug-in stores external paths.

## Build AU/VST3 on macOS

Install full Xcode, launch it and accept its license; select it as the active developer toolchain. Git and Python 3 are also needed. Unpack to a path without spaces because some upstream scripts assume this.

Double-click `Build-Mac.command`, or run `bash scripts/build-mac.command`. It obtains the pinned VST3 SDK and iPlug2 graphics libraries if missing, builds arm64/x86_64 AU and VST3, checks architecture/resources, applies ad-hoc signatures and makes an unsigned test PKG/DMG in a fresh `mac-build-XXXXXX` folder. This build script is prepared and syntax checked; native compilation has not run here.

The generated installer also installs the six amp captures, three cabinet IRs and their notices under `/Library/Application Support/I.R.Evan/Beavis And Butthead/models`. Load files from that permanent location in the native plug-in.

Those packages are for testing, not notarized distribution. The build script does not create AAX. An AU/VST3 cannot load directly in Pro Tools.

## Signing and release tests

See `docs/FINISH-RELEASE.md` for the Apple signing/notarization script, installed-plugin validation and required credentials. Record actual DAW results in `docs/RELEASE-HOST-TESTS.csv`. All host rows start as NOT RUN; scripts are not evidence of completed signing.

## Pro Tools

Read `docs/PRO-TOOLS-AAX.md`. A separate `scripts/build-aax-dev.command` prepares an unsigned AAX Native development build once a compatible Avid SDK is supplied. Required AAX signing and an actual Pro Tools host test remain outstanding. There is no claim of AudioSuite/HDX or universal version compatibility.

## Audit and repeatable tests

Read `docs/Research-and-Audit.md`. The shared-engine matrix passed 288 cases across six captures, three real IRs, four sample rates and four buffer sizes. Added pedal DSP, guarded state reading and preview DOM behavior also passed the recorded tests. These results do not establish physical-amp accuracy or replace native DAW validation.

With CMake, a C++20 compiler and Python installed, run `python3 scripts/run-smoke.py` to rebuild the standalone tests. The DOM test is `tests/preview_dom.cjs` and requires Node with `jsdom` installed. No browser automation dependency is needed to use the preview itself.

## GitHub

The included `.github/workflows/build-irevan.yml` can build the Mac AU/VST3 test installers in your own repository. No remote repository was created or modified. AAX is not included in that workflow because its SDK/signing setup is absent.

## Credits

Based on https://github.com/sdatkinson/NeuralAmpModelerPlugin, pinned at `16be869746b8915885c7a35bafdcc0061faeb50e`. The NAM MIT license and dependency notices remain with the source. Added original prototype pedal code does not include BYOD or Airwindows code.

The character artwork is an unofficial themed prototype generated with ChatGPT's built-in image tool using the user's reference. Prompt: four locked-composition Beavis and Butt-Head couch/guitar scenes, bored to smirking to laughing to headbanging, 1990s cel styling, teal wall, burgundy couch, no generated lettering. Character/reference rights remain with their owners; NAM's software license does not license character artwork. Full text in the interface is rendered as typed UI text.
