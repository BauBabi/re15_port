#!/usr/bin/env bash
# Runde 4 (Kette B2): GANZE Kette (build_android.sh --gate-only = apk_pruefen.sh) mit signierten Faelschungen, wie
# pruefer_umgehung_r3_belege/r3_kette_mutanten.sh (dort: MU1/F1 und MU2/F2 je EXIT 0 durch die ganze Kette).
# Faelschungen mit den UNVERAENDERTEN Werkzeugen der Pruefer (r3_faelschen.py, r3_signieren.sh: zipalign -P 16 4 +
# DERSELBE Debug-Schluessel wie der Release-Bau):
#   K0_sig  Referenz neu geschrieben, sonst nichts           F1_sig  Manifestpfad RE15DOOR/P07G.DO2 + Leerzeichen
#   F2_sig  Groesse 55908 von P07G.DO2 in Vollbreit-Ziffern
# Laeufe:  (a) echte Skripte des Arbeitsbaums (--gate-only schreibt nichts)
#          (b) Sandbox (r4_sandbox_anlegen.sh), nur release/apk_asset_gate.py := Mutant MU1..MU8 aus r4_selbsttests.sh
# Rueckgabe je Lauf selbst abgefangen, kein Pipe um den Aufruf.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; B=analysis/befunde_runde34_android/android_gate_r4_belege; B3R=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege
PY=/c/Python310/python; R=build/r34a/ref_v0.8.19.apk; A=$W/apk; L=$W/logs/kette; S=$W/sb_kette
P07=shared_assets/RE15DOOR/P07G.DO2
mkdir -p "$A" "$L"
f() { local z="$1"; shift; "$PY" $B3R/r3_faelschen.py "$R" "$A/$z.apk" "$@" > "$L/faelschen_$z.txt" 2>&1 || { echo "FAELSCHEN FEHLGESCHLAGEN: $z"; exit 1; }
      bash $B3R/r3_signieren.sh "$A/$z.apk" "$A/${z}_sig.apk" | tee -a "$L/signieren.txt"; rm -f "$A/$z.apk"; }
[[ -f "$A/K0_sig.apk" ]] || f K0
[[ -f "$A/F1_leerzeichen_sig.apk" ]] || f F1_leerzeichen --manifest-sub "$P07\n=$P07 \n"
[[ -f "$A/F2_vollbreit_sig.apk" ]] || f F2_vollbreit --manifest-sub "55908\t$P07=\uff15\uff15\uff19\uff10\uff18\t$P07"
[[ -d "$S" ]] || bash $B/r4_sandbox_anlegen.sh "$S" > "$L/sandbox_anlegen.txt" 2>&1 || { echo "Sandbox fehlgeschlagen"; exit 1; }
kette() {   # $1 = Tag, $2 = build_android.sh, $3 = APK
    local rc=0 t0 t1
    t0=$(date +%s)
    bash "$2" --gate-only "$3" --version v0.8.19 > "$L/$1.log" 2>&1 || rc=$?
    t1=$(date +%s)
    printf '%-40s EXIT=%s %4ss | %s\n' "$1" "$rc" "$((t1 - t0))" \
        "$(grep -a -E 'ANDROID-GATES-OK|^ABBRUCH|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE-(OK|ABWEICHUNG)' "$L/$1.log" | tr -d '\r' | cut -c1-100 | tr '\n' ' ')"
}
mutant() { cp "$W/mutanten/$1/apk_asset_gate.py" "$S/release/apk_asset_gate.py"; }
echo "Gate im Arbeitsbaum: sha256 $(sha256sum release/apk_asset_gate.py | cut -c1-16)..."
kette echt_K0_sig                 release/build_android.sh "$A/K0_sig.apk"
kette echt_F1_leerzeichen_sig     release/build_android.sh "$A/F1_leerzeichen_sig.apk"
kette echt_F2_vollbreit_sig       release/build_android.sh "$A/F2_vollbreit_sig.apk"
mutant MU1_manifest_rstrip_alles;   kette MU1_F1_leerzeichen_sig "$S/release/build_android.sh" "$A/F1_leerzeichen_sig.apk"
mutant MU2_groesse_unicode_ziffern; kette MU2_F2_vollbreit_sig   "$S/release/build_android.sh" "$A/F2_vollbreit_sig.apk"
for m in MU1_manifest_rstrip_alles MU2_groesse_unicode_ziffern MU3_splitlines MU4_manifest_max_64GiB MU5_methode_bis_8 \
         MU6_groesse_isdigit MU7_kopf_strip MU8_pfad_strip; do
    mutant "$m"; kette "${m%%_*}_K0_sig" "$S/release/build_android.sh" "$A/K0_sig.apk"
done
cp release/apk_asset_gate.py "$S/release/apk_asset_gate.py"
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" && echo "   Sandbox-Gate zurueck = Arbeitsbaum"
kette sandbox_echt_F1_leerzeichen_sig "$S/release/build_android.sh" "$A/F1_leerzeichen_sig.apk"
kette sandbox_echt_K0_sig             "$S/release/build_android.sh" "$A/K0_sig.apk"
echo "Pruefkopien uebrig: $(ls -d "${TMPDIR:-/tmp}"/re15_apk_pruefen.* 2>/dev/null | wc -l)"
echo FERTIG
