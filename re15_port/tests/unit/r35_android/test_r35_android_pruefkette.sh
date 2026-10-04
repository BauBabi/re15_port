#!/usr/bin/env bash
# =============================================================================================
# Runde 35 Spur N "android" - Punkt 3: "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst
# sorgfaeltig mittesten." (Gegenpruefung R4-2: F-Y1 Selbsttest-Luecken, F-Y2 Urteil ungeprueft.)
# Dossier: analysis/befunde_runde35/N_android.md Punkt 3. ctest unit_r35_android_pruefkette (probes/r35_android.cmake).
#
# Faehrt die ECHTE Kette aus release/apk_pruefen.sh (gate_festhalten, gate_laufen, gate_urteil) mit dem ECHTEN,
# gepinnten Gate und Urteil - ohne Android-SDK (aapt/apksigner braucht nur apk_pruefen, nicht diese Bausteine):
#   P  Positiv:   gate_festhalten (beide Pins + Urteils-Selbsttest), Gate-Selbsttest und Quellbaum ueber gate_laufen -> 0
#   N  Negativ-Kontrollen, die ROT werden MUESSEN (Urteil != 0 bzw. Abbruch):
#      N1 0-Byte-"APK", N2 leeres ZIP, N3 ZIP nur mit Manifest, N4 Zufallsbytes        (echtes Gate)
#      N5 kaputtes Gate (main() ohne sys.exit) auf eine eigene Pin-Datei umgepinnt     -> Selbsttest/APK nicht 0
#      N6 Urteil mit dem 1-Zeichen-Fehler aus F-Y2 (ende(1, "das Gate meldet" -> ende(0, ...), umgepinnt -> Abbruch
#         im Urteils-Selbsttest
#      N7 Urteil 0 Byte, umgepinnt -> Abbruch (keine Schlusszeile)
#      N8 Urteil voellig kompromittiert (Selbsttest LUEGT OK, Urteil sagt immer 0 mit Urteilszeile), umgepinnt ->
#         die zweite Instanz in gate_laufen (Rueckgabe des Gates != 0) haelt die leere APK trotzdem ROT
#      N9 Urteils-Selbsttest mit weniger Mutanten als festgehalten (GATE_URTEIL_MIN_ERKANNT) -> Abbruch
#   Am Ende: release/apk_asset_gate.py, gate_urteil.py und beide Pins unveraendert (cmp gegen den Start).
# Aufruf: test_r35_android_pruefkette.sh <repo-wurzel> <arbeitsordner>
# =============================================================================================
set -u
REPO="$1"
W="$2"
rm -rf "$W" && mkdir -p "$W" || exit 9
cd "$REPO" || exit 9
# shellcheck source=/dev/null
source release/python_finden.sh > "$W/python.txt" 2>&1 || { cat "$W/python.txt"; echo "kein Python >= 3.8"; exit 9; }
echo "Python: $PY ($PY_VERSION)"
die() { echo "DIE: $*" >&2; exit 1; }
# shellcheck source=/dev/null
source release/apk_pruefen.sh
for f in release/apk_asset_gate.py release/gate_urteil.py release/apk_asset_gate.sha256 release/gate_urteil.sha256; do
    cp "$f" "$W/start_$(basename "$f")"
done
fehler=0
gut()    { echo "ok      $*"; }
falsch() { echo "FALSCH  $*"; fehler=$((fehler + 1)); }
sha() { "$PY" -c "import hashlib,sys; print(hashlib.sha256(open(sys.argv[1],'rb').read()).hexdigest())" "$(apk_nativ "$1")"; }

# ---------------------------------------------------------------------------------------------- P: Positiv
mkdir -p "$W/fest"
if ( gate_festhalten "$W/fest" ) > "$W/P0.txt" 2>&1; then
    gut "P0 gate_festhalten: $(grep -m1 'Gate-Urteil selbstgeprueft' "$W/P0.txt" | sed 's/^ *//')"
else
    falsch "P0 gate_festhalten bricht ab"; cat "$W/P0.txt"
fi
rm -rf "$W/fest"; mkdir -p "$W/fest"
gate_festhalten "$W/fest" > /dev/null 2>&1 || { echo "gate_festhalten im Hauptlauf scheitert"; exit 1; }

lauf() {   # $1 = Titel, $2 = soll ("0" oder "rot"), Rest = gate_laufen-Argumente; Ausgabe nach $W/<titel>.txt
    local t="$1" soll="$2" u=0
    shift 2
    ( gate_laufen "$@" ) > "$W/$t.txt" 2>&1 || u=$?
    local z
    z="$(grep -a -E 'Gate-Urteil|ueberstimmt|ohne Urteilszeile|DIE:' "$W/$t.txt" | tail -1 | sed 's/^ *//' | cut -c1-150)"
    if [[ "$soll" == 0 && "$u" == 0 ]] || [[ "$soll" == rot && "$u" != 0 ]]; then
        gut "$t -> $u   $z"
    else
        falsch "$t -> $u (soll $soll)   $z"; tail -5 "$W/$t.txt"
    fi
}
lauf P1_selbsttest 0 selbsttest "$GATE_KOPIE" --selbsttest
lauf P2_quellbaum 0 quellbaum "$GATE_KOPIE" --repo "$(apk_nativ "$REPO")" --quellbaum

# ---------------------------------------------------------------------------------------------- N1-N4: kaputte APKs
"$PY" - "$(apk_nativ "$W")" <<'PY'
import os, sys, zipfile
w = sys.argv[1]
open(os.path.join(w, "N1_null.apk"), "wb").close()
zipfile.ZipFile(os.path.join(w, "N2_leer.apk"), "w").close()
with zipfile.ZipFile(os.path.join(w, "N3_nur_manifest.apk"), "w") as z:
    z.writestr("assets/re15_assets.txt", "# re15 assets v2 1 4\n4\t" + "0" * 64 + "\tshared_assets/PSX/a.bin\n")
with open(os.path.join(w, "N4_zufall.apk"), "wb") as f:
    f.write(bytes((i * 131 + 7) % 256 for i in range(65536)))
PY
for a in N1_null N2_leer N3_nur_manifest N4_zufall; do
    lauf "$a" rot apk "$GATE_KOPIE" --repo "$(apk_nativ "$REPO")" "$(apk_nativ "$W/$a.apk")"
done

# ---------------------------------------------------------------------------------------------- N5: kaputtes Gate
sed 's/^    sys\.exit(main())$/    main()/' release/apk_asset_gate.py > "$W/G1.py"
if cmp -s release/apk_asset_gate.py "$W/G1.py"; then falsch "N5 G1 nicht erzeugt (Muster fehlt)"; fi
(
    fehler=0
    GATE_PIN_DATEI="$W/pin_G1"; sha "$W/G1.py" > "$GATE_PIN_DATEI"
    lauf N5a_G1_selbsttest rot selbsttest "$W/G1.py" --selbsttest
    lauf N5b_G1_apk rot apk "$W/G1.py" --repo "$(apk_nativ "$REPO")" "$(apk_nativ "$W/N3_nur_manifest.apk")"
    exit "$fehler"
) || fehler=$((fehler + $?))

# ---------------------------------------------------------------------------------------------- N6-N9: Urteil
urteil_umgepinnt() {   # $1 = Titel, $2 = Urteils-Datei, $3 = Teilstring der erwarteten Abbruchmeldung
    local t="$1" d="$2" m="$3" u=0
    (
        GATE_URTEIL_PIN_DATEI="$W/pin_$t"; sha "$d" > "$GATE_URTEIL_PIN_DATEI"
        GATE_URTEIL_GEPRUEFT=""
        gate_urteil_selbsttest "$d"
    ) > "$W/$t.txt" 2>&1 || u=$?
    if [[ "$u" != 0 ]] && grep -aq "$m" "$W/$t.txt"; then
        gut "$t -> Abbruch: $(grep -a -m1 'DIE:' "$W/$t.txt" | cut -c1-140)"
    else
        falsch "$t -> $u, Meldung '$m' fehlt"; tail -5 "$W/$t.txt"
    fi
}
"$PY" - release/gate_urteil.py "$(apk_nativ "$W/U_fy2.py")" <<'PY'
import sys
s = open(sys.argv[1], encoding="utf-8", newline="").read()
alt = 'ende(1, "das Gate meldet %s'
assert s.count(alt) == 1, s.count(alt)
open(sys.argv[2], "w", encoding="utf-8", newline="").write(s.replace(alt, 'ende(0, "das Gate meldet %s'))
PY
urteil_umgepinnt N6_urteil_fy2_ein_zeichen "$W/U_fy2.py" "Selbsttest des Gate-Urteils ohne gueltige Schlusszeile"
: > "$W/U_leer.py"
urteil_umgepinnt N7_urteil_leer "$W/U_leer.py" "Selbsttest des Gate-Urteils ohne gueltige Schlusszeile"
cat > "$W/U_luegt.py" <<'PY'
import sys
if sys.argv[1:] == ["--selbsttest"]:
    print("== URTEIL-SELBSTTEST-OK: 119/119 Faelle, 257/260 Mutanten erkannt, 3 als gleichwertig begruendet ==")
    sys.exit(0)
print("   Gate-Urteil (%s, Rueckgabe 0): alles bestens" % sys.argv[1])
sys.exit(0)
PY
(
    fehler=0
    GATE_URTEIL_PIN_DATEI="$W/pin_luegt"; sha "$W/U_luegt.py" > "$GATE_URTEIL_PIN_DATEI"
    GATE_URTEIL_KOPIE="$W/U_luegt.py"; GATE_URTEIL_GEPRUEFT=""
    lauf N8_urteil_luegt_leere_apk rot apk "$GATE_KOPIE" --repo "$(apk_nativ "$REPO")" "$(apk_nativ "$W/N3_nur_manifest.apk")"
    grep -aq "ueberstimmt (zweite Instanz)" "$W/N8_urteil_luegt_leere_apk.txt" \
        && gut "N8 die zweite Instanz hat ueberstimmt" || { falsch "N8 ohne 'ueberstimmt (zweite Instanz)'"; }
    exit "$fehler"
) || fehler=$((fehler + $?))
sed 's/^GATE_URTEIL_MIN_ERKANNT=.*/GATE_URTEIL_MIN_ERKANNT=999/' release/apk_pruefen.sh > "$W/apk_pruefen_min999.sh"
(
    source "$W/apk_pruefen_min999.sh"
    GATE_URTEIL_PIN_DATEI="$REPO/release/gate_urteil.sha256"
    GATE_URTEIL_GEPRUEFT=""
    gate_urteil_selbsttest "$REPO/release/gate_urteil.py"
) > "$W/N9.txt" 2>&1 && { falsch "N9 Mindestzahl 999 Mutanten angenommen"; } \
    || { grep -aq "Mutanten erkannt" "$W/N9.txt" && gut "N9 Mindestzahl Mutanten: $(grep -a -m1 'DIE:' "$W/N9.txt" | cut -c1-120)" \
         || falsch "N9 falsche Meldung: $(tail -2 "$W/N9.txt")"; }

# ---------------------------------------------------------------------------------------------- unveraendert?
for f in release/apk_asset_gate.py release/gate_urteil.py release/apk_asset_gate.sha256 release/gate_urteil.sha256; do
    cmp -s "$f" "$W/start_$(basename "$f")" || falsch "$f waehrend des Tests veraendert"
done
echo "FEHLER=$fehler"
[[ "$fehler" == 0 ]]
