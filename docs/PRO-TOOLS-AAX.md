# Pro Tools / AAX Native build and validation

## Current status

AAX Native is a prepared source target. It has **not been compiled, signed, loaded or tested in Pro Tools**. No AAX binary or Pro Tools installer is supplied. This project does not claim AAX DSP/HDX processing, AudioSuite operation, or compatibility with every Pro Tools/macOS version.

An AU or VST3 file does not become AAX by renaming its extension. The Mac AU/VST3 test installer is a separate build route.

## What is prepared

- Shared NAM audio engine, gate, post-EQ, cabinet convolution and prototype drive/echo processors.
- AAX target in `BBIREvan/projects/BBIREvan-macOS.xcodeproj`.
- Manufacturer `I.R.Evan`, manufacturer ID `IrEv`, real-time type ID `BbIr`, product name `Beavis And Butthead`.
- Bundle identifier `com.IREvan.aax.BBIREvan`; Harmonic category. AudioSuite is disabled pending separate testing.
- Mono/stereo configurations inherited from NAM: 1-in/1-out, 1-in/2-out and 2-in/2-out. The internal amp path collapses input to mono and duplicates output; it is not a stereo amp model.
- New parameters are appended after the existing thirteen; guarded state-reading tests cover thirteen/eighteen-value chunks.
- `scripts/build-aax-dev.command` stages a universal arm64/x86_64 **unsigned development** bundle and checks binary architectures and reaction resources after a successful build. It does not install anything into a running Pro Tools setup.

## Required build environment

1. A Mac with full Xcode and an active developer toolchain.
2. A compatible Avid AAX SDK obtained through [Avid's developer program](https://developer.avid.com/aax/). The folder included in the source is only a README placeholder. Place the real SDK at `iPlug2/Dependencies/IPlug/AAX_SDK`; its `Interfaces/AAX.h` and `Libs/AAXLibrary/MacBuild/AAXLibrary.xcodeproj` must exist. This pinned iPlug2 project expects the AAXLibrary/libcpp layout. If the chosen SDK changes that layout, adapt and validate the target/library references before building.
3. The graphics dependencies used by iPlug2. The build script downloads the upstream prebuilt package if absent.
4. For release loading in regular Pro Tools, the required Avid/PACE signing tools and signing identity. An Apple ad-hoc signature is not sufficient. Follow [PACE's current AAX signing guide](https://paceap.com/getting-started-with-aax-code-signing-for-pro-tools-plugins/), including its Avid onboarding route. Use the developer-host/testing route provided by Avid during development.

Run `bash scripts/build-aax-dev.command` from the unpacked source folder. It fails clearly when the SDK is missing. A successful result is an unsigned development `.aaxplugin`, not a production-ready installer. Do not describe it as ready for normal Pro Tools until the remaining steps below pass.

No accounts were created, license terms accepted, keys obtained, emails sent or signing operations performed on the user's behalf during this work.

## Release validation to complete

- Build the final AAX binary for the target Pro Tools/macOS combination. Verify architecture using `lipo`; confirm product/type/manufacturer identities and all resources.
- Use Avid's current developer validation tools and host workflow. Exercise mono, mono-to-stereo and stereo insert layouts; check the declared channel behavior.
- Load all six starter captures and all three IRs. Verify missing-file errors and recovery.
- Test every parameter's automation, both drawer and main controls, bypass, session save/reopen, and restored external paths. The drawer is an editor view and opening it must not change sound.
- Check live playback, stop/start, live monitoring, offline bounce, changing hardware buffer sizes and rates 44.1/48/96/192 kHz as supported by the target host/interface.
- Check latency reporting, echo tails, clipping meters, CPU, repeated model switching and closing/reopening the editor while audio runs.
- Apply the AAX signing workflow provided by Avid/PACE. Verify the signed binary, then test that exact signed bundle in regular Pro Tools. Signing is a release step, not a substitute for the host test.
- Only after those results pass, package for `/Library/Application Support/Avid/Audio/Plug-Ins/BBIREvan.aaxplugin` on macOS. Preserve the signed bundle; do not mutate resources afterward. Signing/notarization requirements for the installer are a separate distribution concern.

Until then, Pro Tools compatibility is **pending**, not guaranteed by the standalone DSP smoke tests.
