#!/usr/bin/env bash
# Nachbesserung R1: make_package.sh ECHT (isoliert, mp_isoliert_nb.sh) mit APKs, die NICHT gezippt werden duerfen.
# Kurze Faelle brechen vor den Kopierminuten ab; die Tausch-Faelle (B5) wechseln die APK WAEHREND des Kopierens.
set -u
BAUM=C:/workspace/git/reAi_v2/.claude/worktrees/r34a_android
cd "$BAUM" || exit 99
NB=build/r34a/nb; APK=release/re15_port_v0.8.19_android.apk; SICHER=$NB/apk_positiv_sicher.apk
ERG=$NB/mp_negativ_ergebnis.txt; MP=analysis/befunde_runde34_android/nachbesserung_r1_belege/mp_isoliert_nb.sh
[[ -f "$SICHER" ]] || cp -p "$APK" "$SICHER"
MODUS="${1:-alle}"
[[ "$MODUS" == tausch ]] || echo "# make_package.sh --version v0.8.19 isoliert, Negativfaelle ($(date '+%F %T')); positive APK gesichert: $(sha256sum "$SICHER" | cut -c1-16)" > "$ERG"
kurz() {  # $1 = Name, $2 = Vorbereitung (bash -c), Rest = make_package-Argumente
    local name="$1" vorb="$2"; shift 2
    bash -c "$vorb"
    local rc=0 t0=$(date +%s)
    LOG="$NB/mp_$name.log" bash "$MP" "$@" > /dev/null 2>&1 || rc=$?
    printf '%-22s EXIT=%d (%3d s) | %s\n' "$name" "$rc" "$(( $(date +%s) - t0 ))" \
        "$(grep -E 'ABBRUCH|VERALTET|versionName|DOES NOT|unzulaessiger|Fertig|Split-Satz' "$NB/mp_$name.log" | sed 's/^[0-9.]* //' | head -2 | cut -c1-160 | tr '\n' '#')" >> "$ERG"
    cp -p "$SICHER" "$APK"; touch "$APK"
}
if [[ "$MODUS" != tausch ]]; then
kurz apk_veraltet   "cp -p $SICHER $APK; touch -d '2026-09-29 20:00:00' $APK"   --version v0.8.19
kurz apk_version    "cp $SICHER release/re15_port_v0.8.20_android.apk"          --version v0.8.20
rm -f release/re15_port_v0.8.20_android.apk
kurz apk_unsigniert "cp $NB/apk/K0_neu.apk $APK"                                --version v0.8.19
kurz apk_f2_signiert "cp $NB/apk/F2_signiert.apk $APK"                          --version v0.8.19
fi

tausch() {  # $1 = Name, $2 = Vorbereitung, $3 = Eingriff waehrend des Kopierens
    local name="$1" vorb="$2" eingriff="$3"
    local log="$NB/mp_$name.log" rc=0 t0
    t0=$(date +%s)
    bash -c "$vorb"
    ( LOG="$log" bash "$MP" --version v0.8.19 > /dev/null 2>&1; echo "RC=$?" > "$NB/mp_$name.rc" ) &
    local pid=$!
    until grep -q "Assets kopieren (shared_assets/PSX" "$log" 2>/dev/null || ! kill -0 "$pid" 2>/dev/null; do sleep 0.5; done
    bash -c "$eingriff"; echo "   $name: Eingriff $(date +%T): $eingriff" >> "$ERG"
    wait "$pid"; rc=$(sed 's/RC=//' "$NB/mp_$name.rc")
    printf '%-22s EXIT=%s (%3d s) | %s\n' "$name" "$rc" "$(( $(date +%s) - t0 ))" \
        "$(grep -E 'ABBRUCH|geprueft:|jetzt:|aufgetaucht|Fertig' "$log" | sed 's/^[0-9.]* //' | head -3 | cut -c1-160 | tr '\n' '#')" >> "$ERG"
    cp -p "$SICHER" "$APK"; touch "$APK"
}
tausch apk_getauscht   "cp -p $SICHER $APK; touch $APK"  "cp $NB/apk/F1_crc_koll_sig.apk $APK"
tausch apk_spaeter_da  "rm -f $APK"                       "cp $SICHER $APK"
echo "ENDE $(date '+%F %T')" >> "$ERG"
cat "$ERG"
