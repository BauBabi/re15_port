#!/usr/bin/env bash
# Gegenpruefung R3: neue Faelschungen (vom Bauer nicht getestet) an Kopien der Referenz-APK, gegen das
# ECHTE Gate und - wo einschlaegig - gegen die Mutanten aus r3_mutanten.py. Unsigniert (das Gate prueft keine
# Signatur; die Kette mit signierten Kopien laeuft getrennt). Rueckgabe je Lauf selbst abgefangen.
cd C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android || exit 9
W=build/r34a/pruefer_r3; B=analysis/befunde_runde34_android/pruefer_umgehung_r3_belege
PY=/c/Python310/python; R=build/r34a/ref_v0.8.19.apk; F=$W/apk; L=$W/logs/bat; mkdir -p "$F" "$L"
P07=shared_assets/RE15DOOR/P07G.DO2
lauf() {   # $1 = Fall, $2 = soll, $3 = apk, [$4 = Gate-Datei, Standard das echte]
    local rc=0 gate="${4:-release/apk_asset_gate.py}" tag
    tag="$1"; [[ -n "${4:-}" ]] && tag="$1@$(basename "$(dirname "$gate")")"
    "$PY" "$gate" --repo . "$3" > "$L/$tag.log" 2>&1 || rc=$?
    printf '%-52s soll %s ist %s %-10s | %s\n' "$tag" "$2" "$rc" "$([[ $rc == "$2" ]] && echo ok || echo ABWEICHUNG)" \
        "$(grep -m2 -E '^      |ABBRUCH|GATE-OK' "$L/$tag.log" | sed 's/^ *//' | tr '\n' ' ' | cut -c1-230)"
}
f() { local z="$1"; shift; "$PY" $B/r3_faelschen.py $R "$F/$z.apk" "$@" > "$L/faelschen_$z.txt" 2>&1 \
        || echo "FAELSCHEN FEHLGESCHLAGEN: $z $*"; }
M=$W/mutanten
head -c 55908 /dev/zero > $W/null55908.bin
: > $W/leer.bin

f K0                                                                 # Kontrolle: Neuschreiben ohne Eingriff
f F1_leerzeichen  --manifest-sub "$P07\\n=$P07 \\n"
f F2_vollbreit    --manifest-sub "55908\\t$P07=\\uff15\\uff15\\uff19\\uff10\\uff18\\t$P07"
f F2b_arabisch    --manifest-sub "55908\\t$P07=\\u0665\\u0665\\u0669\\u0660\\u0668\\t$P07"
f F3_nul_ende     --manifest-anhang "\\x00"
f F4_ff_ende      --manifest-sub "$P07\\n=$P07\\x0c\\n"
f F5_fuehrende_0  --manifest-sub "55908\\t$P07=055908\\t$P07"
f F6_u2028        --manifest-sub "\\n55908\\t$P07=\\u202855908\\t$P07"
f F7_kleinschrift --zusatz "assets/shared_assets/RE15DOOR/p07g.do2=$W/null55908.bin" \
                  --manifest-sub "# re15 assets 3603 356678277\\n=# re15 assets 3604 356734185\\n55908\\tshared_assets/RE15DOOR/p07g.do2\\n"
f F8_leerer_name  --zusatz "=$W/leer.bin"
f F9_vt_ende      --manifest-sub "$P07\\n=$P07\\x0b\\n"
f F10_nbsp_ende   --manifest-sub "$P07\\n=$P07\\u00a0\\n"

echo "--- echtes Gate ---"
lauf K0 0 $F/K0.apk
lauf F1_leerzeichen 1 $F/F1_leerzeichen.apk
lauf F2_vollbreit 1 $F/F2_vollbreit.apk
lauf F2b_arabisch 1 $F/F2b_arabisch.apk
lauf F3_nul_ende 1 $F/F3_nul_ende.apk
lauf F4_ff_ende 1 $F/F4_ff_ende.apk
lauf F5_fuehrende_0 0 $F/F5_fuehrende_0.apk
lauf F6_u2028 1 $F/F6_u2028.apk
lauf F7_kleinschrift 1 $F/F7_kleinschrift.apk
lauf F8_leerer_name 0 $F/F8_leerer_name.apk
lauf F9_vt_ende 1 $F/F9_vt_ende.apk
lauf F10_nbsp_ende 1 $F/F10_nbsp_ende.apk
echo "--- Mutanten gegen ihre Faelschungen (soll = was das ECHTE Gate sagt) ---"
lauf F1_leerzeichen 1 $F/F1_leerzeichen.apk $M/MU1_manifest_rstrip_alles/apk_asset_gate.py
lauf F4_ff_ende 1 $F/F4_ff_ende.apk $M/MU1_manifest_rstrip_alles/apk_asset_gate.py
lauf F9_vt_ende 1 $F/F9_vt_ende.apk $M/MU1_manifest_rstrip_alles/apk_asset_gate.py
lauf F2_vollbreit 1 $F/F2_vollbreit.apk $M/MU2_groesse_unicode_ziffern/apk_asset_gate.py
lauf F2b_arabisch 1 $F/F2b_arabisch.apk $M/MU2_groesse_unicode_ziffern/apk_asset_gate.py
lauf F2_vollbreit 1 $F/F2_vollbreit.apk $M/MU6_groesse_isdigit/apk_asset_gate.py
lauf F4_ff_ende 1 $F/F4_ff_ende.apk $M/MU3_splitlines/apk_asset_gate.py
lauf F6_u2028 1 $F/F6_u2028.apk $M/MU3_splitlines/apk_asset_gate.py
lauf F1_leerzeichen 1 $F/F1_leerzeichen.apk $M/MU8_pfad_strip/apk_asset_gate.py
lauf F10_nbsp_ende 1 $F/F10_nbsp_ende.apk $M/MU8_pfad_strip/apk_asset_gate.py
echo "FERTIG"
