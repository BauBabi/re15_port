#!/usr/bin/env bash
# Nachbesserung R2 - make_package.sh ECHT (git-Schreibzugriffe per mp_isoliert_nb2.sh in einen Wegwerf-Index):
#  P  positiv: PC-Binaries aus dem Archiv v0.8.19 (Original-mtime, kein touch) + frisch gebaute APK
#  N1 APK waehrend des Gate-Selbsttests gegen die (gueltig signierte) Referenz-APK getauscht (B3)
#  N2 Quellbaum ohne RE15DOOR/P2DS.DO2 (B2) - Abbruch VOR den Kopierminuten
#  N3 --zip-only --only win, Paket mit Zusatzdatei unter shared_assets (B6: copy_common weicht ab)
#  N4 --zip-only --only linux, 1 Byte in shared_assets/PSX/DATA/TEX.TIM des Pakets gekippt (bisher ungeprueft)
# Danach: release/ zurueck (git restore der Split-Volumes/SUMS, Paketordner/Binaries/APK weg).
set -u
B=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
BEL=$B/analysis/befunde_runde34_android/nachbesserung_r2_belege
W=$B/build/r34a/nb2
L=$W/logs
cd "$B"
APK=release/re15_port_v0.8.19_android.apk
[[ -f $APK ]] || { echo "APK fehlt (erst build_android.sh)"; exit 3; }
cp -p $APK $W/apk/neu_v0.8.19.apk                                   # Sicherung der frischen APK
echo "frische APK: $(sha256sum $APK | cut -c1-16)... $(stat -c %s $APK) B, mtime $(stat -c %y $APK | cut -c1-19)"
bash $BEL/binaries_vorbereiten_nb2.sh > $L/binaries_vorbereiten.log 2>&1 || { echo "Binaries: Fehler (Log)"; tail -5 $L/binaries_vorbereiten.log; exit 4; }
tail -4 $L/binaries_vorbereiten.log
ergebnis() { printf '%-4s EXIT=%s (%s s) %s\n' "$1" "$(grep -E '^EXIT=' "$2" | cut -d= -f2)" \
    "$(awk 'NR==1{a=$1} /ENDE/{b=$1} END{printf "%.0f", b-a}' "$2")" "$(grep -m1 -E 'ABBRUCH|== Fertig ==' "$2" | cut -d' ' -f2- | cut -c1-200)"; }
# --- P
LOG=$L/mp_positiv.log bash $BEL/mp_isoliert_nb2.sh --version v0.8.19 > /dev/null 2>&1; ergebnis P $L/mp_positiv.log
grep -E "Quellbaum|Tuer-Soll|APK-ASSET-GATE-(PAKET|QUELLBAUM)-OK|APK-PRUEFUNG-OK|gepruefte APK|APK im Split-Satz|zipalign|Signer #1|erwarteter Signer" $L/mp_positiv.log | cut -d' ' -f2- | cut -c1-170 | sed 's/^/     /'
# --- N1: Tausch waehrend des Selbsttests gegen die Referenz (gueltig signiert, gleiche Version, andere Bytes)
cp -f build/r34a/ref_v0.8.19.apk $W/apk/tausch.apk; touch $W/apk/tausch.apk
( LOG=$L/mp_n1_tausch.log bash $BEL/mp_isoliert_nb2.sh --version v0.8.19 > /dev/null 2>&1 ) &
pid=$!
for i in $(seq 1 1500); do
  if grep -q "Volle Asset-Pruefung 1/2" $L/mp_n1_tausch.log 2>/dev/null; then mv -f $W/apk/tausch.apk $APK; echo "N1 $(date +%T): APK gegen die Referenz getauscht"; break; fi
  sleep 0.2
done
wait $pid; ergebnis N1 $L/mp_n1_tausch.log
grep -A2 -m1 "WAEHREND der Pruefung" $L/mp_n1_tausch.log | cut -d' ' -f2- | sed 's/^/     /'
cp -p $W/apk/neu_v0.8.19.apk $APK                                   # frische APK zurueck
# --- N2: Quellbaum ohne P2DS (Falle stellt die Datei in jedem Fall zurueck)
mv re15_port/shared_assets/RE15DOOR/P2DS.DO2 $W/P2DS.geparkt
trap 'mv -f $W/P2DS.geparkt re15_port/shared_assets/RE15DOOR/P2DS.DO2 2>/dev/null' EXIT
LOG=$L/mp_n2_ohne_p2ds.log bash $BEL/mp_isoliert_nb2.sh --version v0.8.19 > /dev/null 2>&1; ergebnis N2 $L/mp_n2_ohne_p2ds.log
grep -m2 -E "Port-Tuerarchiv fehlt|Assets kopieren" $L/mp_n2_ohne_p2ds.log | cut -d' ' -f2- | cut -c1-170 | sed 's/^/     /'
mv -f $W/P2DS.geparkt re15_port/shared_assets/RE15DOOR/P2DS.DO2; trap - EXIT
echo "     P2DS zurueck: $(sha256sum re15_port/shared_assets/RE15DOOR/P2DS.DO2 | cut -c1-16)..., git status: '$(git status --short re15_port/ | tr '\n' ' ')'"
# --- N3/N4 auf den Paketordnern aus P (--zip-only)
NAME=re15_port_v0.8.19
echo zusatz > release/pkg-win/$NAME/shared_assets/RE2/EXTRA.BIN
LOG=$L/mp_n3_zusatz_win.log bash $BEL/mp_isoliert_nb2.sh --version v0.8.19 --zip-only --only win > /dev/null 2>&1; ergebnis N3 $L/mp_n3_zusatz_win.log
grep -m1 -E "zusaetzlich im Paket" $L/mp_n3_zusatz_win.log | cut -d' ' -f2- | cut -c1-170 | sed 's/^/     /'
rm -f release/pkg-win/$NAME/shared_assets/RE2/EXTRA.BIN
T=release/pkg-linux/$NAME/shared_assets/PSX/DATA/TEX.TIM
/c/Python310/python -c "import sys; p=sys.argv[1]; b=bytearray(open(p,'rb').read()); b[1000]^=1; open(p,'wb').write(b)" $T
LOG=$L/mp_n4_psx_byte_linux.log bash $BEL/mp_isoliert_nb2.sh --version v0.8.19 --zip-only --only linux > /dev/null 2>&1; ergebnis N4 $L/mp_n4_psx_byte_linux.log
grep -m1 -E "Inhalt weicht ab" $L/mp_n4_psx_byte_linux.log | cut -d' ' -f2- | cut -c1-170 | sed 's/^/     /'
# alter check_tree (Stand vor R2, per awk unveraendert) auf demselben Paket: haette er es bemerkt?
git show e1640cd0:release/make_package.sh > $W/make_package_alt.sh
cat > $W/check_tree_alt_sonde.sh <<'SH'
set -euo pipefail
die() { echo "ABBRUCH: $*" >&2; exit 1; }
REPO=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
RE2="$REPO/re15_port/shared_assets/RE2"; RE15DOOR="$REPO/re15_port/shared_assets/RE15DOOR"; SYNCHRO="$REPO/synchro"
FX_REQUIRED=(effect0_blood.tim effect2_muzzle.tim effect3_smoke.tim effect4_shell.tim)
eval "$(awk '/^check_tree\(\) \{/{f=1} f{print} f&&/^}$/{exit}' "$1")"
check_tree "$2" && echo "CHECK_TREE_ALT_OK"
SH
bash $W/check_tree_alt_sonde.sh $W/make_package_alt.sh release/pkg-linux/$NAME > $L/check_tree_alt_n4.log 2>&1; echo "N4-alt check_tree vor R2 auf dem gekippten Paket: rc=$? $(tail -1 $L/check_tree_alt_n4.log)"
/c/Python310/python -c "import sys; p=sys.argv[1]; b=bytearray(open(p,'rb').read()); b[1000]^=1; open(p,'wb').write(b)" $T
cmp $T re15_port/shared_assets/PSX/DATA/TEX.TIM && echo "     TEX.TIM im Paket zurueck = Quelle"
