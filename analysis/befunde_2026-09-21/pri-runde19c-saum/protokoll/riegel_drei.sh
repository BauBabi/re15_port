#!/usr/bin/env bash
# Die drei Riegel, die diese Aenderung beruehrt, namentlich gruen zeigen.
set -euo pipefail
cd "$(dirname "$0")/../../../.."
ctest --test-dir re15_port/build_r19c -R "pri_hashes|pri_silhouette|r19b_marke4" --output-on-failure
