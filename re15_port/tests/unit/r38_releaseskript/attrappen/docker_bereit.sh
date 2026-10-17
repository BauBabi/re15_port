#!/usr/bin/env bash
# Attrappe von release/docker_bereit.sh (Test r38_releaseskript): startet nie Docker Desktop.
set -euo pipefail
if [[ -n "${FAKE_PROTOKOLL:-}" ]]; then echo "docker $*" >> "$FAKE_PROTOKOLL"; fi
if [[ "${FAKE_DOCKER:-}" == nein ]]; then
    echo "Container-Engine antwortet nicht (Attrappe)" >&2
    if [[ "${1:-}" == --pruefen ]]; then exit 3; fi
    exit 1
fi
echo "Container-Engine bereit: Attrappe"
