#!/bin/bash
# Sign a fresh copy of an already built AU/VST3 payload. Never edits the input.
set -euo pipefail
[[ "$(uname -s)" == Darwin ]] || { echo 'Requires macOS, Xcode, and installed Developer ID certificates.' >&2; exit 1; }
[[ $# == 1 ]] || { echo 'Usage: bash scripts/sign-mac-release.command /absolute/path/mac-build-XXXXXX' >&2; exit 2; }
: "${IREVAN_APP_IDENTITY:?Set to your Developer ID Application identity in Keychain}"
: "${IREVAN_INSTALLER_IDENTITY:?Set to your Developer ID Installer identity in Keychain}"
: "${IREVAN_NOTARY_PROFILE:?Set to a notarytool Keychain profile name}"
[[ "$IREVAN_APP_IDENTITY" == 'Developer ID Application:'* ]] || { echo 'A Developer ID Application identity is required.' >&2; exit 2; }
[[ "$IREVAN_INSTALLER_IDENTITY" == 'Developer ID Installer:'* ]] || { echo 'A Developer ID Installer identity is required.' >&2; exit 2; }
TASK_BUILD="$(cd "$1" && pwd)"
TASK_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
for TASK_EXT in component vst3; do
  test -f "$TASK_BUILD/payload/Library/Audio/Plug-Ins/$( [[ "$TASK_EXT" == component ]] && echo Components || echo VST3 )/BBIREvan.$TASK_EXT/Contents/MacOS/BBIREvan"
done
TASK_OUT="$(mktemp -d "$TASK_ROOT/mac-release-XXXXXX")"
ditto "$TASK_BUILD/payload" "$TASK_OUT/payload"
mkdir "$TASK_OUT/disk"
for TASK_FORMAT in Components/BBIREvan.component VST3/BBIREvan.vst3; do
  TASK_BUNDLE="$TASK_OUT/payload/Library/Audio/Plug-Ins/$TASK_FORMAT"
  lipo -verify_arch arm64 x86_64 "$TASK_BUNDLE/Contents/MacOS/BBIREvan"
  codesign --force --options runtime --timestamp --sign "$IREVAN_APP_IDENTITY" "$TASK_BUNDLE"
  codesign --verify --deep --strict --verbose=2 "$TASK_BUNDLE"
done
TASK_PKG="$TASK_OUT/disk/Beavis-And-Butthead-by-IR-Evan-0.2.0.pkg"
pkgbuild --root "$TASK_OUT/payload" --identifier com.IREvan.BBIREvan.installer \
  --version 0.2.0 --install-location / --sign "$IREVAN_INSTALLER_IDENTITY" --timestamp "$TASK_PKG"
pkgutil --check-signature "$TASK_PKG" | tee "$TASK_OUT/installer-signature.txt"
# notarytool's exit status alone is not the acceptance check.
xcrun notarytool submit "$TASK_PKG" --keychain-profile "$IREVAN_NOTARY_PROFILE" --wait --output-format json > "$TASK_OUT/pkg-notary.json"
python3 -c 'import json,sys; r=json.load(open(sys.argv[1])); assert r.get("status")=="Accepted",r' "$TASK_OUT/pkg-notary.json"
xcrun stapler staple "$TASK_PKG"
xcrun stapler validate "$TASK_PKG"
spctl --assess --type install --verbose=2 "$TASK_PKG"
cp "$TASK_ROOT/docs/START-HERE.md" "$TASK_OUT/disk/Read-Me.txt"
TASK_DMG="$TASK_OUT/Beavis-And-Butthead-by-IR-Evan-0.2.0.dmg"
hdiutil create -volname 'Beavis And Butthead by I.R.Evan' -srcfolder "$TASK_OUT/disk" -format UDZO "$TASK_DMG"
codesign --timestamp --sign "$IREVAN_APP_IDENTITY" "$TASK_DMG"
xcrun notarytool submit "$TASK_DMG" --keychain-profile "$IREVAN_NOTARY_PROFILE" --wait --output-format json > "$TASK_OUT/dmg-notary.json"
python3 -c 'import json,sys; r=json.load(open(sys.argv[1])); assert r.get("status")=="Accepted",r' "$TASK_OUT/dmg-notary.json"
xcrun stapler staple "$TASK_DMG"
xcrun stapler validate "$TASK_DMG"
shasum -a 256 "$TASK_PKG" "$TASK_DMG" > "$TASK_OUT/SHA256SUMS.txt"
echo "Signed and notarized AU/VST3 packages: $TASK_OUT"
echo 'Pro Tools/AAX is not included. Host testing is still required.'
