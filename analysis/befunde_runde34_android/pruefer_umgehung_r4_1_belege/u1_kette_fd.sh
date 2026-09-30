#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1: Nachtrag zu u1_kette.sh Teil A - die signierte Faelschung FD_sig (N2 + RE15DOOR/P07G.DO2 mit
# 1 gekipptem Byte, Listenzeile mit passender sha256 = das Geraet entpackt sie klaglos) durch build_android.sh
# --gate-only (Sandbox), echtes Gate gegen G0 (leer) und G2 (RC_ABWEICHUNG = 0).
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_u1; S=$W/sb; L=$W/logs/kette; A=$W/apk; M=$W/mut
gate_setzen() { if [[ "$1" == ECHT ]]; then cp -p release/apk_asset_gate.py "$S/release/apk_asset_gate.py"; else cp "$1" "$S/release/apk_asset_gate.py"; fi
                echo "--- Sandbox-Gate := $1 ($(sha256sum "$S/release/apk_asset_gate.py" | cut -c1-16)...)"; }
ga() { local tag="$1" apk="$2" ver="$3" rc=0 t0 t1; t0=$(date +%s)
       bash "$S/release/build_android.sh" --gate-only "$apk" --version "$ver" > "$L/$tag.log" 2>&1 || rc=$?
       t1=$(date +%s); echo "== A $tag ($(basename "$apk") --version $ver): EXIT=$rc ($((t1 - t0)) s)"
       grep -a -E 'ANDROID-GATES-OK|^ABBRUCH:|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE-(OK|ABWEICHUNG)|Inhalt weicht ab|P07G' "$L/$tag.log" | tr -d '\r' | sed 's/^/     /' | cut -c1-190; }
gate_setzen ECHT;                               ga A8_echt_FD "$A/FD_sig.apk" v0.8.20-n1f
gate_setzen "$M/G0_leer/apk_asset_gate.py";     ga A9_G0_FD   "$A/FD_sig.apk" v0.8.20-n1f
gate_setzen "$M/G2_rc_abweichung_0/apk_asset_gate.py"; ga A10_G2_FD "$A/FD_sig.apk" v0.8.20-n1f
gate_setzen ECHT;                               ga A11_echt_FK "$A/FK_sig.apk" v0.8.20-n1f
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" && echo "   Sandbox-Gate = Arbeitsbaum"
echo FERTIG
