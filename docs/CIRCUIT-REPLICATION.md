# Circuit replication research
Researched October 24, 2026. This is an engineering reference, not implemented amp DSP.

The current plug-in uses six NAM captures and generic Post EQ. Their filenames do not establish exact circuit revisions or capture settings. The factory selector/default is still an outstanding product fix.

## Verified starting targets

### Fender Twin Reverb AB763 Normal channel
Original drawing, page 1, visually inspected:
https://schematicheaven.net/fenderamps/twin_reverb_ab763_schem.pdf

Input: 1 MΩ and two 68 kΩ resistors with switched-jack routing.
First triode: 100 kΩ plate, 1.5 kΩ cathode, 25 µF bypass.
Tone network: 100 kΩ slope; 250 pF, 0.1 µF, 0.047 µF.
Pots: Treble/Bass 250 kΩ, Middle 10 kΩ, Volume 1 MΩ.
Bright switch: 120 pF across upper volume section.
Power section: 12AT7 phase inverter, four 6L6GC valves, transformer-secondary feedback through 820 Ω.

Model plate-source loading, interacting pots and Bright/Volume behaviour. Vibrato-channel reverb/recovery/modulation requires a separate extension. This vintage target must not silently inherit modern reissue specifications.

### Marshall 2203, drawing dated 11/10/78
Original Marshall/Unicord drawing, visually inspected:
https://schematicheaven.net/marshallamps/jmp_mastervol_100w_2203u.pdf

This drawing specifies 6550 output valves; it is not a generic EL34 JCM800 reference.
First stage: 100 kΩ plate, 2.7 kΩ cathode, 0.68 µF bypass.
Second stage: 100 kΩ plate, unbypassed 10 kΩ cathode.
Interstage shaping includes 22 nF coupling and 470 kΩ/470 pF networks.
Follower-driven tone stack: 33 kΩ slope, 500 pF and two 22 nF capacitors.
Treble 250 kΩ, Bass 1 MΩ, Middle 25 kΩ; 1 MΩ master before phase inverter.

Preserve input switching, stage bias, capacitor charging and follower loading. The existing JVM capture is a different amplifier and must retain its identity.

## Other targets and unresolved evidence

- **AC15C1 Top Boost:** VOX service-manual text identifies A500K Normal/Top Boost volumes, A1M Bass, B220K Tone Cut and R14 = 1.5 kΩ. Full node connectivity needs original-page verification. A separately inspected PDF marked “Drawn by inkomodo.dragon” is a third-party redraw, not factory authority. Keep C1, C2, CH and historic EF86 circuits separate.
  https://www.manualslib.com/manual/1520645/Vox-Ac15c1.html?page=2
- **Mark IV:** Factory-labeled scan page 1 shows input R201 = 150 kΩ, R251 = 1.5 kΩ, C321 = 0.47 µF; lead tone network between V1A/V1B with R211 = 100 kΩ, C11 = 250 pF, C242 = 0.1 µF, C232 = 0.047 µF. Trace LDR switching and resolve full-set revision before coding.
  https://schematicheaven.net/boogieamps/boogie_mkiv.pdf
  Manufacturer manual identifies input/tone-recovery and later lead stages; graphic-EQ centers are 80, 240, 750, 2200 and 6600 Hz. Centers alone do not establish bandwidth/gain range. Early tone shaping and later EQ must stay separate.
  https://mesa-boogie.imgix.net/media/User%20Manuals/Mark%204.pdf
- **5150 head:** Verify exact original-head revision, gain-stage routing, interstage networks, Presence/Resonance feedback and load. Do not substitute combo, II, modern 6505 or EVH III. Factory candidate:
  https://el34world.com/charts/Schematics/files/Peavey/5150_evh.pdf
  The separately inspected sectioned drawing at https://schematicheaven.net/newamps/peavey_evh5150.pdf lacks a factory title block on its first page; component values are not approved from that review.
- **SLO-100:** Current manufacturer manual documents power-section Presence/Depth and revised DC heaters, but supplies no complete netlist. Obtain a traceable revision-specific schematic; vintage redraws do not prove current circuitry.
  https://www.soldano.com/wp-content/uploads/2020/04/SOLDANO-SLO-100-INSTRUCTION-MANUAL-FINAL.pdf

## Implementation specification

1. Freeze revision, channel, switch positions and component/node ledger. Mark uncertain readings.
2. Build offline SPICE reference; check DC operating points and loaded AC response. Include pot tapers.
3. Model nonlinear tube current, cathode bypass, grid current and coupling-capacitor state. Memoryless clipping is insufficient to establish recovery behaviour.
4. Include phase inverter, output valves, feedback, supply dynamics, transformer and reactive load. Unknown transformer parameters require measurement or explicit approximation. Cabinet IRs do not alone model electrical speaker loading.
5. Implement passive blocks with circuit-derived filters/WDF/state space; select nonlinear solver and oversampling using error/alias/CPU measurements. Smooth controls without losing capacitor state.
6. Expose model-specific controls separately from optional Post EQ. Do not cascade a full circuit amp with a full-amp NAM capture and present it as one faithful amplifier.

## Methods and tools
- Yeh/Smith DAFx 2006 derives a third-order Bassman tone stack. Reference values: C1 250 pF, C2/C3 20 nF; R1 250 kΩ, R2 1 MΩ, R3 25 kΩ, R4 56 kΩ. Use as a solver benchmark, not the Twin/Marshall component set; revisit loading assumptions per target.
  https://www.dafx.de/paper-archive/2006/papers/p_001.pdf
- Koren's nonlinear tube-current models and fitting procedure are useful offline references. Generic tube fits do not validate an amplifier.
  https://www.normankoren.com/Audio/Tubemodspice_article.html
- Candidate C++ WDF infrastructure, not integrated in this pass:
  https://github.com/Chowdhury-DSP/chowdsp_wdf

## Required evidence — not yet run for new circuit models
Verify every component/node against drawings. Compare DC, AC gain/phase, transients, harmonics, intermodulation and loud-burst recovery against reference simulation. Sweep calibrated levels and controls. Test reactive-load stability, aliasing, worst-case CPU, allocations, silence/reset, automation, state recall and buffer independence at 44.1/48/96/192 kHz. Compare calibrated recordings of the exact hardware revision/channel/load before claiming hardware accuracy.

Existing NAM smoke tests and AU validation do not validate these unimplemented circuits. No new playable amp or installer is delivered by this document.
