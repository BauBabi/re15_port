#!/usr/bin/env bash
# Vergleicht zwei Linux-Bau-Ergebnisse (alter Weg = Bindmount /src, neuer Weg = Kopie).
# Laeuft IM Container (braucht objdump/ldd/cmp aus dem Bau-Image).
# Aufruf: vergleich.sh <dirA> <dirB>
#   je Verzeichnis: re15_pc, ctest_fingerprint.tsv, ctest_fingerprint.skip
set -uo pipefail
A="$1"; B="$2"
echo "=== Binary ==="
for d in "$A" "$B"; do
    printf '%-40s %10s  %s\n' "$d" "$(stat -c %s "$d/re15_pc")" "$(sha256sum "$d/re15_pc" | cut -c1-16)"
done
n=$(cmp -l "$A/re15_pc" "$B/re15_pc" 2>/dev/null | wc -l)
echo "abweichende Bytes (cmp -l): $n"
if [[ "$n" -gt 0 ]]; then
    # Bereiche zusammenfassen und den Text an der Stelle zeigen
    cmp -l "$A/re15_pc" "$B/re15_pc" | awk '{o=$1-1; if (o!=last+1) { if (NR>1) print s, last; s=o } last=o } END { print s, last }' |
    while read -r s e; do
        printf '  Bereich 0x%x..0x%x (%d B): A="%s" B="%s"\n' "$s" "$e" $((e-s+1)) \
            "$(dd if="$A/re15_pc" bs=1 skip=$((s-12)) count=$((e-s+25)) 2>/dev/null | tr -c '[:print:]' '.')" \
            "$(dd if="$B/re15_pc" bs=1 skip=$((s-12)) count=$((e-s+25)) 2>/dev/null | tr -c '[:print:]' '.')"
    done
fi
echo "=== ldd ==="
diff <(ldd "$A/re15_pc" | sed 's/ (0x[0-9a-f]*)//') <(ldd "$B/re15_pc" | sed 's/ (0x[0-9a-f]*)//') && echo "ldd gleich ($(ldd "$A/re15_pc" | wc -l) Zeilen)"
echo "=== GLIBC-Versionen ==="
ga=$(objdump -T "$A/re15_pc" | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tr '\n' ' ')
gb=$(objdump -T "$B/re15_pc" | grep -oE 'GLIBC_[0-9.]+' | sort -uV | tr '\n' ' ')
echo "A: $ga"; echo "B: $gb"; [[ "$ga" == "$gb" ]] && echo "GLIBC gleich"
echo "=== ctest ==="
for d in "$A" "$B"; do
    echo "$d: $(wc -l < "$d/ctest_fingerprint.tsv") Tests; Status $(cut -f2 "$d/ctest_fingerprint.tsv" | sort | uniq -c | tr -s ' ' | tr '\n' ' '); SKIP/fehlt-Zeilen $(wc -l < "$d/ctest_fingerprint.skip")"
done
echo "--- Name+Status+Zahl der SKIP/fehlt-Zeilen (diff) ---"
diff <(cut -f1-3 "$A/ctest_fingerprint.tsv") <(cut -f1-3 "$B/ctest_fingerprint.tsv") && echo "gleich"
echo "--- SKIP/fehlt-Zeilen woertlich (diff) ---"
diff "$A/ctest_fingerprint.skip" "$B/ctest_fingerprint.skip" && echo "gleich"
echo "=== In welchen ELF-Abschnitten liegen die abweichenden Bytes? ==="
readelf -S -W "$A/re15_pc" | sed -n 's/^ *\[ *[0-9]*\] *//p' | while read -r nm ty ad of sz rest; do [[ "$of" =~ ^[0-9a-f]+$ ]] && echo "$nm $((16#$of)) $((16#$sz))"; done > /tmp/sek.txt
cmp -l "$A/re15_pc" "$B/re15_pc" | awk 'NR==FNR { n[NR]=$1; o[NR]=$2; s[NR]=$3; k=NR; next }
    { off=$1-1; hit="(kein Abschnitt)"; for (i=1;i<=k;i++) if (off>=o[i] && off<o[i]+s[i]) hit=n[i]; c[hit]++ }
    END { for (h in c) print "  " h ": " c[h] " Bytes" }' /tmp/sek.txt -
echo "Build-ID A: $(readelf -n "$A/re15_pc" | grep -o 'Build ID: .*')"
echo "Build-ID B: $(readelf -n "$B/re15_pc" | grep -o 'Build ID: .*')"
