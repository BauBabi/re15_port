#!/usr/bin/env bash
# Gegenpruefung R4-2 (Umgehung): weitere Szenarien fuer den Linux-Pruefstand des ECHTEN android_glue.c (Teil 2).
set -u
Q=/src; T=/tmp/hs2; rm -rf "$T"; mkdir -p "$T"
gcc -std=c11 -D_GNU_SOURCE -O1 -w -I"$Q/harness/stub" -I"$Q" -o "$T/entpacker" "$Q/harness/harness_main.c" "$Q/android_glue.c" \
    "$Q/asset_abgleich.c" || { echo "UEBERSETZEN FEHLGESCHLAGEN"; exit 3; }
liste() {
    ( cd "$1" && find shared_assets synchro -type f 2>/dev/null | LC_ALL=C sort | while IFS= read -r p; do
          printf '%s\t%s\t%s\n' "$(stat -c %s "$p")" "$(sha256sum "$p" | cut -c1-64)" "$p"; done > "$1/.zeilen"
      n=$(grep -c '' "$1/.zeilen"); b=$(awk -F'\t' '{s+=$1} END {print s+0}' "$1/.zeilen")
      { printf '# re15 assets v2 %d %d\n' "$n" "$b"; cat "$1/.zeilen"; } > "$1/re15_assets.txt"; rm -f "$1/.zeilen" )
}
datei() { mkdir -p "$(dirname "$1")"; printf '%s' "$2" > "$1"; }
lauf() {
    local t="$1" r="$2" a="$3" rc=0; shift 3
    env H_ROOT="$r" H_APK="$a" "$@" "$T/entpacker" > "$T/lauf.txt" 2>&1 || rc=$?
    echo "== $t: EXIT=$rc"
    grep -E 'Abgleich|Entpacken fertig|Assets aktuell|ABBRUCH|FEHLER|Waise|entfernt|PRUEFSTAND: SPIELSTART|Summe weicht|entpacke|unlesbar|rename' "$T/lauf.txt" \
        | sed 's/^/     /' | cut -c1-190
}
inhalt() { printf '     Speicher: '; ( cd "$1" && find . -path ./re15_assets_entpackt.txt -prune -o \( -type f -o -type d \) -print | LC_ALL=C sort \
           | grep -v '^\.$' | while read -r p; do if [[ -d "$p" ]]; then printf '%s/ ' "${p#./}"; else printf '%s=%s ' "${p#./}" "$(head -c 12 "$p")"; fi; done; echo ); }

A=$T/apk_A; datei $A/shared_assets/PSX/a.bin AAAA; datei $A/shared_assets/PSX/DATA/b.bin bbbb1; liste $A

# H1 Grenze: Datei auf dem Geraet von aussen gleich gross veraendert -> schneller Weg merkt es nicht (so dokumentiert)
R=$T/s_h1; mkdir -p $R
lauf "H1a A frisch" $R $A
printf 'XXXX' > $R/shared_assets/PSX/a.bin
lauf "H1b Neustart nach Aenderung von aussen (gleiche Groesse)" $R $A; inhalt $R

# H4 halbe/kaputte "zuletzt entpackt"-Liste -> jede Datei pruefen, Waisen weg
R=$T/s_h4; mkdir -p $R
lauf "H4a A frisch" $R $A
datei $R/shared_assets/PSX/alt_waise.bin w
head -c 40 $R/re15_assets_entpackt.txt > $R/x && mv $R/x $R/re15_assets_entpackt.txt
lauf "H4b Neustart mit abgeschnittener Liste (40 B)" $R $A; inhalt $R

# H7 Update: Datei a/b wird zum Ordner a/b/c
H7A=$T/apk_H7A; datei $H7A/shared_assets/PSX/q AAAA; datei $H7A/shared_assets/PSX/k.bin kkkk; liste $H7A
H7B=$T/apk_H7B; datei $H7B/shared_assets/PSX/q/c CCCC; datei $H7B/shared_assets/PSX/k.bin kkkk; liste $H7B
R=$T/s_h7; mkdir -p $R
lauf "H7a Datei PSX/q" $R $H7A
lauf "H7b Update: PSX/q wird Ordner PSX/q/c" $R $H7B; inhalt $R

# H8 Update: Ordner a/b (mit a/b/c) wird zur Datei a/b
R=$T/s_h8; mkdir -p $R
lauf "H8a Ordner PSX/q/c" $R $H7B
lauf "H8b Update: Ordner PSX/q wird Datei PSX/q" $R $H7A; inhalt $R
lauf "H8c Neustart (gleiche APK)" $R $H7A; inhalt $R
lauf "H8d noch ein Neustart" $R $H7A

# H9 Update mit nur in Gross/klein geaendertem Pfad (hier case-SENSITIV)
H9A=$T/apk_H9A; datei $H9A/shared_assets/PSX/X.BIN xxxx; liste $H9A
H9B=$T/apk_H9B; datei $H9B/shared_assets/PSX/x.bin xxxx; liste $H9B
R=$T/s_h9; mkdir -p $R
lauf "H9a X.BIN" $R $H9A
lauf "H9b Update auf x.bin" $R $H9B; inhalt $R
echo "== Szenarien 2 fertig"
