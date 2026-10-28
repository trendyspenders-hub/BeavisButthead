# Beavis And Butthead by I.R.Evan

## 0.3.0 circuit-preamp test edition

Two experimental circuit preamps — **AB763 Normal** and **1978 2203 high input** — run independently of external NAM files. New instances select AB763. The six factory NAM captures and three cabinet IRs remain available; factory amp buttons switch back to NAM. Older sessions retain NAM mode.

These are component-network **preamps**, not complete amplifier replicas. Power stages, phase inverters, transformers, sag and feedback are omitted. Generic tube parameters and some circuit loads are approximated. See [circuit implementation and limitations](docs/CIRCUIT-ENGINE.md).

Universal Apple Silicon/Intel AU and VST3 are packaged by [GitHub Actions](https://github.com/trendyspenders-hub/BeavisButthead/actions/workflows/build-irevan.yml). Download the installer artifact from a successful run, unzip, open the DMG and run its PKG. GitHub sign-in may be required. Pro Tools/AAX is excluded.

Plug-in bundles are ad-hoc signed. The installer is unsigned and not Apple-notarized. AU validation, circuit stress/SPICE tests, and other recorded checks are described in `docs/evidence`; separate VST3 host validation, manual DAW GUI/listening tests and physical-amplifier comparisons remain outstanding.

## Build

On macOS with full Xcode, run `bash scripts/build-mac.command`. It fetches pinned dependencies and resources, builds both architectures and packages AU/VST3. The workflow installs the package and validates the registered Audio Unit. See [installation notes](docs/START-HERE.md) and [release signing](docs/FINISH-RELEASE.md). No signing secrets belong in this repository.
