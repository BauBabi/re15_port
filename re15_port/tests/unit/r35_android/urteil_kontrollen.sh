#!/usr/bin/env bash
# =============================================================================================
# Runde 35 Spur N "android", Nachbesserung 2 (Abnahme 1, M3) - Kontrollen des BASH-Urteils in release/apk_pruefen.sh
# (gate_pin_pruefen, gate_urteil_pin_pruefen, gate_urteil_selbsttest, gate_festhalten, gate_urteil, gate_laufen).
# Nutzer: "Kuenftige Aenderungen am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten."
# Dossier: analysis/befunde_runde35/N_android.md, Abschnitt "Nachbesserung 2".
#
# Nur Attrappen (kein echtes Gate, kein echtes Urteil) - deshalb schnell genug, um auch gegen jeden MUTANTEN des
# bash-Urteils zu laufen (bash_urteil_mutanten.py, ctest unit_r35_android_bash_mutanten). Je Kontrolle genau EINE
# verletzte Bedingung; Soll = genaue Rueckgabe bzw. Abbruch MIT der Meldung genau dieser Pruefzeile. Die
# Positiv-Kontrollen (P*) muessen angenommen werden - sonst waeren die Abbrueche wertlos. Jede Vergleichsstelle wird von
# BEIDEN Seiten geprueft (Abnahme 1 M3: vorher kannte N8 nur Gate-Rueckgabe 1, N10 nur Selbsttest-Rueckgabe 1, N11 nur
# f1 < f2, N13 nur e + g < m, und keine Kontrolle hatte eine leere Zahl). Jede Kontrolle laeuft in einer Unterschale mit
# "set -euo pipefail" - wie bei den echten Aufrufern (Kopf von apk_pruefen.sh).
#
# AUFRUF
#   urteil_kontrollen.sh anlegen <apk_pruefen.sh> <attrappen-ordner>      Attrappen + Pins einmal anlegen (die Zahlen
#                                                                          F/E/G aus GATE_URTEIL_MIN_* dieser Datei)
#   urteil_kontrollen.sh pruefen <apk_pruefen.sh> <attrappen> <arbeit> [--schnell]
#       Zeilen "ok      <name>" / "FALSCH  <name> ..."; Schluss "KONTROLLEN: n ok, m FALSCH"; Rueckgabe 0 nur ohne
#       FALSCH. --schnell: Ende beim ersten FALSCH (Mutantenlauf). PY wird aus der Umgebung genommen, wenn gesetzt.
# =============================================================================================
set -u
HIER="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
RELEASE="$(cd "$HIER/../../../../release" && pwd)"
if [[ -z "${PY:-}" ]]; then
    # shellcheck source=/dev/null
    source "$RELEASE/python_finden.sh" > /dev/null 2>&1 || { echo "kein Python >= 3.8"; exit 9; }
fi
pin() { sha256sum < "$1" | cut -c1-64 > "$2"; }   # stdin: bei einem Pfad mit Backslash setzt sha256sum einen davor
zahlen() {   # $1 = apk_pruefen.sh -> F E G (GATE_URTEIL_MIN_FAELLE, _MIN_ERKANNT, _MAX_GLEICH), ohne die Datei zu laden
    local f e g
    f="$(sed -n 's/^GATE_URTEIL_MIN_FAELLE=\([0-9]*\)$/\1/p' "$1")"
    e="$(sed -n 's/^GATE_URTEIL_MIN_ERKANNT=\([0-9]*\)$/\1/p' "$1")"
    g="$(sed -n 's/^GATE_URTEIL_MAX_GLEICH=\([0-9]*\)$/\1/p' "$1")"
    [[ -n "$f" && -n "$e" && -n "$g" ]] || { echo "Mindestzahlen in $1 nicht lesbar"; exit 9; }
    echo "$f $e $g"
}
okz() { echo "== URTEIL-SELBSTTEST-OK: $1/$2 Faelle, $3/$4 Mutanten erkannt, $5 als gleichwertig begruendet =="; }
UZ="   Gate-Urteil (<modus>, Rueckgabe <rc>): Attrappe"   # ehrliche Urteilszeile (<modus>/<rc> setzt die Attrappe ein)

# ------------------------------------------------------------------------------------------- Attrappen anlegen
anlegen() {
    local AP="$1" A="$2" F E G M OK i w v
    read -r F E G <<< "$(zahlen "$AP")"
    M=$((E + G)); OK="$(okz "$F" "$F" "$E" "$M" "$G")"
    rm -rf "$A" && mkdir -p "$A" || exit 9
    echo "$F $E $G" > "$A/zahlen"
    printf '%s\n' 'print("== SELBSTTEST-OK: Attrappe ==")' 'raise SystemExit(0)' > "$A/G0.py"
    printf '%s\n' 'print("== SELBSTTEST-FEHLER: Attrappe ==")' 'raise SystemExit(1)' > "$A/G1.py"
    printf '%s\n' 'print("== SELBSTTEST-OK: Attrappe ==")' 'raise SystemExit(2)' > "$A/G2.py"
    for i in G0 G1 G2; do pin "$A/$i.py" "$A/pin_$i"; done
    echo "== SELBSTTEST-OK: x ==" > "$A/G0.txt"
    : > "$A/U_leer.py"; pin "$A/U_leer.py" "$A/pin_U_leer"
    printf 'abc\n' > "$A/pin_kaputt"; printf '%064d\n' 0 | tr 0 g > "$A/pin_nichthex"; head -c 63 "$A/pin_G0" > "$A/pin_63"
    # Urteils-Attrappe: $1 Name, $2 Rueckgabe des Selbsttests, $3 Schlusszeile, $4 Urteilszeile ("" = keine),
    # $5 Rueckgabe des Urteils ("ehrlich" = 0/1/2 nach der Gate-Rueckgabe), [$6 Zeile NACH der Schlusszeile]
    ua() {
        {
            echo "import sys"
            echo "if sys.argv[1:] == ['--selbsttest']:"
            echo "    print('== gate_urteil.py: Selbsttest (Attrappe) ==')"
            echo "    print('$3')"
            [[ -n "${6:-}" ]] && echo "    print('$6')"
            echo "    sys.exit($2)"
            echo "rc = int(sys.argv[3])"
            [[ -n "$4" ]] && echo "print('$4'.replace('<modus>', sys.argv[1]).replace('<rc>', sys.argv[3]))"
            if [[ "$5" == ehrlich ]]; then echo "sys.exit(0 if rc == 0 else (1 if rc == 1 else 2))"; else echo "sys.exit($5)"; fi
        } > "$A/U_$1.py"
        pin "$A/U_$1.py" "$A/pin_U_$1"
    }
    ua gut 0 "$OK" "$UZ" ehrlich
    ua r1 1 "$OK" "$UZ" ehrlich
    ua r2 2 "$OK" "$UZ" ehrlich
    ua f1k 0 "$(okz $((F - 1)) "$F" "$E" "$M" "$G")" "$UZ" ehrlich
    ua f1g 0 "$(okz $((F + 1)) "$F" "$E" "$M" "$G")" "$UZ" ehrlich
    ua fmin 0 "$(okz $((F - 1)) $((F - 1)) "$E" "$M" "$G")" "$UZ" ehrlich
    ua sk 0 "$(okz "$F" "$F" "$E" $((M + 1)) "$G")" "$UZ" ehrlich
    ua sg 0 "$(okz "$F" "$F" "$E" $((M - 1)) "$G")" "$UZ" ehrlich
    ua emin 0 "$(okz "$F" "$F" $((E - 1)) $((M - 1)) "$G")" "$UZ" ehrlich
    ua gmax 0 "$(okz "$F" "$F" "$E" $((M + 1)) $((G + 1)))" "$UZ" ehrlich
    ua nz 0 "$OK" "$UZ" ehrlich "nachgeschoben"
    ua zs 0 "$OK X" "$UZ" ehrlich
    ua vs 0 "X $OK" "$UZ" ehrlich
    for i in 1 2 3 4 5; do
        for w in leer x; do
            v=("$F" "$F" "$E" "$M" "$G")
            if [[ "$w" == leer ]]; then v[$((i - 1))]=""; else v[$((i - 1))]="x"; fi
            ua "z${i}_$w" 0 "$(okz "${v[0]}" "${v[1]}" "${v[2]}" "${v[3]}" "${v[4]}")" "$UZ" ehrlich
        done
    done
    for w in URTEIL SELBSTTEST OK Faelle Mutanten erkannt als gleichwertig begruendet; do
        ua "w_$w" 0 "$(echo "$OK" | sed "s/\\b$w\\b/XX/")" "$UZ" ehrlich
    done
    ua luegt 0 "$OK" "   Gate-Urteil (<modus>, Rueckgabe 0): alles bestens" 0
    ua ohne 0 "$OK" "" 0
    ua modus 0 "$OK" "   Gate-Urteil (apk, Rueckgabe 0): falscher Modus" 0
    ua zrc1 0 "$OK" "   Gate-Urteil (<modus>, Rueckgabe 1): x" 0
    ua zvor 0 "$OK" "X   Gate-Urteil (<modus>, Rueckgabe 0): x" 0
    ua zdp 0 "$OK" "   Gate-Urteil (<modus>, Rueckgabe 0) x" 0
    ua zein 0 "$OK" "  Gate-Urteil (<modus>, Rueckgabe 0): x" 0
    for w in Gate Urteil Rueckgabe; do
        ua "zw_$w" 0 "$OK" "$(echo "   Gate-Urteil (<modus>, Rueckgabe 0): x" | sed "s/\\b$w\\b/XX/")" 0
    done
    echo "Attrappen angelegt in $A (F=$F E=$E G=$G)"
}

# ------------------------------------------------------------------------------------------- Pruefen
pruefen() {
    AP="$1"; A="$2"; W="$3"; SCHNELL="${4:-}"
    rm -rf "$W" && mkdir -p "$W" || exit 9
    read -r F E G < "$A/zahlen"
    [[ "$(zahlen "$AP")" == "$F $E $G" ]] || { echo "FALSCH  Attrappen fuer andere Mindestzahlen angelegt ($F $E $G)"; exit 1; }
    M=$((E + G))
    die() { echo "DIE: $*" >&2; exit 1; }
    # shellcheck source=/dev/null
    source "$AP" > /dev/null 2>&1 || { echo "FALSCH  apk_pruefen.sh laesst sich nicht laden"; exit 1; }
    n_ok=0; n_falsch=0
    fertig() { echo "KONTROLLEN: $n_ok ok, $n_falsch FALSCH"; exit $(( n_falsch > 0 )); }
    k() {   # $1 Name, $2 Soll ("0" | "1" | "2" | "die:<Meldung>" | "0:<Ausgabe>"), Rest = Befehl (Unterschale, set -e)
        local t="$1" s="$2" u gut=0 out="$W/$1.txt"
        shift 2
        ( set -euo pipefail; "$@" ) > "$out" 2>&1
        u=$?
        case "$s" in
            die:*) [[ "$u" != 0 ]] && grep -aqF -- "${s#die:}" "$out" && gut=1 ;;
            0:*)   [[ "$u" == 0 ]] && grep -aqF -- "${s#0:}" "$out" && gut=1 ;;
            *)     [[ "$u" == "$s" ]] && gut=1 ;;
        esac
        if (( gut )); then
            n_ok=$((n_ok + 1)); echo "ok      $t"
        else
            n_falsch=$((n_falsch + 1))
            echo "FALSCH  $t -> Rueckgabe $u (soll $s): $(grep -a -v '^ *$' "$out" | tail -2 | tr '\n' ' ' | cut -c1-170)"
            [[ -n "$SCHNELL" ]] && fertig
        fi
        return 0
    }
    mit() {   # Urteils-Attrappe $1 (gepinnt, ungeprueft) und Gate-Pin $2
        GATE_URTEIL_PIN_DATEI="$A/pin_U_$1"; GATE_URTEIL_KOPIE="$A/U_$1.py"; GATE_URTEIL_GEPRUEFT=""
        GATE_PIN_DATEI="$A/pin_$2"
    }
    st() { mit "$1" G0; gate_urteil_selbsttest "$A/U_$1.py"; }                 # Selbsttest der Urteils-Attrappe $1
    lauf() { mit "$1" "$2"; gate_laufen selbsttest "$A/$2.py" --selbsttest; }   # gate_laufen: Urteil $1, Gate $2
    fh() {   # gate_festhalten mit Attrappen-Quellen (Gate G0, Urteil gut) in $W/fest_$1; Rest per eval davor
        GATE_QUELLE="$A/G0.py"; GATE_PIN_DATEI="$A/pin_G0"; GATE_URTEIL_QUELLE="$A/U_gut.py"
        GATE_URTEIL_PIN_DATEI="$A/pin_U_gut"; GATE_URTEIL_GEPRUEFT=""
        local d="$W/fest_$1"; shift
        mkdir -p "$d"; eval "$*"; gate_festhalten "$d"
    }
    MS="Selbsttest des Gate-Urteils ohne gueltige Schlusszeile"
    MR="Selbsttest des Gate-Urteils: OK-Schlusszeile, aber Rueckgabe"
    MF="Faelle, verlangt n/n"
    MM="Mutanten erkannt, $G gleichwertig - verlangt"
    local i w

    # ---- P: Positiv-Kontrollen ZUERST (ein Mutant, der den guten Weg bricht, faellt hier)
    k P3_selbsttest_angenommen "0:Gate-Urteil selbstgeprueft: $F/$F Faelle, $E/$M Mutanten erkannt, $G gleichwertig" st gut
    k P4_gate_0_urteil_0 0 lauf gut G0
    k P5_gate_1_befund_bleibt_1 1 lauf gut G1
    k P5b_gate_2_bleibt_2 2 lauf gut G2
    c_P6() { fh p6 :; gate_laufen selbsttest "$GATE_KOPIE" --selbsttest; echo "P6 Ende"; }
    k P6_festhalten_und_laufen "0:P6 Ende" c_P6
    c_P7() { mit gut G0; gate_urteil selbsttest "$A/G0.txt" 0; }
    k P7_gate_urteil_direkt 0 c_P7
    c_P8() { mit gut G0; gate_urteil_selbsttest "$A/U_gut.py"; gate_urteil_selbsttest "$A/U_gut.py"; echo "P8 Ende"; }
    k P8_zweiter_aufruf_aus_dem_zwischenspeicher "0:P8 Ende" c_P8
    c_D5p() { GATE_PIN_DATEI="$A/pin_G0"; gate_pin_pruefen "$A/G0.py"; echo "GATE_SHA256=$GATE_SHA256"; }
    k P9_gate_pin_richtig "0:GATE_SHA256=$(cat "$A/pin_G0")" c_D5p

    # ---- Selbsttest des Urteils: Rueckgabe (beide Seiten von rc == 0)
    k N10_ok_zeile_rueckgabe_1 "die:$MR 1" st r1
    k N10b_ok_zeile_rueckgabe_2 "die:$MR 2" st r2
    # ---- Faelle: f1 == f2 von beiden Seiten, Mindestzahl
    k N11_f1_kleiner_f2 "die:$MF" st f1k
    k N11b_f1_groesser_f2 "die:$MF" st f1g
    k N12_faelle_unter_mindestzahl "die:$MF" st fmin
    # ---- Mutanten: e + g == m von beiden Seiten, Mindestzahl erkannt, Hoechstzahl gleichwertig
    k N13_summe_kleiner_alle "die:$MM" st sk
    k N13b_summe_groesser_alle "die:$MM" st sg
    k N14_erkannt_unter_mindestzahl "die:$MM" st emin
    k N15_zu_viele_gleichwertige "die:Mutanten erkannt, $((G + 1)) gleichwertig - verlangt" st gmax
    # ---- Schlusszeile: Lage, Anker, leeres Urteil
    k N16_ok_zeile_nicht_zuletzt "die:$MS" st nz
    k N17_ok_zeile_mit_zusatz "die:$MS" st zs
    k N18_ok_zeile_mit_vorsatz "die:$MS" st vs
    k N7_urteil_leer "die:$MS" st leer
    # ---- je Zahl der Schlusszeile: leer (N22) und keine Ziffer (N23); je Wort ein anderes Wort (N24)
    for i in 1 2 3 4 5; do
        k "N22_zahl${i}_leer" "die:$MS" st "z${i}_leer"
        k "N23_zahl${i}_keine_ziffer" "die:$MS" st "z${i}_x"
    done
    for w in URTEIL SELBSTTEST OK Faelle Mutanten erkannt als gleichwertig begruendet; do
        k "N24_wort_$w" "die:$MS" st "w_$w"
    done

    # ---- zweite Instanz in gate_laufen: Urteil 0 trotz Gate-Rueckgabe 1 bzw. 2 -> 2 (beide Seiten von rc != 0)
    k N8c_urteil_luegt_gate_1 2 lauf luegt G1
    k N8b_urteil_luegt_gate_2 2 lauf luegt G2
    k N8m_meldung_ueberstimmt 0 grep -aq "ueberstimmt (zweite Instanz)" "$W/N8b_urteil_luegt_gate_2.txt"
    # ---- zweite Instanz: Urteil 0 bei Gate-Rueckgabe 0, Urteilszeile fehlt oder weicht ab -> 2
    k N19a_ohne_urteilszeile 2 lauf ohne G0
    k N19m_meldung_ohne_zeile 0 grep -aq "ohne Urteilszeile" "$W/N19a_ohne_urteilszeile.txt"
    k N19b_zeile_anderer_modus 2 lauf modus G0
    k N19c_zeile_rueckgabe_1 2 lauf zrc1 G0
    k N19d_zeile_mit_vorsatz 2 lauf zvor G0
    k N19e_zeile_ohne_doppelpunkt 2 lauf zdp G0
    k N19f_zeile_zwei_leerzeichen 2 lauf zein G0
    for w in Gate Urteil Rueckgabe; do k "N19w_wort_$w" 2 lauf "zw_$w" G0; done

    # ---- Pins: Pin-Datei fehlt / kein SHA-256 / Datei fehlt / nicht lesbar / nicht festgehalten - Gate UND Urteil
    c_pg() { GATE_PIN_DATEI="$1"; gate_pin_pruefen "$2"; }
    k D1_gate_pin_fehlt "die:Gate-Pin fehlt" c_pg "$W/gibt_es_nicht" "$A/G0.py"
    k D2_gate_pin_kein_sha "die:ist kein SHA-256" c_pg "$A/pin_kaputt" "$A/G0.py"
    k D2b_gate_pin_nicht_hex "die:ist kein SHA-256" c_pg "$A/pin_nichthex" "$A/G0.py"
    k D2c_gate_pin_63_ziffern "die:ist kein SHA-256" c_pg "$A/pin_63" "$A/G0.py"
    k D3_gate_fehlt "die:Asset-Gate fehlt" c_pg "$A/pin_G0" "$W/gibt_es_nicht.py"
    c_D4() { GATE_PIN_DATEI="$A/pin_G0"; PY=false; gate_pin_pruefen "$A/G0.py"; }
    k D4_gate_nicht_lesbar "die:Asset-Gate nicht lesbar" c_D4
    k D5_gate_nicht_festgehalten "die:Asset-Gate ist NICHT das festgehaltene" c_pg "$A/pin_G0" "$A/G1.py"
    c_pu() { GATE_URTEIL_PIN_DATEI="$1"; gate_urteil_pin_pruefen "$2"; }
    k D6_urteil_pin_fehlt "die:Urteils-Pin fehlt" c_pu "$W/gibt_es_nicht" "$A/U_gut.py"
    k D7_urteil_pin_kein_sha "die:ist kein SHA-256" c_pu "$A/pin_kaputt" "$A/U_gut.py"
    k D7b_urteil_pin_63_ziffern "die:ist kein SHA-256" c_pu "$A/pin_63" "$A/U_gut.py"
    k D8_urteil_fehlt "die:Gate-Urteil fehlt" c_pu "$A/pin_U_gut" "$W/gibt_es_nicht.py"
    c_D9() { GATE_URTEIL_PIN_DATEI="$A/pin_U_gut"; PY=false; gate_urteil_pin_pruefen "$A/U_gut.py"; }
    k D9_urteil_nicht_lesbar "die:Gate-Urteil nicht lesbar" c_D9
    k D10_urteil_nicht_festgehalten "die:Gate-Urteil ist NICHT das festgehaltene" c_pu "$A/pin_U_gut" "$A/U_r1.py"
    # ---- Selbsttest: kein Temp-Platz; der Zwischenspeicher gilt nur fuer GENAU die gepruefte Kopie
    c_D11() { mit gut G0; TMPDIR="$W/gibt_es_nicht"; gate_urteil_selbsttest "$A/U_gut.py"; }
    k D11_selbsttest_kein_temp "die:gate_urteil_selbsttest: kein Temp-Platz" c_D11
    c_D11b() { mit gut G0; gate_urteil_selbsttest "$A/U_gut.py"; GATE_URTEIL_PIN_DATEI="$A/pin_U_r1"; gate_urteil_selbsttest "$A/U_r1.py"; }
    k D11b_zwischenspeicher_nur_gleiche_kopie "die:$MR 1" c_D11b
    # ---- gate_festhalten: Ordner, schon vorhandene Ziele, Quellen, Pins der Quellen, Selbsttest
    k D12_festhalten_ordner_fehlt "die:gate_festhalten: Ordner fehlt" fh x 'rmdir "$d"'
    k D13_festhalten_gate_ziel_da "die:apk_asset_gate.py existiert schon" fh a ': > "$d/apk_asset_gate.py"'
    k D14_festhalten_urteil_ziel_da "die:gate_urteil.py existiert schon" fh b ': > "$d/gate_urteil.py"'
    k D15_festhalten_gate_quelle_fehlt "die:Asset-Gate nicht kopierbar" fh c 'GATE_QUELLE="$W/gibt_es_nicht.py"'
    k D16_festhalten_urteil_quelle_fehlt "die:Gate-Urteil nicht kopierbar" fh e 'GATE_URTEIL_QUELLE="$W/gibt_es_nicht.py"'
    k D20_festhalten_gate_nicht_gepinnt "die:Asset-Gate ist NICHT das festgehaltene" fh f 'GATE_QUELLE="$A/G1.py"'
    k D21_festhalten_urteil_nicht_gepinnt "die:Gate-Urteil ist NICHT das festgehaltene" fh g 'GATE_URTEIL_QUELLE="$A/U_r1.py"'
    k D22_festhalten_urteil_selbsttest_rot "die:$MR 1" fh h 'GATE_URTEIL_QUELLE="$A/U_r1.py"; GATE_URTEIL_PIN_DATEI="$A/pin_U_r1"'
    # ---- ohne gate_festhalten; gate_laufen ohne Temp-Platz
    c_D17() { GATE_URTEIL_KOPIE=""; gate_urteil selbsttest "$A/G0.txt" 0; }
    k D17_urteil_ohne_festhalten "die:gate_urteil: erst gate_festhalten" c_D17
    c_D18() { GATE_URTEIL_KOPIE=""; GATE_PIN_DATEI="$A/pin_G0"; gate_laufen selbsttest "$A/G0.py" --selbsttest; }
    k D18_laufen_ohne_festhalten "die:gate_laufen: erst gate_festhalten" c_D18
    c_D19() { mit gut G0; gate_urteil_selbsttest "$A/U_gut.py"; TMPDIR="$W/gibt_es_nicht"; gate_laufen selbsttest "$A/G0.py" --selbsttest; }
    k D19_laufen_kein_temp "die:gate_laufen: kein Temp-Platz" c_D19
    # ---- Pins VOR JEDEM Lauf: private Kopien nach gate_festhalten veraendert (Urteil: VOR dem Gate-Lauf erkannt)
    c_N20() { fh n20 :; echo "# veraendert" >> "$GATE_URTEIL_KOPIE"; gate_laufen selbsttest "$GATE_KOPIE" --selbsttest; }
    k N20_urteil_kopie_veraendert "die:Gate-Urteil ist NICHT das festgehaltene" c_N20
    k N20c_urteil_vor_dem_gatelauf_geprueft 1 grep -aq "Attrappe ==" "$W/N20_urteil_kopie_veraendert.txt"
    c_N20b() { fh n20b :; echo "# veraendert" >> "$GATE_URTEIL_KOPIE"; gate_urteil selbsttest "$A/G0.txt" 0; }
    k N20b_urteil_kopie_veraendert_gate_urteil "die:Gate-Urteil ist NICHT das festgehaltene" c_N20b
    c_N21() { fh n21 :; echo "# veraendert" >> "$GATE_KOPIE"; gate_laufen selbsttest "$GATE_KOPIE" --selbsttest; }
    k N21_gate_kopie_veraendert "die:Asset-Gate ist NICHT das festgehaltene" c_N21
    fertig
}

case "${1:-}" in
    anlegen) anlegen "$2" "$3" ;;
    pruefen) pruefen "$2" "$3" "$4" "${5:-}" ;;
    *) echo "Aufruf: urteil_kontrollen.sh anlegen <apk_pruefen.sh> <ordner> | pruefen <apk_pruefen.sh> <attrappen> <arbeit> [--schnell]"
       exit 9 ;;
esac
