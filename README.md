# Beavis And Butthead by I.R.Evan

## Mac test installer available

[Download the Mac installer artifact](https://github.com/trendyspenders-hub/BeavisButthead/actions/runs/40120416123/artifacts/12571770280) (GitHub sign-in may be required). Unzip, open the DMG and run the PKG inside. The artifact also contains the same standalone PKG.

Includes AU and VST3 for Apple Silicon and Intel, six NAM amp captures and three cabinet IRs. Select models from `/Library/Application Support/I.R.Evan/Beavis And Butthead/models` in the plug-in's model and cabinet browsers.

Both native builds, installer creation, installation on macOS 14.8.9 and Apple's Audio Unit validation passed. VST3 compiled but its separate host validator was not run. The plug-in bundles are ad-hoc signed; the installer is unsigned and not Apple-notarized. Pro Tools/AAX is excluded. Native Intel DAW and full music-session testing remain outstanding.

The older source-package documentation inside the test installer predates the successful Mac build; this README and `docs/evidence/mac-installer-validation.json` record the current status.

## Build

On macOS with full Xcode, run `bash scripts/build-mac.command`. It fetches pinned engine dependencies and unchanged original resources. The GitHub Actions workflow builds the universal test installers and validates the installed AU. See `docs/FINISH-RELEASE.md` for Developer ID signing and notarization. No signing secrets belong in this repository.
