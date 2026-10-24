#!/bin/bash
set -euo pipefail
TASK_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
[[ "$(uname -s)" == Darwin ]] || { echo 'AAX development build requires macOS and full Xcode.'; exit 1; }
TASK_SDK="$TASK_ROOT/iPlug2/Dependencies/IPlug/AAX_SDK"
if [[ ! -f "$TASK_SDK/Interfaces/AAX.h" || ! -d "$TASK_SDK/Libs/AAXLibrary/MacBuild/AAXLibrary.xcodeproj" ]]; then
  echo 'Avid AAX SDK is missing. The included AAX_SDK folder is only an upstream placeholder.'
  echo "Obtain the AAX evaluation toolkit from https://developer.avid.com/aax/ and place the compatible SDK in: $TASK_SDK"
  echo 'No Pro Tools compatible binary has been created.'
  exit 2
fi
if [[ ! -d "$TASK_ROOT/iPlug2/Dependencies/Build/mac" ]]; then
  (cd "$TASK_ROOT/iPlug2/Dependencies" && bash download-prebuilt-libs.sh)
fi
(cd "$TASK_ROOT/BBIREvan/scripts" && python3 update_version-mac.py)
TASK_OUT="$(mktemp -d "$TASK_ROOT/aax-dev-build-XXXXXX")"
xcodebuild -project "$TASK_ROOT/BBIREvan/projects/BBIREvan-macOS.xcodeproj" \
  -xcconfig "$TASK_ROOT/BBIREvan/config/BBIREvan-mac.xcconfig" \
  -target AAX -configuration Release ARCHS='arm64 x86_64' ONLY_ACTIVE_ARCH=NO DEPLOYMENT_LOCATION=NO \
  CODE_SIGNING_ALLOWED=NO CODE_SIGNING_REQUIRED=NO DEVELOPMENT_TEAM= \
  CONFIGURATION_BUILD_DIR="$TASK_OUT/products" SYMROOT="$TASK_OUT/build" OBJROOT="$TASK_OUT/obj" build \
  | tee "$TASK_OUT/AAX-build.log"
TASK_BUNDLE="$TASK_OUT/products/BBIREvan.aaxplugin"
test -f "$TASK_BUNDLE/Contents/MacOS/BBIREvan"
lipo "$TASK_BUNDLE/Contents/MacOS/BBIREvan" -verify_arch arm64 x86_64
for TASK_FRAME in 0 1 2 3; do test -f "$TASK_BUNDLE/Contents/Resources/Reaction$TASK_FRAME.png"; done
echo "UNSIGNED DEVELOPMENT AAX: $TASK_BUNDLE"
echo 'This is not a release package. PACE signing and testing in Pro Tools are still required.'
echo 'See docs/PRO-TOOLS-AAX.md. An Apple ad-hoc signature is not a substitute for AAX signing.'
