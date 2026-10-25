# Version 0.2.1 validation
Build commit: 2e10382d18d6d042b54b3a7b622e5498a400b459.
Mac build: https://github.com/trendyspenders-hub/BeavisButthead/actions/runs/40161827115
Installer artifact: https://github.com/trendyspenders-hub/BeavisButthead/actions/runs/40161827115/artifacts/12587200097

## Passed
- Factory catalog: all six NAM paths, three cabinet paths, default identities, missing-library detection (local and macOS runner).
- Shared-engine smoke: 288 combinations across six captures, three IRs, four rates and four buffer sizes.
- Pedal DSP suite: bypass, drive, echo timing/feedback, silence/reset, block independence, no allocations in new pedal Process methods.
- State-reader suite: 13/18-parameter byte-chunk round trips, truncation/invalid input rejection and atomic update.
- Universal arm64/x86_64 AU and VST3 builds.
- Ad-hoc bundle signature verification.
- PKG/DMG generation, installation on macOS runner, AU VALIDATION SUCCEEDED.

Artifact ZIP SHA-256: 5a8119a41a563ba629faf31a8141b00f91dece68e2bff4575c929cabcd5f5b50
DMG: Beavis-And-Butthead-by-IR-Evan-0.2.1-unsigned.dmg

## Limits
No actual DAW GUI interaction, musical listening, VST3 validator, or in-host empty/custom/missing-file session recall test was performed. The state-reader test does not cover all native lifecycle behaviour. Circuit-based replicas are not implemented; see CIRCUIT-REPLICATION.md. Pro Tools/AAX remains deferred. Installer is unsigned and not notarized; bundles are ad-hoc signed.

Use the new main-panel factory buttons described in FACTORY-AMPS.md. New Mac instances load the SLO capture and V30 cabinet when installed content is available.
