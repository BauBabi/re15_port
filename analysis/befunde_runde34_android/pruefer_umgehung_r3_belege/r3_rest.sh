#!/usr/bin/env bash
# Gegenpruefung R3: Randfaelle der Kette und des Quellbaums, Regression mit dem Werkzeug der R2.
#   A  build_android.sh --gate-only (echte Skripte, nur lesend) mit falschen/ungewoehnlichen Pfaden und Versionen
#   B  Quellbaum-Varianten in der Sandbox sb_kette (Hardlinks; nur Umbenennen/Neuanlegen IN der Sandbox) gegen die
#      Referenz-APK, echtes Gate mit --repo <sandbox>
#   C  Regression: Faelschungsarten der R2 (r2_faelschen.py, unabhaengiger Parser) gegen das Gate v6
# Rueckgabe je Lauf selbst abgefangen, kein Pipe um den Aufruf.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; B=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege; L=$W/logs/rest; mkdir -p "$L"
PY=/c/Python310/python; R=build/r34a/ref_v0.8.19.apk; S=$W/sb_kette; F=$W/apk
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" || { echo "Sandbox-Gate ist nicht das echte"; exit 1; }
zeile() { printf '%-50s soll %s ist %s %-10s | %s\n' "$1" "$2" "$3" "$([[ "$3" == "$2" ]] && echo ok || echo ABWEICHUNG)" "$4"; }
go() {      # $1 = Tag, $2 = soll, Rest = Argumente fuer build_android.sh
    local tag="$1" soll="$2" rc=0; shift 2
    bash release/build_android.sh "$@" > "$L/A_$tag.log" 2>&1 || rc=$?
    zeile "A_$tag" "$soll" "$rc" "$(grep -a -E 'ABBRUCH|ANDROID-GATES-OK|braucht|unbekannte|unbound' "$L/A_$tag.log" | tr -d '\r' | head -2 | cut -c1-150 | tr '\n' ' ')"
}
gate() {    # $1 = Tag, $2 = soll, $3 = repo, $4 = apk
    local rc=0
    "$PY" release/apk_asset_gate.py --repo "$3" "$4" > "$L/$1.log" 2>&1 || rc=$?
    zeile "$1" "$2" "$rc" "$(grep -a -E '^      |ABBRUCH|GATE-OK' "$L/$1.log" | tr -d '\r' | head -2 | sed 's/^ *//' | cut -c1-150 | tr '\n' ' ')"
}
echo "=== A: --gate-only ==="
mkdir -p "$F/mit leer"; cp -f "$F/K0_sig.apk" "$F/mit leer/K0 sig.apk"
head -c 100000000 "$F/K0_sig.apk" > "$F/K0_sig_abgeschnitten.apk"; : > "$F/null.apk"; cp release/apk_signer.sha256 "$F/text.apk"
go 01_pfad_fehlt           1 --gate-only "$F/gibt_es_nicht.apk" --version v0.8.19
go 02_pfad_ordner          1 --gate-only "$F" --version v0.8.19
go 03_pfad_leer            2 --gate-only "" --version v0.8.19
go 04_textdatei            1 --gate-only "$F/text.apk" --version v0.8.19
go 05_null_byte            1 --gate-only "$F/null.apk" --version v0.8.19
go 06_abgeschnitten        1 --gate-only "$F/K0_sig_abgeschnitten.apk" --version v0.8.19
go 07_leerzeichen_im_pfad  0 --gate-only "$F/mit leer/K0 sig.apk" --version v0.8.19
go 08_version_leerzeichen  1 --gate-only "$F/K0_sig.apk" --version "v0.8.19 "
go 09_version_praefix      1 --gate-only "$F/K0_sig.apk" --version v0.8.1
go 10_version_vor_pfad     0 --version v0.8.19 --gate-only "$F/K0_sig.apk"
go 11_ohne_version         2 --gate-only "$F/K0_sig.apk"
go 12_version_ohne_wert    1 --gate-only "$F/K0_sig.apk" --version
echo "=== B: Quellbaum-Varianten (Sandbox $S) gegen die Referenz-APK ==="
gate B00_sandbox_unveraendert 0 "$S" "$R"
P="$S/parkplatz"; mkdir -p "$P"
mkdir -p "$P/RE15DOOR"; mv "$S"/re15_port/shared_assets/RE15DOOR/* "$P/RE15DOOR/"
gate B01_RE15DOOR_leer 1 "$S" "$R"
rmdir "$S/re15_port/shared_assets/RE15DOOR"
gate B02_RE15DOOR_fehlt 1 "$S" "$R"
mkdir "$S/re15_port/shared_assets/RE15DOOR"; mv "$P"/RE15DOOR/* "$S/re15_port/shared_assets/RE15DOOR/"
mv "$S/synchro/STAGE2" "$S/synchro/unused2"
gate B03_synchro_STAGE2_nach_unused2 1 "$S" "$R"
mv "$S/synchro/unused2" "$S/synchro/STAGE2"
printf 'liesmich\n' > "$S/synchro/README.md"
gate B04_synchro_README_ausserhalb 0 "$S" "$R"
rm -f "$S/synchro/README.md"
printf 'x\n' > "$S/synchro/STAGEX.txt"
gate B05_synchro_STAGEX_datei_oben 1 "$S" "$R"
rm -f "$S/synchro/STAGEX.txt"
cp "$S/re15_port/platform/android/app/build.gradle" "$P/build.gradle"
"$PY" - "$S/re15_port/platform/android/app/build.gradle" <<'PY'
import sys
p = sys.argv[1]; t = open(p, encoding="utf-8").read()
alt = 'from(new File(portRoot, "shared_assets/RE15DOOR"))'
assert t.count(alt) == 1, t.count(alt)
t = t.replace(alt, 'from(new File(portRoot, "shared_assets/NEU"))         { into "shared_assets/NEU" }\n        ' + alt)
open(p, "w", encoding="utf-8", newline="").write(t)
PY
gate B06_build_gradle_zusatzbaum 2 "$S" "$R"
cp -f "$P/build.gradle" "$S/re15_port/platform/android/app/build.gradle"
mkdir -p "$S/re15_port/shared_assets/RE15NEU"; printf 'neu\n' > "$S/re15_port/shared_assets/RE15NEU/NEU.DAT"
gate B07_neuer_baum_unter_shared_assets 1 "$S" "$R"
rm -rf "$S/re15_port/shared_assets/RE15NEU"
printf 'v\n' > "$S/re15_port/shared_assets/PSX/.versteckt"
gate B08_punktdatei_in_PSX 1 "$S" "$R"
rm -f "$S/re15_port/shared_assets/PSX/.versteckt"
: > "$S/re15_port/shared_assets/RE2/LEER.BIN"
gate B09_null_byte_datei_in_RE2 1 "$S" "$R"
rm -f "$S/re15_port/shared_assets/RE2/LEER.BIN"
gate B10_sandbox_wieder_unveraendert 0 "$S" "$R"
echo "=== C: Regression mit r2_faelschen.py (R2-Werkzeug) ==="
R2F=analysis/befunde_runde34_android/pruefer_umgehung_r2_belege/r2_faelschen.py
P07=assets/shared_assets/RE15DOOR/P07G.DO2
[[ -f $W/null55908.bin ]] || head -c 55908 /dev/zero > $W/null55908.bin
"$PY" $R2F $R $F/C1.apk --doppelt $P07=$W/null55908.bin > /dev/null && gate C1_doppelt_zweiter_falsch 1 . $F/C1.apk; rm -f $F/C1.apk
"$PY" $R2F $R $F/C2.apk --verzeichnis assets/shared_assets/ > /dev/null && gate C2_verzeichnis 1 . $F/C2.apk; rm -f $F/C2.apk
"$PY" $R2F $R $F/C3.apk --cd-backslash $P07 > /dev/null && gate C3_backslash_nur_CD 1 . $F/C3.apk; rm -f $F/C3.apk
"$PY" $R2F $R $F/C4.apk --umbenennen $P07=assets/shared_assets/re15door/P07G.DO2 > /dev/null && gate C4_ordner_kleingeschrieben 1 . $F/C4.apk; rm -f $F/C4.apk
"$PY" $R2F $R $F/C5.apk --manifest-crlf > /dev/null && gate C5_manifest_crlf 0 . $F/C5.apk; rm -f $F/C5.apk
"$PY" $R2F $R $F/C6.apk --manifest-zeile-plus "5\\tshared_assets/RE15DOOR/GEIST.DO2" > /dev/null && gate C6_geisterzeile 1 . $F/C6.apk; rm -f $F/C6.apk
rm -f "$F/K0_sig_abgeschnitten.apk" "$F/null.apk" "$F/text.apk"; rm -rf "$F/mit leer"
echo FERTIG
