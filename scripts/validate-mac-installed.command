#!/bin/bash
# Read-only checks of the installed AU/VST3. Does not install or delete anything.
set -euo pipefail
[[ "$(uname -s)" == Darwin ]] || { echo 'Requires macOS.' >&2; exit 1; }
TASK_ROOT="$(cd "$(dirname "$0")/.." && pwd)"
TASK_LOG="$(mktemp -d "$TASK_ROOT/mac-validation-XXXXXX")"
for TASK_FORMAT in Components/BBIREvan.component VST3/BBIREvan.vst3; do
  TASK_BUNDLE="/Library/Audio/Plug-Ins/$TASK_FORMAT"
  test -f "$TASK_BUNDLE/Contents/MacOS/BBIREvan"
  lipo -verify_arch arm64 x86_64 "$TASK_BUNDLE/Contents/MacOS/BBIREvan"
  codesign --verify --deep --strict --verbose=2 "$TASK_BUNDLE" 2>&1 | tee -a "$TASK_LOG/bundles.log"
  for TASK_FRAME in 0 1 2 3; do test -f "$TASK_BUNDLE/Contents/Resources/Reaction$TASK_FRAME.png"; done
done
# Test the registered Audio Unit, not a source stub.
auval -v aufx BbIr IrEv 2>&1 | tee "$TASK_LOG/auval.log"
grep -q 'AU VALIDATION SUCCEEDED' "$TASK_LOG/auval.log"
# Steinberg's validator is supplied/built separately from the VST3 SDK.
if [[ -n "${IREVAN_VST3_VALIDATOR:-}" ]]; then
  "$IREVAN_VST3_VALIDATOR" /Library/Audio/Plug-Ins/VST3/BBIREvan.vst3 2>&1 | tee "$TASK_LOG/vst3-validator.log"
else
  echo 'VST3 validation NOT RUN: set IREVAN_VST3_VALIDATOR to the SDK validator executable.' | tee "$TASK_LOG/vst3-NOT-RUN.txt"
fi
echo "Validation evidence: $TASK_LOG"
echo 'These checks do not test AAX or replace the manual DAW checklist.'
