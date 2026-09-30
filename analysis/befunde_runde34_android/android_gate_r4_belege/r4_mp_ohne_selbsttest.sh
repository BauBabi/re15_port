#!/usr/bin/env bash
# Runde 4 (Kette B4): Negativ-Kontrolle wie pruefer_umgehung_r3_belege/r3_mp_ohne_selbsttest.sh - make_package.sh
# ECHT in einer eigenen Sandbox (r4_sandbox_anlegen.sh), --version v0.8.19 --only linux, KEINE APK:
#   L0  --no-zip, echtes Gate -> EXIT 0, legt pkg-linux an
#   --  1 Byte in der PAKET-Kopie von shared_assets/PSX/DATA/TEX.TIM (Linkzahl 1 geprueft)
#   LA  --zip-only, echtes Gate                           -> soll EXIT 1 (--paket: Inhalt weicht ab)
#   LB  --zip-only, Sandbox-Gate := MUP (paket_pruefen: 'if q_sha != p_sha:' -> 'if False:')
#                                                          -> soll EXIT 1 im SELBSTTEST, vor --quellbaum
#   LC  --zip-only, MUP + ALTES make_package.sh/apk_pruefen.sh (9d2337e4) -> Kontrolle: EXIT 0 (der Befund)
# Danach Sandbox-Gate/-Skripte zurueck (cmp), Paketdatei zurueck. Rueckgabe je Lauf selbst abgefangen.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/r4; B=analysis/befunde_runde34_android/android_gate_r4_belege; S=$W/sb_b4; L=$W/logs/b4; mkdir -p "$L"
PY=/c/Python310/python; MUP=$W/mutanten/MUP_paket_sha_aus/apk_asset_gate.py
[[ -f "$MUP" ]] || { echo "MUP fehlt (r4_selbsttests.sh zuerst)"; exit 1; }
rm -rf "$S"; bash $B/r4_sandbox_anlegen.sh "$S" || { echo "Sandbox fehlgeschlagen"; exit 1; }
mkdir -p "$S/release/linux_out"; cp -p $W/bin/linux/re15_pc "$S/release/linux_out/re15_pc"
mp() {      # $1 = Tag, Rest = Zusatzargumente
    local tag="$1" rc=0 t0 t1; shift
    t0=$(date +%s)
    bash "$S/release/make_package.sh" --version v0.8.19 --only linux "$@" > "$L/$tag.log" 2>&1 || rc=$?
    t1=$(date +%s)
    echo "== $tag ($*): EXIT=$rc ($((t1 - t0)) s)"
    grep -a -E '^ABBRUCH|Selbsttest|SELBSTTEST-(OK|FEHLER)|\[FEHLER\] (132|185|186)|QUELLBAUM-OK|PAKET-(OK|ABWEICHUNG)|Inhalt weicht ab|Assets kopieren|kein Android|== Fertig' \
        "$L/$tag.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-190
}
mp L0_no_zip --no-zip
T="$S/release/pkg-linux/re15_port_v0.8.19/shared_assets/PSX/DATA/TEX.TIM"
[[ -f "$T" ]] || { echo "Paketordner fehlt: $T"; exit 1; }
echo "Linkzahl Paket-TEX.TIM: $(stat -c %h "$T") (1 = eigene Kopie)"; [[ "$(stat -c %h "$T")" == 1 ]] || exit 1
cp -p "$T" "$L/TEX.TIM.orig"
"$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x01; open(p,'wb').write(d)" "$T"
echo "Paket-TEX.TIM jetzt $(sha256sum "$T" | cut -c1-16)..., Quelle $(sha256sum re15_port/shared_assets/PSX/DATA/TEX.TIM | cut -c1-16)..."
mp LA_echtes_gate --zip-only
cp "$MUP" "$S/release/apk_asset_gate.py"; echo "--- Sandbox-Gate := MUP ($(sha256sum "$S/release/apk_asset_gate.py" | cut -c1-16)...)"
diff release/apk_asset_gate.py "$S/release/apk_asset_gate.py" | sed 's/^/   /'
mp LB_MUP_neues_skript --zip-only
cp -p $W/alt/make_package.sh "$S/release/make_package.sh"; cp -p $W/alt/apk_pruefen.sh "$S/release/apk_pruefen.sh"
echo "--- Sandbox-Skripte := ALT (9d2337e4)"
mp LC_MUP_altes_skript --zip-only
for f in apk_asset_gate.py make_package.sh apk_pruefen.sh; do cp -p "release/$f" "$S/release/$f"; done
for f in apk_asset_gate.py make_package.sh apk_pruefen.sh; do cmp -s "release/$f" "$S/release/$f" && echo "   $f = Arbeitsbaum" || echo "   $f ANDERS"; done
cp -p "$L/TEX.TIM.orig" "$T" && echo "Paket-TEX.TIM zurueck: $(sha256sum "$T" | cut -c1-16)..."
echo FERTIG
