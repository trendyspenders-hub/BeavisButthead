#!/bin/bash
set -euo pipefail
TASK_ROOT="$(cd "$(dirname "$0")" && pwd)"
bash "$TASK_ROOT/scripts/build-mac.command"
