# Factory amp access — 0.2.1

The main panel has six amp-capture buttons and three cabinet buttons. Select an amp and cabinet independently; the file controls below show the selected filenames and still accept custom NAM/WAV files.

New Mac instances attempt to load SLO with the V30/SM57 cabinet from the content installed by the full package. No editor opening is required. Saved session/preset state replaces these defaults, including empty paths; a missing saved file must not silently play the default amp. Missing factory content produces an actionable message when selected.

These are the existing licensed NAM captures, not the circuit-derived replicas described in CIRCUIT-REPLICATION.md. The five legacy captures have unverified cabinet inclusion and settings; use cabinet bypass when appropriate. SLO is publisher-labeled amp-only. Post EQ remains global Post EQ.

## Verification

The catalog test checks all six NAM and three IR paths, default identities and missing-library detection against real packaged files. Native Mac build and AU validation run in GitHub Actions. Actual DAW interaction, saved empty/custom sessions and missing-file session recall still require in-host testing. AAX remains deferred. Ad-hoc signed test installers are not Developer ID signed or notarized.
