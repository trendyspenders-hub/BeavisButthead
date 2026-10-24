# Beavis And Butthead by I.R.Evan — research and audit

Version 0.2.0 source prototype · Research completed October 23, 2026 (Vancouver).

## Outcome

The artwork defect is fixed. A pedal drawer is implemented in the native source and interactive visual preview. Six real NAM captures and three cabinet IRs are included as external starter files. Shared audio processing and state-reading components passed the tests described below.

This is **not a release-certified or installable Pro Tools plug-in**. Native macOS AU/VST3/AAX compilation and host validation remain outstanding. No AAX binary, PACE signature, or Pro Tools compatibility result was produced here.

## Amp reference research

These are six useful reference families spanning clean, British crunch and high gain, not an objective ranking of the world's best amps. The factual specifications are recorded in `amp-reference-specs.json`. They describe the named manufacturer's reference variant; they do not prove which exact variant, controls or cabinet were used in a community capture.

| Amp reference | Verified reference facts | What this implies for faithful modeling |
|---|---|---|
| Fender '65 Twin Reverb reissue | 85 W into 4 ohms; two 12-inch Jensen C-12K speakers; four 6L6GC output tubes; Normal and Vibrato channels | Capture the chosen channel, Bright switch and volume setting; use a matching speaker capture or verified Jensen cabinet IR. |
| Vox AC15C1 Custom | 15 W into 16 ohms; EL84 power section; one 12-inch G12M Greenback; Normal and Top Boost inputs; Tone Cut | The C1 and Alnico-Blue C1X must remain distinct. A generic post-EQ is not a reconstruction of Top Boost or Tone Cut behavior. |
| Marshall JVM410H | 100 W; four EL34 power valves; five ECC83 including phase splitter; four channels; per-channel three-way EQ; two effects loops | Mode and channel must be recorded per capture. A JVM-labeled file does not establish a particular 410H channel or complete 12-mode coverage. |
| Peavey 5150 / related 6505 family | Original 5150 guide documents Rhythm/Lead, pre/post gain, shared tone controls, Presence/Resonance and high/normal input pad. Current 6505 1992 Original reference is 120 W with four 6L6GC tubes | Do not conflate a vintage Block Letter capture with a modern reissue or 5150 II/III. Gain and power-feedback controls need measured data. The current product page's preamp tube count is internally inconsistent (five in its body, six in an icon), so that count is not used as an implementation fact. |
| Mesa/Boogie Mark IV | Owner manual gives approximately 85 W in Simul-Class/Pentode and 70 W in Simul-Class/Triode; standard four 6L6s; alternate outer EL34 pair documented; three modes and graphic EQ | Power mode, lead voicing and graphic-EQ state are part of the capture identity. The five-band graphic EQ is not implemented by this fork's existing three-band post-EQ. |
| Soldano SLO-100 Classic | 100 W, two channels; five 12AX7/ECC83 and four 5881/6L6; Preamp/Master, three-band EQ, Presence and current Depth control | Keep Normal/Overdrive and vintage/current Depth variants separate. The supplied AMP-SLO1-3 capture is an amp-only snapshot, not an independently verified full-range control model. |

### Manufacturer sources

- [Fender current '65 Twin Reverb](https://www.fender.com/products/65-twin-reverb) and [owner manual](https://www.fmicassets.com/Damroot/Original/10001/021730_gamp_manual_all_revE.pdf).
- [Vox AC15 Custom specifications](https://voxamps.com/product/the-vox-ac15-custom/).
- [Marshall JVM410H support/specifications](https://www.marshall.com/us/en/support/amps/support-for-jvm410-head).
- [Peavey original 5150 guide](https://assets.peavey.com/literature/manuals/80302108.pdf), [6505 1992 Original](https://peavey.com/en-int/collections/amplifiers/products/6505-1992-original-guitar-amp-head) and [reissue guide](https://assets.peavey.com/literature/manuals/119251_38557.pdf).
- [Mesa Mark IV manufacturer manual](https://mesa-boogie.imgix.net/media/User%20Manuals/Mark%204.pdf), particularly its power-switch discussion.
- [Soldano SLO-100 Classic specifications](https://www.soldano.com/products/classic/amplifiers/slo-100-classic/).

## What actually creates the amp sound

The plug-in runs the downloaded NAM model weights. Copying wattage, tube counts or a control list into JSON cannot produce an accurate circuit or neural emulation. Those facts are research references, not DSP coefficients. The existing Low/Mid/High controls are now explicitly called **Post EQ**; they are not presented as the original tone stack of all six amplifiers.

The six starter captures cover Twin, AC15 Top Boost, JVM crunch, 5150 Block Letter, Mark IV and SLO100. The first five come from the [GPL-3.0-declared NAM community collection](https://github.com/pelennor2170/NAM_models). The SLO100 comes from [VoLum](https://github.com/guitarlum/VoLum), whose repository carries the MIT license and credits profiles to Lum. Original file names, weight bytes, credits, licenses and SHA-256 hashes are retained.

The five older WaveNet files have no sample-rate, input-calibration or capture-setting metadata. The upstream 48 kHz fallback is retained and explicitly documented. The newer VoLum SLO file declares 48 kHz and uses format 0.7.0 SlimmableContainer. None of these were trained or compared to physical hardware during this work. Filename ESR labels are the authors' labels, not measurements obtained here.

Three short cabinet IRs from [Jester Dyne Productions](https://www.jester-dyne-productions.com/brutal-ir-pack/) are included: V30/SM57, DV-77/SM57 and Rockdriver Junior/e606 in a modified BG412S cabinet. Their source mirror preserves the creator's CC0 notice. These are useful choices, but are not claimed to be matching Twin Jensen or AC15 Greenback cabinets.

## Useful findings in Awesome Audio DSP

[BillyDM's collection](https://github.com/BillyDM/awesome-audio-dsp) is a reference index; its README says ongoing maintenance moved to Codeberg. Relevant leads are:

| Project | Useful next step | Licensing observed / compatibility |
|---|---|---|
| [chowdsp_wdf](https://github.com/Chowdhury-DSP/chowdsp_wdf) | Build measured circuit models for original drive/fuzz pedals | Header-only C++ circuit-modeling library; repository states BSD-3-Clause. |
| [BYOD](https://github.com/Chowdhury-DSP/BYOD) | Study pedal topology, routing and analog-modeling approaches | Guitar distortion suite; GPL-3.0/dual-license terms require per-component review before integration. |
| [Airwindows](https://github.com/airwindows/airwindows) | Explore tape, saturation and dynamics modules | Repository states MIT. No Airwindows code is copied into the new prototype pedals. |
| [RTNeural](https://github.com/jatinchowdhury18/RTNeural) | Future Proteus/AIDA-X inference support | BSD-3-Clause inference library; it does not make those model formats automatically loadable by the NAM loader. |
| [GuitarML ToneLibrary](https://github.com/GuitarML/ToneLibrary) | Alternative trained models and capture workflows | Its README explicitly warns that different GuitarML architectures are not interchangeable. Not bundled as NAM files. |
| Dragonfly Reverb, Cocoa Delay and the DSP cookbooks | Future room/tape-delay and filter improvements | Research candidates, not claimed integrated effects. Check each upstream license and validate any later port. |

The collection's UX section recommends keeping the main controls obvious and secondary parameters less prominent. That supports the drawer design without copying another plug-in's interface.

## Changes made after audit

| Finding | Action |
|---|---|
| Maximum-Drive PNG was truncated, producing the black bottom strip | Rebuilt all four frames from the intact atlas; fully decoded every PNG and every embedded preview image. |
| Fork version 0.1.0 entered an inherited NAM-version parser that did not support it | Added explicit 13-parameter v0.1 and 18-parameter v0.2 reading; initialize new pedals to defaults when loading old state. |
| Partial/corrupt state chunks could continue reading after failure | Added atomic guarded reading and invalid-offset/NaN/Inf rejection; unsupported or malformed versions return failure instead of asserting. |
| Resampling wrapper reset the encapsulated model using the host rate | Reset it at the model's processing rate; added a probe regression and multi-rate chain tests. |
| Resampler headers depended on unqualified iPlug constants | Made those defaults/constants standalone and qualified the resampler alias; retained original licenses and marked modifications. |
| Additional gate knob needed to follow the same bypass state | Apply gate enable/disable to both linked threshold controls; preview mirrors value and bypass between views. |
| Time effects need a declared decay tail | Extended the declared tail to at least ten seconds for the bounded echo feedback. |
| Windows artwork resources lacked the new frames | Added named reaction resources and config constants. Windows compilation remains untested. |
| AAX used template manufacturer/type identities | Replaced template identities with I.R.Evan/project-specific identifiers; selected the Harmonic category and disabled untested AudioSuite support. |

## Pedal drawer

The native source includes **Shut Up** (existing gate), **Riff Juice** (new generic soft-clip overdrive), **Heh Heh** (new mono echo), and **The Box** (existing cabinet bypass). Routing is gate → overdrive → NAM → Post EQ → cabinet → echo → DC blocker → output. New effects default off.

The new overdrive is a lightweight mathematical effect, not a claim to emulate a particular hardware pedal. It is not oversampled. Echo has 80–900 ms time, 0–50% mix, fixed 0.35 feedback, filtered repeats and smoothed controls. There is no reverb, chorus, tempo sync, stereo ping-pong or drag-to-reorder routing in this version. The drawer's cabinet button uses the same bypass parameter as the main IR control. The browser preview remains silent and demonstrates controls only.

## Smoke tests and limits

Evidence and test sources are included under `docs/evidence/` and `tests/`. `python3 scripts/run-smoke.py` rebuilds the standalone engine tests with CMake and a C++20 compiler.

- Six models × three real cabinet IRs × four host rates (44.1, 48, 96 and 192 kHz) × four block sizes (32, 64, 256 and 1024) = **288 integrated chain cases**. Five-model matrix: 240; added SLO matrix: 48. Each exercises gate, added drive, NAM/resampling, Post EQ, actual IR convolution, echo and DC blocking, including a +20 dB input-gain stress segment. All completed with finite, non-silent output. Exact rates and peaks are in the evidence CSV; this is a synthetic stimulus test, not a musical listening or hardware-match test.
- Pedal tests passed for exact fresh/reset bypass, drive signal change, impulse delay timing/feedback, reset-to-silence, block-boundary independence and no allocations inside the new pedal Process methods at 44.1/48/96/192 kHz.
- AddressSanitizer and UndefinedBehaviorSanitizer passed on the new pedal tests with leak detection disabled. LeakSanitizer could not inspect processes in this environment; leak detection is not claimed passed.
- State reader passed real iPlug byte-chunk round trips for both parameter counts; every truncated prefix, invalid offsets and NaN/Inf values were rejected without changing the existing config.
- Interactive-preview DOM tests passed artwork thresholds (including +20 dB), drawer open/close, Escape/focus return, mirrored gate controls and no JavaScript runtime errors. A browser-pixel screenshot test could not run because the browser download was unavailable; the design PNG was rendered separately and visually inspected.
- Mac build scripts pass shell syntax checks; regenerated AU/AAX identity metadata was inspected. Source ZIP integrity, manifest hashes, embedded-image decode and required-file checks pass.

### Still unverified / release gates

- Native macOS GUI and AU/VST3/AAX compilation; Apple Silicon/Intel binaries; installer execution and DAW scanning.
- Real Pro Tools AAX loading, automation, session recall, bypass, offline bounce and multichannel configurations. The state reader test is not a substitute for an in-host round trip.
- Hardware A/B, calibrated input levels, exact knob-range modeling, matched-cabinet responses and perceptual quality. Specifications alone do not establish these.
- Full-chain real-time allocation behavior, worst-case CPU on the user's Mac, and non-finite input handling throughout upstream processors. Allocation tests cover the new pedals only.
- The non-oversampled drive needs listening/aliasing review before a polished release. Output gain can still exceed full scale when the user boosts it; the interface meters are not a limiter.

## Pro Tools requirement

AAX Native is the Pro Tools target; AU/VST3 files are not substitutes. `scripts/build-aax-dev.command` is an **unsigned development build** script, not an installer. It stops clearly when the SDK is absent. The bundled `AAX_SDK` directory is an upstream placeholder, not the actual Avid SDK. A compatible SDK, macOS/Xcode build, required AAX signing, and a real Pro Tools test are still needed.

[Avid's AAX developer page](https://developer.avid.com/aax/) describes access to the SDK/toolkit and development requirements. [PACE's current signing guide](https://paceap.com/getting-started-with-aax-code-signing-for-pro-tools-plugins/) states that a valid signature is required for commercial Pro Tools and describes starting that process through Avid. Apple ad-hoc signing does not satisfy the AAX signing requirement. See `PRO-TOOLS-AAX.md` for the concrete build/validation handoff.

## Interface and installer revision — October 24, 2026

- Native pedal drawer now uses machined-style stomp switches, on/bypass LEDs, recessed tread plates, screw heads and enclosure highlights. These reuse IVToggleControl's parameter handling; the audio engine and parameter identities are unchanged.
- Browser preview adds matching pedal details and chassis shading. Its interaction tests pass after the styling changes. The static design render was visually inspected; it is not a screenshot from a compiled native plug-in.
- Mac AU/VST3 packaging now includes all bundled models, IRs and license notices at `/Library/Application Support/I.R.Evan/Beavis And Butthead/models`. A top-level `Build-Mac.command` launches the build. Shell syntax checks pass.
- Environment check: Linux; xcodebuild unavailable. Native compilation, installation, Apple signing/notarization and Pro Tools/AAX validation remain unperformed. No compiled Mac installer is included. Existing DSP test results apply to the unchanged DSP source, not to native compilation of the new visual controls.

## Release preparation and rerun — October 24, 2026

Rebuilt the shared NAM test harness and reran all 288 model/cabinet/rate/buffer cases successfully. Pedal DSP and state-reader suites passed; browser DOM tests passed. Raw rerun output is retained in `evidence/release-smoke-output.txt`, with status in `evidence/release-checks.json`.

Added `scripts/sign-mac-release.command` for Developer ID AU/VST3 signing, signed PKG creation, explicit notarization acceptance checking, stapling, Gatekeeper assessment and signed/notarized DMG packaging. Added installed Mac bundle/AU/VST3 validator runner. Both scripts pass Bash syntax checks. `tests/release_script_flow.py` passed mocked rejection/acceptance flow checks; those mocks do not sign software or prove Apple's acceptance.

Apple certificates, a Mac builder and Avid/PACE setup remain unavailable. Native build/signing/Pro Tools test rows are explicitly NOT RUN in `RELEASE-HOST-TESTS.csv`. No compiled installer is present.
