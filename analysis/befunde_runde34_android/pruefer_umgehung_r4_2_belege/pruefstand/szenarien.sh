#!/usr/bin/env bash
# Gegenpruefung R4-2 (Umgehung): Szenarien fuer den Linux-Pruefstand des ECHTEN android_glue.c (im Container unter /tmp).
# Aufruf im Container: bash /src/harness/szenarien.sh   (Quellen /src read-only; Ergebnis nach stdout)
set -u
Q=/src; T=/tmp/hs; rm -rf "$T"; mkdir -p "$T"
gcc -std=c11 -D_GNU_SOURCE -O1 -Wall -Wextra -Wno-unused-parameter -I"$Q/harness/stub" -I"$Q" \
    -o "$T/entpacker" "$Q/harness/harness_main.c" "$Q/android_glue.c" "$Q/asset_abgleich.c" 2> "$T/cc.txt" \
    || { cat "$T/cc.txt"; echo "UEBERSETZEN FEHLGESCHLAGEN"; exit 3; }
grep -c 'warning' "$T/cc.txt" | sed 's/^/Warnungen beim Uebersetzen: /'
echo "android_glue.c sha256 $(sha256sum "$Q/android_glue.c" | cut -c1-16)..., asset_abgleich.c $(sha256sum "$Q/asset_abgleich.c" | cut -c1-16)..."

liste() {   # $1 = APK-Ordner: re15_assets.txt v2 aus shared_assets/ + synchro/ schreiben
    ( cd "$1" && find shared_assets synchro -type f 2>/dev/null | LC_ALL=C sort | while IFS= read -r p; do
          printf '%s\t%s\t%s\n' "$(stat -c %s "$p")" "$(sha256sum "$p" | cut -c1-64)" "$p"; done > "$1/.zeilen"
      n=$(grep -c '' "$1/.zeilen"); b=$(awk -F'\t' '{s+=$1} END {print s+0}' "$1/.zeilen")
      { printf '# re15 assets v2 %d %d\n' "$n" "$b"; cat "$1/.zeilen"; } > "$1/re15_assets.txt"; rm -f "$1/.zeilen" )
}
datei() { mkdir -p "$(dirname "$1")"; printf '%s' "$2" > "$1"; }
lauf() {    # $1 = Titel, $2 = Speicher, $3 = APK, Rest = Umgebung
    local t="$1" r="$2" a="$3" rc=0; shift 3
    env H_ROOT="$r" H_APK="$a" "$@" "$T/entpacker" > "$T/lauf.txt" 2>&1 || rc=$?
    echo "== $t: EXIT=$rc"
    grep -E 'Abgleich|Entpacken fertig|Assets aktuell|ABBRUCH|FEHLER|Waise|entfernt|PRUEFSTAND|Summe weicht|entpacke|nicht anlegbar|rename' "$T/lauf.txt" \
        | sed 's/^/     /' | cut -c1-200
    return 0
}
inhalt() { printf '     Speicher: '; ( cd "$1" && find . -path ./re15_assets_entpackt.txt -prune -o \( -type f -o -type d \) -print | LC_ALL=C sort \
           | grep -v '^\.$' | while read -r p; do if [[ -d "$p" ]]; then printf '%s/ ' "${p#./}"; else printf '%s=%s ' "${p#./}" "$(head -c 12 "$p")"; fi; done; echo );
           printf '     Liste zuletzt entpackt: %s\n' "$( [[ -f "$1/re15_assets_entpackt.txt" ]] && head -1 "$1/re15_assets_entpackt.txt" || echo '(keine)')"; }

# --- A/B: Grundlauf, Neustart, gleich grosse Aenderung (N1a)
A=$T/apk_A; datei $A/shared_assets/PSX/a.bin AAAA; datei $A/shared_assets/PSX/DATA/b.bin bbbb1; datei $A/synchro/STAGE1/m.wav mmmm; liste $A
B=$T/apk_B; cp -r $A $B; datei $B/shared_assets/PSX/DATA/b.bin bbbb2; liste $B
R=$T/s_ab; mkdir -p $R
lauf "S0 A frisch" $R $A; inhalt $R
lauf "S1 A Neustart" $R $A
lauf "S2 Update A->B (b.bin gleich gross geaendert)" $R $B; inhalt $R

# --- Y4: Datei X und Ordner X.neu/ (Liste gueltig), danach X geaendert
Y1=$T/apk_Y1; datei $Y1/shared_assets/PSX/X xxxx1; datei $Y1/shared_assets/PSX/X.neu/B BBBB; datei $Y1/shared_assets/PSX/a.bin AAAA; liste $Y1
Y2=$T/apk_Y2; cp -r $Y1 $Y2; datei $Y2/shared_assets/PSX/X xxxx2; liste $Y2
R=$T/s_y4; mkdir -p $R
echo "--- Y4-Liste: $(tail -n +2 $Y1/re15_assets.txt | cut -f3 | tr '\n' ' ')"
lauf "S3a Y1 frisch (X + X.neu/B)" $R $Y1; inhalt $R
lauf "S3b Update Y1->Y2 (X geaendert, gleich gross)" $R $Y2; inhalt $R
lauf "S3c Neustart mit Y2" $R $Y2; inhalt $R
lauf "S3d noch ein Neustart mit Y2" $R $Y2; inhalt $R

# --- verschluckt: weg-Pfad, dessen unlink scheitert (Ordner nur lesbar, als uid != 0)
W1=$T/apk_W1; datei $W1/shared_assets/PSX/a.bin AAAA; datei $W1/shared_assets/ALT/w.bin wwww; liste $W1
W2=$T/apk_W2; mkdir -p $W2/shared_assets/PSX; datei $W2/shared_assets/PSX/a.bin AAA2; liste $W2
R=$T/s_weg; mkdir -p $R
lauf "S4a W1 frisch" $R $W1
chmod 0555 $R/shared_assets/ALT
lauf "S4b Update W1->W2 (w.bin gestrichen, ALT/ nur lesbar; uid $(id -u))" $R $W2; inhalt $R
lauf "S4c Neustart W2" $R $W2
chmod 0755 $R/shared_assets/ALT

# --- JNI/Anzeige-Fehlerpfade
R=$T/s_jni; mkdir -p $R
lauf "S5 kein AssetManager" $R $A H_KEIN_AM=1
lauf "S6 kein AssetManager UND kein Renderer" $R $A H_KEIN_AM=1 H_KEIN_RENDERER=1

# --- Abbruch mitten im Entpacken, Neustart
C=$T/apk_C; mkdir -p $C/shared_assets/PSX; head -c 300000 /dev/urandom > $C/shared_assets/PSX/gross.bin
datei $C/shared_assets/PSX/a.bin AAAA; datei $C/synchro/STAGE1/m.wav mmmm; liste $C
R=$T/s_abbruch; mkdir -p $R
lauf "S7a C frisch, Abbruch nach 150000 B (mitten in gross.bin)" $R $C H_ABBRUCH_NACH_BYTES=150000
printf '     .neu im Speicher: %s\n' "$(cd $R && find . -name '*.neu' | tr '\n' ' ')"; inhalt $R
lauf "S7b Neustart C" $R $C
printf '     .neu im Speicher: %s; gross.bin sha gleich APK: %s\n' "$(cd $R && find . -name '*.neu' | wc -l)" \
    "$(cmp -s $R/shared_assets/PSX/gross.bin $C/shared_assets/PSX/gross.bin && echo ja || echo NEIN)"

# --- Liste v1, Datei fehlt in der APK, falsche Summe in der Liste
V=$T/apk_V; mkdir -p $V; cp -r $A/shared_assets $V/; printf '# re15 assets 1 4\n4\tshared_assets/PSX/a.bin\n' > $V/re15_assets.txt
R=$T/s_v1; mkdir -p $R; lauf "S9 Liste v1" $R $V
X=$T/apk_X; cp -r $A $X; rm -f $X/shared_assets/PSX/DATA/b.bin
R=$T/s_fehlt; mkdir -p $R; lauf "S10 Liste nennt Datei, die der APK fehlt" $R $X
Z=$T/apk_Z; cp -r $A $Z; sed -i "s/\t[0-9a-f]\{64\}\tshared_assets\/PSX\/a.bin/\t$(printf 'f%.0s' $(seq 64))\tshared_assets\/PSX\/a.bin/" $Z/re15_assets.txt
R=$T/s_summe; mkdir -p $R; lauf "S11a Liste: falsche Summe fuer a.bin, frisch" $R $Z; lauf "S11b Neustart" $R $Z
echo "== Szenarien fertig"
