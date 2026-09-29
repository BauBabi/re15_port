#!/usr/bin/env bash
# Datei-Zugriffs-Sonde: /src (Bind-Mount) gegen Container-eigenes Dateisystem (/tmp).
set -u
t() { local s e; s=$(date +%s.%N); "$@" >/dev/null 2>&1; e=$(date +%s.%N); awk -v s="$s" -v e="$e" 'BEGIN{printf "%.3f", e-s}'; }
echo "mount: $(grep ' /src ' /proc/mounts)"
echo "nproc=$(nproc)  mem=$(awk '/MemTotal/{print $2" kB"}' /proc/meminfo)"
N=$(find /src/re15_port/engine /src/re15_port/include -type f | wc -l)
echo "A find+stat engine+include ($N Dateien) /src : $(t find /src/re15_port/engine /src/re15_port/include -type f -printf '%s\n') s"
mkdir -p /tmp/c
echo "B tar-Kopie re15_port (ohne build) /src->/tmp : $(t sh -c 'tar -C /src --exclude=re15_port/build -cf - re15_port | tar -C /tmp/c -xf -') s"
echo "   Groesse: $(du -sh /tmp/c/re15_port | cut -f1)  Dateien: $(find /tmp/c/re15_port -type f | wc -l)"
echo "C find+stat engine+include /tmp : $(t find /tmp/c/re15_port/engine /tmp/c/re15_port/include -type f -printf '%s\n') s"
# kleine Dateien oeffnen+lesen: jede .h einmal
echo "D cat aller .h (/src) : $(t sh -c 'cat /src/re15_port/include/*.h /src/re15_port/engine/src/*.h 2>/dev/null')"
echo "D cat aller .h (/tmp) : $(t sh -c 'cat /tmp/c/re15_port/include/*.h /tmp/c/re15_port/engine/src/*.h 2>/dev/null')"
# 5000x stat derselben Datei
F1=/src/re15_port/CMakeLists.txt; F2=/tmp/c/re15_port/CMakeLists.txt
echo "E 5000x stat /src : $(t sh -c "for i in \$(seq 5000); do stat -c %s $F1; done") s"
echo "E 5000x stat /tmp : $(t sh -c "for i in \$(seq 5000); do stat -c %s $F2; done") s"
# 2000 kleine Dateien schreiben + loeschen (try_compile-Muster)
W1=/src/release/_io_probe_$$; W2=/tmp/_io_probe
mkdir -p "$W1" "$W2"
echo "F 2000x write 1KB /src : $(t sh -c "for i in \$(seq 2000); do head -c 1024 /dev/zero > $W1/f\$i; done") s"
echo "F 2000x write 1KB /tmp : $(t sh -c "for i in \$(seq 2000); do head -c 1024 /dev/zero > $W2/f\$i; done") s"
echo "G rm 2000 /src : $(t rm -rf "$W1") s"
echo "G rm 2000 /tmp : $(t rm -rf "$W2") s"
# grosse Lesevorgaenge (kalt schwer herzustellen; zweimal lesen)
BIG=$(find /src/re15_port/shared_assets/PSX -type f -size +8M | head -1)
[ -n "$BIG" ] || BIG=$(find /src/re15_port/shared_assets/PSX -type f -printf '%s %p\n' | sort -n | tail -1 | cut -d' ' -f2)
echo "H grosse Datei: $BIG ($(stat -c %s "$BIG") B)"
echo "H read /src #1 : $(t cat "$BIG") s   #2 : $(t cat "$BIG") s"
cp "$BIG" /tmp/big
echo "H read /tmp #1 : $(t cat /tmp/big) s"
echo "I Summe shared_assets lesen /src : $(t sh -c 'find /src/re15_port/shared_assets -type f -exec cat {} +') s  ($(du -sh /src/re15_port/shared_assets | cut -f1))"
echo "I Summe shared_assets lesen /tmp : $(t sh -c 'find /tmp/c/re15_port/shared_assets -type f -exec cat {} +') s"
