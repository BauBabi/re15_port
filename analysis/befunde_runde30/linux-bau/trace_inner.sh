#!/usr/bin/env bash
# Spurlauf: der normale Bau, aber unter strace; protokolliert jedes erfolgreiche open,
# dessen Deskriptor auf /host/ zeigt (= Zugriff ueber einen Rueckfall-Link).
set -euo pipefail
apt-get update -qq >/dev/null && apt-get install -y -qq --no-install-recommends strace >/dev/null
strace -V | head -1
exec strace -f -qq -y --seccomp-bpf -e trace=open,openat,openat2 -e signal=none \
    -o "|grep -a '</host/' > /src/release/linux_out/trace_host.txt" \
    bash /src/release/docker_linux_build.sh
