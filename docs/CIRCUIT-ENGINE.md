# Circuit preamp engine — 0.3.0

Two playable experimental preamps now run independently of NAM files: **AB763 Normal** and **1978 2203 high-input**. Select their tabs on the main panel. New instances select AB763; old saved states select NAM and preserve the former parameter order. Factory capture buttons return to NAM.

These are preamp implementations, not complete amplifier replicas. They do not model phase-inverter/output valves, transformer magnetics, power-supply sag, negative-feedback Presence, electrical speaker impedance, reverb or tremolo. Existing cabinet IRs provide acoustic filtering, not those missing circuits. Vox, Mark IV, 5150 and SLO remain NAM captures; their circuit engines have not been implemented.

## Implemented circuitry

- AB763: high-input resistance, first valve/cathode bypass, plate-driven 100k/250p/100n/47n tone network, interacting controls, Volume and switchable 120p Bright, second valve with shared 820-ohm cathode and the idle Vibrato recovery valve's bias contribution, output coupling capacitor.
- 2203: high-input first valve with 2.7k/680n cathode network; gain/bright and interstage shaping; unbypassed 10k cold-biased stage; third gain valve; direct-coupled cathode follower; 33k/500p/22n/22n tone network and three-terminal Mid pot; pre-phase-inverter Master.
- Tube plate currents depend on grid and plate voltages. Grid conduction, cathode capacitors, coupling-capacitor charge and generic interelectrode capacitances participate in the nodal solution. There is no NAM or static distortion preset hidden inside these preamps.

## Numerical method and explicit approximations

The original C++ implementation uses modified nodal equations reduced to nonlinear tube ports, bounded Newton iterations, fixed-size matrices and backward-Euler capacitor companions. Backward Euler replaced the initial trapezoidal prototype after extreme burst/release testing exposed instability. It adds numerical damping; reference tests measure its error rather than assuming an exact analog match.

Processing runs at 4x host rate with 65-tap Blackman-windowed sinc filters before and after the network. Reported FIR latency is 16 host samples. This filter is not an assurance of inaudible aliasing at every setting. Controls slew at a 20 ms time constant; matrix updates occur at a fixed sample count independent of host buffer size. Engine changes reinitialize the selected circuit and ramp the output. The solver and engine allocate no memory while processing; the host wrapper retains its upstream allocation/staging behaviour.

Reference rails are stiff 410 V (AB763) and 320 V (2203). The 2203 rail is an approximation, not a measured vintage amplifier supply. Generic Koren 12AX7 parameters are used, with smooth resistive grid conduction and generic 2.3/2.4/0.9 pF capacitances. Pot tapers are approximated (Gain/Bass/Master logarithmic; Treble/Mid linear), not measured. Pot endpoints retain 1 ohm for numerical conditioning. The AB763 output sees an approximate 400k mixer/phase-inverter load; the 2203 output sees 1M. The isolated idle Vibrato recovery is a bias approximation; its full surrounding signal path is omitted.

Input Trim is digital gain before the circuit; one digital unit maps to one volt at its input. Output conversion is a fixed scale (AB763 0.02, 2203 0.03), not hardware output calibration or loudness normalization. NAM calibration/normalization does not apply to circuit modes. The separately labeled Post EQ remains downstream of the selected engine. AB763 has Bright; 2203 has Master. Unused controls are hidden.

## Source basis

The drawings and verification scope are recorded in [CIRCUIT-REPLICATION.md](CIRCUIT-REPLICATION.md). Directly inspected sources:

- https://schematicheaven.net/fenderamps/twin_reverb_ab763_schem.pdf
- https://schematicheaven.net/marshallamps/jmp_mastervol_100w_2203u.pdf
- https://www.normankoren.com/Audio/Tubemodspice_article.html

## Tests and remaining verification

`tests/circuit_dsp.cpp` checks 24 model/rate/control extremes at 44.1/48/96/192 kHz, loud-to-quiet bursts, finite output, convergence, silence, reset, block independence, moving controls, invalid input and no processing allocations. `tests/circuit_spice.py` compares DC operating points and 440 Hz transient waveforms at 0.01/0.5/2 V against an independent ngspice solver using the exported component ledger and device equations. It rejects DC errors above 1 mV and relative waveform RMS errors above 3%. This verifies numerical agreement, not an independent schematic transcription or a physical amplifier.

The state-reader suite covers 13/18/25 parameters and malformed chunks. Native AU/VST3 builds and installed AU validation are separate CI checks. Actual DAW GUI interaction, automation timing/latency changes, hardware A/B, listening quality, alias spectra and worst-case real-time CPU on the user's Mac still require verification. Pro Tools remains deferred; test installers are not notarized.
