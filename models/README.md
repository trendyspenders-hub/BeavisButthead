# Amp and cabinet starter files

Load a `.nam` in the native plug-in's AMP browser and a `.wav` in its CAB browser. These are playable model files, not newly trained or independently hardware-verified emulations. Keep this directory in a permanent location before saving DAW sessions; the native plug-in stores external file paths.

| File / family | Source credit | Repository-declared license |
|---|---|---|
| Fender TwinVerb Norm Bright | Tim R, NAM community collection | GPL-3.0 |
| Vox AC15 TopBoost | Phillipe P, NAM community collection | GPL-3.0 |
| JVM CR OR NoDrive | Phillipe P, NAM community collection | GPL-3.0 |
| 5150 BlockLetter NoBoost | Helga B, NAM community collection | GPL-3.0 |
| Mesa Mark IV | Roman A, NAM community collection | GPL-3.0 |
| AMP-SLO1-3 / Soldano SLO100 | Lum, VoLum project | MIT |

The five older community files are format 0.5.0 WaveNet models without sample-rate, input-calibration or capture-setting metadata. The NAM wrapper follows the upstream 48 kHz fallback for missing sample rates; this is an assumption, not independently verified provenance. Cabinet inclusion for those five has not been verified. Use listening comparisons and bypass the extra cabinet when a model already contains a speaker response.

The VoLum Soldano file is a 48 kHz format 0.7.0 SlimmableContainer, provided as AMP/no-cab by its publisher. Its sample-rate metadata is explicit. The capture's exact original amp year/knob settings have not been independently verified.

`cabinets/` contains three short 48 kHz cabinet IRs from Jester Dyne Productions: V30/SM57, DV-77/SM57, and Rockdriver Junior/e606, all from a modified Behringer BG412S cabinet. These are not the stock Jensen speakers of a Twin or a stock AC15 Greenback. They provide useful rock-cab choices, not verified matching cabinets for every amp.

Licenses and creator credit remain with the files. `manifest.json` records hashes, provenance, format versions and assumptions. `community-starter/COPYING` contains the collection's GPL text; `volum/LICENSE` contains the VoLum MIT license; `cabinets/LICENSE.txt` retains its CC0 source notice. No model weights were altered.
