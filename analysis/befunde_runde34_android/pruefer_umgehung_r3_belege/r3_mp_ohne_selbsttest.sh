#!/usr/bin/env bash
# Gegenpruefung R3: make_package.sh benutzt apk_asset_gate.py --quellbaum/--paket OHNE vorherigen --selbsttest
# (der laeuft nur in apk_pruefen, also nur mit APK). Ist das Gate kaputt, merkt es im PC-Pfad niemand?
# Sandbox sb_mp (nach r3_mp_veraltet.sh: pkg-linux vorhanden). --zip-only nimmt den vorhandenen Paketordner.
#   Eingriff: 1 Byte in der PAKET-Kopie von shared_assets/PSX/DATA/TEX.TIM (echte Kopie, Linkzahl 1 geprueft)
#   Lauf A  echtes Gate                   -> soll EXIT 1 (Paketpruefung)
#   Mutant  MUP: in paket_pruefen 'if q_sha != p_sha:' -> 'if False:'  (sein --selbsttest: soll FEHLER)
#   Lauf B  Sandbox-Gate := MUP            -> ?
# Danach Sandbox-Gate zurueck, Paketdatei zurueck.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; S=$W/sb_mp; L=$W/logs/mp2; mkdir -p "$L" "$W/mutanten/MUP_paket_sha_aus"
PY=/c/Python310/python
T="$S/release/pkg-linux/re15_port_v0.8.19/shared_assets/PSX/DATA/TEX.TIM"
[[ -f "$T" ]] || { echo "Paketordner fehlt: $T"; exit 1; }
echo "Linkzahl Paket-TEX.TIM: $(stat -c %h "$T") (1 = eigene Kopie)"; [[ "$(stat -c %h "$T")" == 1 ]] || exit 1
cp -p "$T" "$L/TEX.TIM.orig"
"$PY" -c "import sys; p=sys.argv[1]; d=bytearray(open(p,'rb').read()); d[len(d)//2]^=0x01; open(p,'wb').write(d)" "$T"
echo "Paket-TEX.TIM jetzt $(sha256sum "$T" | cut -c1-16)..., Quelle $(sha256sum re15_port/shared_assets/PSX/DATA/TEX.TIM | cut -c1-16)..."
mp() {      # $1 = Tag
    local rc=0
    bash "$S/release/make_package.sh" --version v0.8.19 --only linux --zip-only > "$L/$1.log" 2>&1 || rc=$?
    echo "== $1: EXIT=$rc"
    grep -a -E '^ABBRUCH|PAKET-(OK|ABWEICHUNG)|Inhalt weicht ab|Selbsttest|SELBSTTEST|kein Android|== Fertig' "$L/$1.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-190
}
mp laufA_echtes_gate
"$PY" - release/apk_asset_gate.py "$W/mutanten/MUP_paket_sha_aus/apk_asset_gate.py" <<'PY'
import sys
t = open(sys.argv[1], encoding="utf-8", newline="").read()
alt = "        if q_sha != p_sha:\n"
assert t.count(alt) == 1
open(sys.argv[2], "w", encoding="utf-8", newline="").write(t.replace(alt, "        if False:\n"))
PY
diff release/apk_asset_gate.py "$W/mutanten/MUP_paket_sha_aus/apk_asset_gate.py"
rc=0; "$PY" "$W/mutanten/MUP_paket_sha_aus/apk_asset_gate.py" --selbsttest > "$L/selbsttest_MUP.log" 2>&1 || rc=$?
echo "Selbsttest MUP: rc=$rc $(grep -a -E '^== SELBSTTEST' "$L/selbsttest_MUP.log" | tr -d '\r')"
grep -a -E '\[FEHLER\]' "$L/selbsttest_MUP.log" | tr -d '\r' | sed 's/^/   /' | cut -c1-150
cp "$W/mutanten/MUP_paket_sha_aus/apk_asset_gate.py" "$S/release/apk_asset_gate.py"
mp laufB_gate_MUP_ohne_selbsttest
cp release/apk_asset_gate.py "$S/release/apk_asset_gate.py"
cmp -s release/apk_asset_gate.py "$S/release/apk_asset_gate.py" && echo "Sandbox-Gate wieder = Arbeitsbaum"
cp -p "$L/TEX.TIM.orig" "$T" && echo "Paket-TEX.TIM zurueck: $(sha256sum "$T" | cut -c1-16)..."
echo FERTIG
