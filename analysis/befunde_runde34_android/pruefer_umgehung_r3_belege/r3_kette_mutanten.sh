#!/usr/bin/env bash
# Gegenpruefung R3: GANZE Kette (build_android.sh --gate-only = apk_pruefen.sh: Kopie, Stichproben, aapt,
# zipalign, apksigner + Signer-Pin, Selbsttest, Gate, Kennungsvergleich) mit signierten Faelschungen:
#   (a) echte Skripte im Arbeitsbaum (nur lesend, --gate-only schreibt nichts)
#   (b) Sandbox build/r34a/pruefer_r3/sb_kette: UNVERAENDERTE Skripte, nur release/apk_asset_gate.py := Mutant
# Rueckgabe je Lauf selbst abgefangen, kein Pipe um den Aufruf.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; B=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege; L=$W/logs/kette; mkdir -p "$L"
S=$W/sb_kette
[[ -d "$S" ]] || bash $B/r3_sandbox_anlegen.sh "$S" > "$L/sandbox_anlegen.txt" 2>&1 || { echo "Sandbox fehlgeschlagen"; exit 1; }
kette() {   # $1 = Tag, $2 = Skript build_android.sh, $3 = APK
    local rc=0 t0 t1
    t0=$(date +%s)
    bash "$2" --gate-only "$3" --version v0.8.19 > "$L/$1.log" 2>&1 || rc=$?
    t1=$(date +%s)
    printf '%-44s EXIT=%s %4ss | %s\n' "$1" "$rc" "$((t1 - t0))" \
        "$(grep -a -E 'ANDROID-GATES-OK|^ABBRUCH|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE-(OK|ABWEICHUNG)' "$L/$1.log" | tr -d '\r' | cut -c1-110 | tr '\n' ' ')"
}
mutant() {  # $1 = Mutanten-Name -> Sandbox-Gate tauschen
    cp "$W/mutanten/$1/apk_asset_gate.py" "$S/release/apk_asset_gate.py"
    echo "   Sandbox-Gate := $1 (sha256 $(sha256sum "$S/release/apk_asset_gate.py" | cut -c1-16)...)"
}
echo "Gate im Arbeitsbaum: sha256 $(sha256sum release/apk_asset_gate.py | cut -c1-16)..."
kette echt_K0_sig                    release/build_android.sh "$W/apk/K0_sig.apk"
kette echt_F1_leerzeichen_sig        release/build_android.sh "$W/apk/F1_leerzeichen_sig.apk"
kette echt_F2_vollbreit_sig          release/build_android.sh "$W/apk/F2_vollbreit_sig.apk"
mutant MU1_manifest_rstrip_alles
kette MU1_K0_sig                     "$S/release/build_android.sh" "$W/apk/K0_sig.apk"
kette MU1_F1_leerzeichen_sig         "$S/release/build_android.sh" "$W/apk/F1_leerzeichen_sig.apk"
mutant MU2_groesse_unicode_ziffern
kette MU2_F2_vollbreit_sig           "$S/release/build_android.sh" "$W/apk/F2_vollbreit_sig.apk"
cp release/apk_asset_gate.py "$S/release/apk_asset_gate.py"
echo "   Sandbox-Gate zurueck auf das echte (sha256 $(sha256sum "$S/release/apk_asset_gate.py" | cut -c1-16)...)"
kette sandbox_echt_F1_leerzeichen_sig "$S/release/build_android.sh" "$W/apk/F1_leerzeichen_sig.apk"
echo FERTIG
