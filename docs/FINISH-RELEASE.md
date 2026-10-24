# Finish the Mac and Pro Tools release

This source package is not an installer. No Mac binary has been compiled here and no signing operation has been performed. The source-side tests and packaging scripts are separate from native host validation.

## Build and test AU/VST3

1. On a Mac with full Xcode, Git and Python 3, unpack the source to a path without spaces and run `bash scripts/build-mac.command`. The generated unsigned PKG/DMG contains AU, VST3, six amp captures and three IRs.
2. Install the generated test PKG on a test Mac. Run `bash scripts/validate-mac-installed.command`. It verifies bundle resources/signatures/architectures and runs Apple's auval. Set `IREVAN_VST3_VALIDATOR` to the Steinberg SDK validator executable to include VST3 validation; otherwise the result explicitly records NOT RUN.
3. Complete `docs/RELEASE-HOST-TESTS.csv` in the actual DAWs. A source smoke test is not a DAW test.

## Apple signing and notarization

The account holder must provision Developer ID Application and Developer ID Installer certificates with their private keys in the build Mac's Keychain. Store notarization credentials locally with `xcrun notarytool store-credentials`; use its interactive prompts. Do not put certificate passwords or private keys into chat or the source archive.

Set the following environment variables in the Mac terminal using your actual certificate display names and Keychain profile:

```bash
export IREVAN_APP_IDENTITY='Developer ID Application: YOUR NAME (TEAMID)'
export IREVAN_INSTALLER_IDENTITY='Developer ID Installer: YOUR NAME (TEAMID)'
export IREVAN_NOTARY_PROFILE='irevan-notary'
bash scripts/sign-mac-release.command /absolute/path/to/mac-build-XXXXXX
```

The script preserves the development build, signs copies of AU/VST3 with hardened runtime/timestamps, signs the installer, submits it to Apple, requires Accepted status, staples/validates its ticket, checks Gatekeeper, then signs/notarizes/staples a DMG. It retains notarization JSON and final SHA256 checksums. It fails instead of claiming success when prerequisites or acceptance are missing. This workflow has been shell-checked on Linux but not executed on macOS.

Install and retest the exact final signed package on a clean Mac. Apple's acceptance does not verify audio quality or DAW compatibility.

## Pro Tools / AAX

The Apple workflow above does not include AAX. Supply a compatible AAX SDK and run `bash scripts/build-aax-dev.command`. Follow Avid/PACE's current onboarding and signing process in `docs/PRO-TOOLS-AAX.md`. Use the signing command and credentials issued for your actual PACE setup; there is no fabricated universal signing identity in this project. Test the exact PACE-signed AAX in normal Pro Tools, then package it at `/Library/Application Support/Avid/Audio/Plug-Ins/BBIREvan.aaxplugin`. Preserve the signed bundle unchanged.

AAX release requires actual compilation, PACE signature verification, and the Pro Tools rows of the host checklist. Until then it remains incomplete.

## References

- https://developer.apple.com/developer-id/
- https://developer.apple.com/documentation/security/customizing-the-notarization-workflow
- https://developer.avid.com/aax/
- https://paceap.com/getting-started-with-aax-code-signing-for-pro-tools-plugins/
