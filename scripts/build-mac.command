#!/bin/bash
set -euo pipefail
TASK_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
[[ "$(uname -s)" == Darwin ]] || { echo 'Run this on macOS with full Xcode installed.'; exit 1; }
xcodebuild -version
if [[ ! -d "$TASK_ROOT/iPlug2" ]]; then python3 "$TASK_ROOT/scripts/bootstrap-dependencies.py"; fi
# Vendored source is included. The platform SDK and graphics libraries are downloaded by upstream helpers.
if [[ ! -f "$TASK_ROOT/iPlug2/Dependencies/IPlug/VST3_SDK/pluginterfaces/base/funknown.h" ]]; then
  TASK_SDK="$TASK_ROOT/iPlug2/Dependencies/IPlug/VST3_SDK"
  TASK_SDK_STAGE="$(mktemp -d "$TASK_ROOT/vst3-sdk-XXXXXX")"
  git init "$TASK_SDK_STAGE"
  git -C "$TASK_SDK_STAGE" remote add origin https://github.com/steinbergmedia/vst3sdk.git
  git -C "$TASK_SDK_STAGE" fetch --depth 1 origin 3cdf9ca5d1f5b1b21e0a86832aa4abe55607bd96
  git -C "$TASK_SDK_STAGE" checkout --detach FETCH_HEAD
  git -C "$TASK_SDK_STAGE" submodule update --init --recursive
  if [[ -e "$TASK_SDK" ]]; then mv "$TASK_SDK" "$TASK_SDK_STAGE-placeholder"; fi
  mv "$TASK_SDK_STAGE" "$TASK_SDK"
fi
if [[ ! -d "$TASK_ROOT/iPlug2/Dependencies/Build/mac" ]]; then
  (cd "$TASK_ROOT/iPlug2/Dependencies" && bash download-prebuilt-libs.sh)
fi
(cd "$TASK_ROOT/BBIREvan/scripts" && python3 update_version-mac.py)
TASK_OUT="$(mktemp -d "$TASK_ROOT/mac-build-XXXXXX")"
TASK_PRODUCTS="$TASK_OUT/products"
mkdir -p "$TASK_PRODUCTS" "$TASK_OUT/payload/Library/Audio/Plug-Ins/Components" "$TASK_OUT/payload/Library/Audio/Plug-Ins/VST3" "$TASK_OUT/disk"
for TASK_TARGET in AU VST3; do
  xcodebuild -project "$TASK_ROOT/BBIREvan/projects/BBIREvan-macOS.xcodeproj" \
    -xcconfig "$TASK_ROOT/BBIREvan/config/BBIREvan-mac.xcconfig" \
    -target "$TASK_TARGET" -configuration Release \
    ARCHS='arm64 x86_64' ONLY_ACTIVE_ARCH=NO \
    CODE_SIGNING_ALLOWED=NO CODE_SIGNING_REQUIRED=NO DEVELOPMENT_TEAM= \
    CONFIGURATION_BUILD_DIR="$TASK_PRODUCTS" SYMROOT="$TASK_OUT/build" \
    OBJROOT="$TASK_OUT/obj" build | tee "$TASK_OUT/$TASK_TARGET.log"
done
for TASK_EXT in component vst3; do
  TASK_BUNDLE="$TASK_PRODUCTS/BBIREvan.$TASK_EXT"
  test -f "$TASK_BUNDLE/Contents/MacOS/BBIREvan"
  for TASK_FRAME in 0 1 2 3; do test -f "$TASK_BUNDLE/Contents/Resources/Reaction$TASK_FRAME.png"; done
  lipo -verify_arch arm64 x86_64 "$TASK_BUNDLE/Contents/MacOS/BBIREvan"
  codesign --force --sign - "$TASK_BUNDLE"
  codesign --verify --strict "$TASK_BUNDLE"
done
cp -R "$TASK_PRODUCTS/BBIREvan.component" "$TASK_OUT/payload/Library/Audio/Plug-Ins/Components/"
cp -R "$TASK_PRODUCTS/BBIREvan.vst3" "$TASK_OUT/payload/Library/Audio/Plug-Ins/VST3/"
# Install captures and their provenance in a stable location for session recall.
TASK_CONTENT="$TASK_OUT/payload/Library/Application Support/I.R.Evan/Beavis And Butthead"
mkdir -p "$TASK_CONTENT"
cp -R "$TASK_ROOT/models" "$TASK_CONTENT/models"
cp "$TASK_ROOT/docs/START-HERE.md" "$TASK_CONTENT/Read-Me.txt"
pkgbuild --root "$TASK_OUT/payload" --identifier com.IREvan.BBIREvan.installer --version 0.2.0 --install-location / \
  "$TASK_OUT/disk/Beavis-And-Butthead-by-IR-Evan-0.2.0-unsigned.pkg"
cp "$TASK_ROOT/docs/START-HERE.md" "$TASK_OUT/disk/Read-Me.txt"
hdiutil create -volname 'Beavis And Butthead by I.R.Evan' -srcfolder "$TASK_OUT/disk" -format UDZO \
  "$TASK_OUT/Beavis-And-Butthead-by-IR-Evan-0.2.0-unsigned.dmg"
echo "Built personal-test packages at: $TASK_OUT"
echo 'Ad-hoc signed binaries; installer is unsigned and not notarized. DAW validation remains required.'
