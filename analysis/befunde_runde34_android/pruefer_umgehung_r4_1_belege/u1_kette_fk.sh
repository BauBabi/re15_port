#!/usr/bin/env bash
# Pruefer UMGEHUNG R4-1, H5: liegt das Kelvin-Paar auch im QUELLBAUM (auf NTFS moeglich: K.bin und U+212A.bin sind dort
# zwei Dateien, gemessen), besteht FK_sig die ganze Kette mit dem ECHTEN Gate? Sandbox-Quellbaum: die zwei Dateien als
# eigene Dateien in <sandbox>/re15_port/shared_assets/PSX/ (Hardlink-Ordner der Sandbox; das Original bleibt unberuehrt),
# danach wieder entfernt.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_u1; S=$W/sb; L=$W/logs/kette; A=$W/apk; PY=/c/Python310/python
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" || { echo "Sandbox-Gate nicht echt"; exit 1; }
"$PY" -c "
import os, sys
d = sys.argv[1]
for n, b in (('K.bin', b'AAAAA'), ('\u212a.bin', b'BBBBB')):
    open(os.path.join(d, n), 'wb').write(b)
print('   Sandbox-Quellbaum PSX: ', sorted(ascii(x) for x in os.listdir(d) if x.endswith('.bin') and len(x) <= 6))
print('   Original-Quellbaum PSX:', sorted(ascii(x) for x in os.listdir(sys.argv[2]) if x.endswith('.bin') and len(x) <= 6))
" "$S/re15_port/shared_assets/PSX" re15_port/shared_assets/PSX
rc=0; t0=$(date +%s)
bash "$S/release/build_android.sh" --gate-only "$A/FK_sig.apk" --version v0.8.20-n1f > "$L/A12_echt_FK_quelle_mit_paar.log" 2>&1 || rc=$?
echo "== A A12_echt_FK_quelle_mit_paar (FK_sig.apk, Quellbaum mit Kelvin-Paar): EXIT=$rc ($(( $(date +%s) - t0 )) s)"
grep -a -E 'ANDROID-GATES-OK|^ABBRUCH:|SELBSTTEST-(OK|FEHLER)|APK-ASSET-GATE-(OK|ABWEICHUNG)|zusaetzlich|Manifest:|shared_assets/PSX ' "$L/A12_echt_FK_quelle_mit_paar.log" | tr -d '\r' | sed 's/^/     /' | cut -c1-200
"$PY" -c "
import os, sys
for n in ('K.bin', '\u212a.bin'):
    os.remove(os.path.join(sys.argv[1], n))
print('   entfernt; Sandbox-PSX jetzt', len(os.listdir(sys.argv[1])), 'Eintraege oben')" "$S/re15_port/shared_assets/PSX"
echo FERTIG
