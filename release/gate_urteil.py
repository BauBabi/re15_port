#!/bin/bash
''':' #
# Direktaufruf (./release/gate_urteil.py ...) laeuft zuerst als Bash-Skript - derselbe Kopf wie apk_asset_gate.py
# (Nachbesserung R2, Gegenpruefung echtlauf B1): '#!/usr/bin/env python3' startete unter Git-Bash den WindowsApps-
# Alias. Der Interpreter kommt aus release/python_finden.sh. Fuer Python ist dieser Block eine Zeichenkette ohne Wirkung.
. "$(dirname "$0")/python_finden.sh" || exit 2 #
exec "$PY" "$0" "$@" #
'''
# =============================================================================
# release/gate_urteil.py — Urteil ueber einen Lauf von release/apk_asset_gate.py (Runde 35 Spur N, 2026-10-03)
# =============================================================================
# WARUM eine eigene Datei: bis Runde 35 stand dieser Code als Heredoc in release/apk_pruefen.sh (gate_urteil) und wurde
# von NIEMANDEM geprueft. Gegenpruefung R4-2, Befund F-Y2 (analysis/befunde_runde34_android/pruefer_umgehung_r4_2.md
# Abschnitt 2): EIN Zeichen - `ende(1, "das Gate meldet ...` -> `ende(0, ...` - machte aus der richtigen
# APK-ASSET-GATE-ABWEICHUNG des echten, gepinnten Gates ein ANDROID-GATES-OK fuer ein falsches Tuerarchiv. Eine
# nachsichtige Regression im Urteil ist STILL: jeder gute Lauf bleibt gruen. Nutzer (Runde 35): "Kuenftige Aenderungen
# am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten." Deshalb:
#   * --selbsttest prueft das Urteil an FESTEN Ausgaben (gute Laeufe je Modus, je Regel ein Gegenbeispiel, je gelesenem
#     Muster Gegenbeispiele fuer jede Lockerung, je Vergleichsstelle Faelle von BEIDEN Seiten) UND an MUTANTEN der
#     Funktion urteil(). Mutiert werden (Umfang, Stand Nachbesserung 2 - Abnahme 1, M1/M2): Vergleiche (== und != durch
#     jeden anderen Vergleich, also auch die einseitigen Lockerungen != -> < / > und == -> <= / >=; < <-> <=, > <-> >=,
#     < <-> >, <= <-> >=; keine Ordnung auf Zeichenketten; in/not in, is/is not), and/or, not, if-Bedingungen
#     (True/False), ganze Zahlen (+-1, auch die Urteilszahlen 0/1/2 in ende()), weggelassene Ausdrucks-Aufrufe (auch die
#     Pruef-Anweisungen tuer_soll()), JEDE Zeichenkette ausserhalb der Meldungstexte (MARKE-Tabelle, Modusnamen,
#     "[FEHLER]", "ABBRUCH", "Traceback", "ok", "\r"/"\n" ...: + ein fremdes Zeichen) und jedes Regex-Muster mit den
#     Lockerungen R0-R6 (_regex_lockerungen: Text hinter dem Muster erlaubt, Rest ab Stueck k -> .*, Wort -> .*,
#     \d+ -> .*, \s+ -> \s*, Einzelzeichen weg, Alternative weg). Jeder Mutant muss von mindestens einem Fall erkannt
#     werden - sonst ist er als gleichwertig begruendet (AEQUIVALENT unten) oder der Selbsttest scheitert. Wer das
#     Urteil aendert und keinen Fall dazu schreibt, bekommt einen ueberlebenden Mutanten = SELBSTTEST-FEHLER.
#     Strukturregel (Nachbesserung 2, M2): kein Meldungstext ruft eine Funktion des Urteils (ende, genau_eine,
#     tuer_soll ...) - eine Pruefung im Meldungstext waere fuer die Mutanten unsichtbar; der Selbsttest scheitert sonst.
#     NICHT mutiert: die Meldungstexte (zweites Argument von ende() und genau_eine()), urteil_rufen(), main() und der
#     Selbsttest selbst; Aenderungen, die keiner dieser Operatoren abbildet (z.B. any -> all, Funktions- oder
#     Methodentausch, Zeichenklassen in Regex, Rechenoperatoren wie + -> -), faengt nur die Fallsammlung - gemessen
#     in analysis/befunde_runde35/N_android_belege/nb1_urteil_aenderungen_nachher.txt und nb2_urteil_aenderungen_nachher.txt.
#     main()/urteil_rufen() und das bash-Urteil in apk_pruefen.sh pruefen die Negativ-Kontrollen von ctest
#     unit_r35_android_pruefkette.
#   * release/apk_pruefen.sh haelt diese Datei wie das Gate fest (Pin release/gate_urteil.sha256, private Kopie),
#     laesst den Selbsttest vor JEDER Nutzung laufen und prueft dessen Schlusszeile in bash (Mindestzahlen dort), und
#     verlangt fuer ein OK-Urteil zusaetzlich - unabhaengig von diesem Code - Rueckgabe 0 des Gates (gate_laufen).
#   * ctest unit_r35_android_pruefkette laesst den Selbsttest und Negativ-Kontrollen mit dem echten Gate bei jedem
#     Suite-Lauf laufen (Nachbesserung 1: auch je Pruefzeile des bash-Urteils eine Kontrolle, N10-N21).
#
# AUFRUF
#   gate_urteil.py <modus> <ausgabe-datei> <rueckgabe> <min_faelle> <min_innen> [<apk_eintraege>]
#       modus = selbsttest | apk | quellbaum | paket. Rueckgabe 0 = OK, 1 = das Gate meldet Befunde (Ausgabe stimmig),
#       2 = keine Aussage. Druckt genau eine Zeile "   Gate-Urteil (<modus>, Rueckgabe <rc>): <grund>".
#   gate_urteil.py --selbsttest
#       Schlusszeile "== URTEIL-SELBSTTEST-OK: f/f Faelle, k/m Mutanten erkannt, a als gleichwertig begruendet ==",
#       Rueckgabe 0; sonst "== URTEIL-SELBSTTEST-FEHLER: ... ==", Rueckgabe 1.
# Nur Python-Standardbibliothek (>= 3.8: ast.get_source_segment).
# =============================================================================
import ast
import functools
import re
import sys


class _Urteil(Exception):
    def __init__(self, code, grund):
        Exception.__init__(self, grund)
        self.code, self.grund = code, grund


# ---------------------------------------------------------------------------------------------------------------------
# DAS URTEIL (bis Runde 35 wortgleich der Heredoc in apk_pruefen.sh gate_urteil; nur ende() wirft statt sys.exit)
def urteil(modus, text, rc, min_faelle, min_innen, apk_eintraege):
    def ende(code, grund):
        raise _Urteil(code, grund)

    MARKE = {"selbsttest": "SELBSTTEST", "apk": "APK-ASSET-GATE", "quellbaum": "APK-ASSET-GATE-QUELLBAUM",
             "paket": "APK-ASSET-GATE-PAKET"}.get(modus)
    if MARKE is None:                             # Heredoc: KeyError -> Traceback -> Rueckgabe 1 (!); jetzt: keine Aussage
        ende(2, "unbekannter Modus %r" % (modus,))
    text = text.replace("\r", "")
    zeilen = [z for z in text.split("\n") if z.strip()]

    def genau_eine(muster, name):
        t = [m for m in (re.fullmatch(muster, z) for z in zeilen) if m]
        if len(t) != 1:
            ende(2, "Zeile '%s' %d-mal statt genau einmal - keine Aussage" % (name, len(t)))
        return [int(g) for g in t[0].groups()]

    def tuer_soll():
        # Nachbesserung 2 (Abnahme 1, M2): eine EIGENE Anweisung je Modus (apk/quellbaum/paket), nicht mehr im Meldungstext
        # von ende(0, ...) - dort liess sie sich ohne Signal entfernen (Meldungstexte werden nicht mutiert).
        t = [m for m in (re.fullmatch(r"   Tuer-Soll: .* (\d+)/(\d+) wie die Engine-Tabelle .*", z) for z in zeilen) if m]
        # Nachbesserung 1: als Zahlen vergleichen (das Gate druckt "%d/%d", apk_asset_gate.py _tuer_zeilen_drucken) -
        # so endet ein gelockertes Muster (\d+ -> .*) bei einer Nicht-Zahl in einer Ausnahme statt still gleich
        if not t or any(int(m.group(1)) != int(m.group(2)) or int(m.group(2)) == 0 for m in t):
            ende(2, "Tuer-Soll-Zeilen fehlen oder melden nicht g/g - keine Aussage")

    if not zeilen:
        ende(2, "KEINE Ausgabe - das Gate lief nicht (leere oder abgeschnittene Datei? falscher Interpreter?)")
    letzte = zeilen[-1]
    m_ok = re.fullmatch(r"== %s-OK: (.+) ==" % re.escape(MARKE), letzte)
    m_neg = re.fullmatch(r"== %s-(FEHLER|ABWEICHUNG): (.+) ==" % re.escape(MARKE), letzte)
    if m_neg:
        if rc == 1:
            ende(1, "das Gate meldet %s - Schlusszeile und Rueckgabe stimmen ueberein" % m_neg.group(1))
        ende(2, "Schlusszeile meldet %s, aber Rueckgabe %d statt 1 - Ausgangspfad des Gates defekt" % (m_neg.group(1), rc))
    if not m_ok:
        ende(2, "letzte Zeile ist nicht '== %s-OK: ... ==', sondern %r - keine Aussage" % (MARKE, letzte[:140]))
    if rc != 0:
        ende(2, "Schlusszeile OK, aber Rueckgabe %d - Ausgangspfad des Gates defekt" % rc)
    urteile = [z for z in zeilen if re.match(r"== (SELBSTTEST|APK-ASSET-GATE(-PAKET|-QUELLBAUM)?)-(OK|FEHLER|ABWEICHUNG)\b", z)]
    if len(urteile) != 1:
        ende(2, "%d Urteilszeilen statt genau einer - keine Aussage" % len(urteile))
    for z in zeilen:
        if z.startswith("ABBRUCH") or z.startswith("Traceback") or "[FEHLER]" in z:
            ende(2, "OK-Schlusszeile, aber im selben Lauf %r - keine Aussage" % z[:140])
    rest = m_ok.group(1)
    if modus == "selbsttest":
        m = re.fullmatch(r"(\d+)/(\d+) Faelle \(.*\)", rest)
        if not m:
            ende(2, "Schlusszeile ohne 'n/n Faelle' - keine Aussage")
        ok_n, n = int(m.group(1)), int(m.group(2))
        if ok_n != n or n < min_faelle:
            ende(2, "SELBSTTEST-OK meldet %d/%d Faelle, verlangt n/n mit n >= %d (release/apk_pruefen.sh "
                    "GATE_SELBSTTEST_MIN_FAELLE) - weniger Faelle = schwaecherer Selbsttest" % (ok_n, n, min_faelle))
        a, b = genau_eine(r"   Innere Proben: (\d+)/(\d+) \(.*\)", "Innere Proben: m/m")
        if a != b or b < min_innen:
            ende(2, "Innere Proben %d/%d, verlangt m/m mit m >= %d (GATE_SELBSTTEST_MIN_INNEN)" % (a, b, min_innen))
        faelle = [m for m in (re.fullmatch(r"   \[(ok|FEHLER)\] (\d+) (.*) rc=(-?\d+) \(soll (-?\d+)\)(.*)", z)
                              for z in zeilen) if m]
        if sorted(int(f.group(2)) for f in faelle) != list(range(1, n + 1)):
            ende(2, "%d Fallzeilen, Nummern nicht genau 1..%d - keine Aussage" % (len(faelle), n))
        for f in faelle:
            # Nachbesserung 1: rc/soll als Zahlen (das Gate druckt "rc=%d (soll %d)", apk_asset_gate.py selbsttest())
            if f.group(1) != "ok" or int(f.group(4)) != int(f.group(5)) or f.group(6).strip():
                ende(2, "Fallzeile %s nicht [ok] mit rc = soll: %r" % (f.group(2), f.group(0).strip()[:140]))
        ende(0, "SELBSTTEST-OK %d/%d, jede Fallzeile [ok] mit rc = soll, innere Proben %d/%d (Mindestzahlen %d/%d)"
             % (n, n, a, b, min_faelle, min_innen))
    if modus == "apk":
        m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen bytegleich, Tuer-Soll erfuellt, Manifest stimmt, "
                         r"ZIP-Struktur wie Android sie liest", rest)
        if not m:
            ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
        n = int(m.group(1))
        q, a, g, b = genau_eine(r"   SUMME\s+(\d+)\s+(\d+)\s+(\d+)\s+(\d+)", "SUMME")
        mz, mb = genau_eine(r"   Manifest:\s+(\d+) Zeilen / (\d+) Bytes \(.*\)", "Manifest: n Zeilen")
        if n <= 0 or not (q == a == g == mz == n) or mb != b:
            ende(2, "Zaehlung passt nicht zur Schlusszeile (%d): Quelle %d, APK %d, gleich %d, Manifest %d Zeilen / %d B, "
                    "Bytes gleich %d" % (n, q, a, g, mz, mb, b))
        for label in ("RE2/DOOR", "RE15DOOR"):
            t = genau_eine(r"   %s:\s+Quelle (\d+), APK (\d+), sha256 gleich (\d+)/(\d+)" % re.escape(label), label)
            if not (t[0] == t[1] == t[2] == t[3] > 0):
                ende(2, "%s: Quelle/APK/gleich %r nicht gleich bzw. 0" % (label, t))
        genau_eine(r"   TORSE\.VBS: Quelle (\d+) B, APK (\d+) B, sha256 gleich", "TORSE.VBS ... sha256 gleich")
        if apk_eintraege:
            if int(apk_eintraege) != a + 1:
                ende(2, "unzip zaehlt %s Eintraege unter assets/, das Gate %d Asset-Dateien + Manifest" % (apk_eintraege, a))
        tuer_soll()
        ende(0, "APK-ASSET-GATE-OK: %d Dateien, Quelle = APK%s = gleich = Manifestzeilen, RE2/DOOR + RE15DOOR + TORSE.VBS "
                "gleich, %d Tuer-Soll-Zeilen g/g" % (n, " = unzip-Zaehlung - 1" if apk_eintraege else "",
                                                    sum(z.startswith("   Tuer-Soll:") for z in zeilen)))
    if modus == "quellbaum":
        m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen, Tuer-Soll erfuellt", rest)
        if not m:
            ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
        n, k = int(m.group(1)), int(m.group(2))
        baum = [int(x.group(1)) for x in (re.fullmatch(r"   \S+\s+(\d+) Dateien", z) for z in zeilen) if x]
        if n <= 0 or len(baum) != k or sum(baum) != n or 0 in baum:
            ende(2, "Baumzeilen %r passen nicht zu %d Dateien in %d Baeumen" % (baum, n, k))
        tuer_soll()
        ende(0, "APK-ASSET-GATE-QUELLBAUM-OK: %d Dateien in %d Baeumen (Summe der Baumzeilen), %d Tuer-Soll-Zeilen g/g"
             % (n, k, sum(z.startswith("   Tuer-Soll:") for z in zeilen)))
    if modus == "paket":
        m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen bytegleich, nichts zusaetzlich", rest)
        if not m:
            ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
        n, k = int(m.group(1)), int(m.group(2))
        baum = [(int(x.group(1)), int(x.group(2))) for x in (re.fullmatch(r"   \S+\s+(\d+)\s+(\d+)", z) for z in zeilen) if x]
        if n <= 0 or len(baum) != k or any(qq != gg or qq == 0 for qq, gg in baum) or sum(gg for _qq, gg in baum) != n:
            ende(2, "Baumzeilen (Quelle, gleich) %r passen nicht zu %d Dateien in %d Baeumen" % (baum, n, k))
        tuer_soll()
        ende(0, "APK-ASSET-GATE-PAKET-OK: %d Dateien in %d Baeumen, je Baum Quelle = gleich, %d Tuer-Soll-Zeilen g/g"
             % (n, k, sum(z.startswith("   Tuer-Soll:") for z in zeilen)))
    # hierher kommt kein bekannter Modus (jeder Zweig endet mit ende()); urteil_rufen wertet "ohne Ergebnis" als 2


def urteil_rufen(fn, modus, text, rc, min_faelle=5, min_innen=3, apk_eintraege="", art=None):
    """-> (code, grund). Jede andere Ausnahme als _Urteil und ein Ende ohne ende() = 2 (fail closed). art (Liste, optional)
    bekommt "ende" / "ausnahme" / "ohne" angehaengt - der Selbsttest wertet auch das: ein Mutant, der statt einer
    begruendeten Entscheidung abstuerzt, ist ERKANNT, auch wenn die Zahl 2 zufaellig dieselbe ist."""
    try:
        fn(modus, text, rc, min_faelle, min_innen, apk_eintraege)
    except _Urteil as u:
        if art is not None:
            art.append("ende")
        return u.code, u.grund
    except Exception as ex:                       # noqa: BLE001 - absichtlich breit: kein Urteil = keine Aussage
        if art is not None:
            art.append("ausnahme")
        return 2, "Ausnahme im Urteil: %r" % (ex,)
    if art is not None:
        art.append("ohne")
    return 2, "Urteil ohne Ergebnis (ende() nie erreicht)"


# ---------------------------------------------------------------------------------------------------------------------
# SELBSTTEST: feste Ausgaben (Muster nach den print-Zeilen von apk_asset_gate.py: selbsttest() :3353ff, pruefen(),
# nur_quellbaum(), paket_pruefen(); echte Laeufe des Gates prueft zusaetzlich ctest unit_r35_android_pruefkette)
TUER = ["   Tuer-Soll: Port-Tuerarchiv re15_port/shared_assets/RE15DOOR   30/30 wie die Engine-Tabelle (Groesse, Aufbau, FNV-1a)",
        "   Tuer-Soll: RE2-Tuerarchiv  re15_port/shared_assets/RE2/DOOR   27/27 wie die Engine-Tabelle (Groesse, Aufbau)"]


def _selbsttest_log(n=5, innen=3):
    z = ["== APK-Asset-Gate: Selbsttest (Mini-Quellbaum + Mini-APK im Temp-Ordner) ==",
         "   Innere Proben: %d/%d (_ant_passt, manifest_lesen v2)" % (innen, innen)]
    for i in range(1, n + 1):
        soll = 0 if i == 1 else 1
        z.append("   [ok] %02d Fall %d%s rc=%d (soll %d)" % (i, i, " " * 20, soll, soll))
    z += ["   Laufzeit: 13.0 s", "== SELBSTTEST-OK: %d/%d Faelle (gute APKs angenommen, jede Faelschung abgelehnt) ==" % (n, n)]
    return z


def _apk_log(n=12, b=4096):
    return _apk_log_z(n, n, n, b, n, b, n)


def _apk_log_z(q, a, g, b, mz, mb, n):
    """apk-Ausgabe mit einzeln waehlbaren Zahlen (Nachbesserung 2): SUMME Quelle q / APK a / gleich g / Bytes b,
    Manifest mz Zeilen / mb Bytes, Schlusszeile n Dateien. _apk_log(n, b) = _apk_log_z(n, n, n, b, n, b, n)."""
    return ["== APK-Asset-Gate: pruefe x.apk gegen den Quellbaum ==",
            "   %-28s %7s %7s %8s %12s" % ("Baum", "Quelle", "APK", "gleich", "Bytes"),
            "   %-28s %7d %7d %8d %12d" % ("shared_assets/PSX", q, a, g, b),
            "   SUMME %28d %7d %8d %12d" % (q, a, g, b),
            "   Manifest:   %d Zeilen / %d Bytes (Format v2, sha256 je Datei)" % (mz, mb),
            "   RE2/DOOR:  Quelle 27, APK 27, sha256 gleich 27/27",
            "   RE15DOOR:  Quelle 30, APK 30, sha256 gleich 30/30",
            "   TORSE.VBS: Quelle 1234 B, APK 1234 B, sha256 gleich"] + TUER + [
            "== APK-ASSET-GATE-OK: %d Dateien in 1 Baeumen bytegleich, Tuer-Soll erfuellt, Manifest stimmt, "
            "ZIP-Struktur wie Android sie liest ==" % n]


def _faelle_seiten(f, S, A, Q, P):
    """Nachbesserung 2 (Abnahme 1, M1/M2): jede Vergleichsstelle des Urteils von BEIDEN Seiten (kleiner UND groesser) -
    je Fall genau EINE Bedingung verletzt, alle anderen erfuellt. Vorher pruefte die Fallsammlung jede Ungleichung nur
    von einer Seite (Summe nur < n, qq nur > gg, ok_n nur < n, g1 nur < g2, mb nur > b, len nur < k), und eine einseitige
    Lockerung (!= -> < oder >, == -> <= oder >=) blieb unbemerkt. Soll 2 = keine Aussage."""
    # ---- selbsttest: ok_n > n (die andere Seite "6/7 Faelle" steht in _faelle), a > b mit b = Mindestzahl ("3/2" in
    # _faelle verletzt zugleich die Mindestzahl und isoliert a > b nicht)
    f("st/S: 6/5 Faelle bei 5 Fallzeilen (ok_n > n)", "selbsttest", _ersetze(S, "5/5 Faelle", "6/5 Faelle"), 0, 2)
    f("st/S: Innere Proben 4/3 (a > b, b = Mindestzahl)", "selbsttest",
      _ersetze(S, "Innere Proben: 3/3", "Innere Proben: 4/3"), 0, 2)
    # ---- Rueckgabe des Gates unter 0: main() nimmt jede ganze Zahl, OK ist nur genau 0, ein Befund nur genau 1
    for modus, L in (("selbsttest", S), ("apk", A), ("quellbaum", Q), ("paket", P)):
        f("%s gut, Rueckgabe -1" % modus, modus, "\n".join(L), -1, 2)
    f("apk FEHLER, Rueckgabe -1", "apk", "   x\n== APK-ASSET-GATE-FEHLER: 1 Befunde ==", -1, 2)
    # ---- apk: Kette q == a == g == mz == n, je Glied beide Seiten (alle Werte links vom Glied 12 -+ 1, rechts 12)
    for k, glied in enumerate(("Quelle/APK", "APK/gleich", "gleich/Manifest", "Manifest/Schluss")):
        for d in (-1, 1):
            w = [12 + d if i <= k else 12 for i in range(5)]
            f("apk/S: Kette %s %s (%s)" % (glied, "<" if d < 0 else ">", "/".join(map(str, w))), "apk",
              "\n".join(_apk_log_z(w[0], w[1], w[2], 4096, w[3], 4096, w[4])), 0, 2)
    # ---- apk: Manifest-Bytes kleiner UND groesser als die SUMME-Bytes ("SUMME Bytes 4095" in _faelle = mb > b)
    f("apk/S: Manifest 4095 Bytes < SUMME 4096", "apk", "\n".join(_apk_log_z(12, 12, 12, 4096, 12, 4095, 12)), 0, 2)
    f("apk/S: Manifest 4097 Bytes > SUMME 4096", "apk", "\n".join(_apk_log_z(12, 12, 12, 4096, 12, 4097, 12)), 0, 2)
    # ---- apk: Tuerarchiv-Kette Quelle == APK == gleich == von > 0, je Glied beide Seiten (RE2/DOOR, links 27 -+ 1)
    z = "Quelle 27, APK 27, sha256 gleich 27/27"
    for k, glied in enumerate(("Quelle/APK", "APK/gleich", "gleich/von")):
        for d in (-1, 1):
            w = tuple(27 + d if i <= k else 27 for i in range(4))
            f("apk/S: RE2/DOOR %s %s (%d/%d/%d/%d)" % ((glied, "<" if d < 0 else ">") + w), "apk",
              _ersetze(A, z, "Quelle %d, APK %d, sha256 gleich %d/%d" % w), 0, 2)
    # ---- Tuer-Soll g1 > g2 und - als EIGENE Pruefung (M2) - in quellbaum und paket mit erhaltener Schlusszeile:
    # 29/30, 31/30, 0/0 (fehlende Tuer-Soll-Zeilen: "... Tuer-Soll fehlt" in _faelle, nur die Tuer-Soll-Zeilen entfernt)
    f("apk/S: Tuer-Soll 31/30 (g1 > g2)", "apk", _ersetze(A, "30/30 wie", "31/30 wie"), 0, 2)
    for kurz, modus, L in (("qb", "quellbaum", Q), ("pk", "paket", P)):
        f("%s/S: Tuer-Soll 29/30" % kurz, modus, _ersetze(L, "30/30 wie", "29/30 wie"), 0, 2)
        f("%s/S: Tuer-Soll 31/30" % kurz, modus, _ersetze(L, "30/30 wie", "31/30 wie"), 0, 2)
        f("%s/S: Tuer-Soll 0/0" % kurz, modus, _ersetze(L, "27/27 wie", "0/0 wie"), 0, 2)
    # ---- quellbaum: Anzahl Baumzeilen groesser UND kleiner als k bei passender Summe; Summe groesser als n
    f("qb/S: 3 Baumzeilen bei 'in 2 Baeumen', Summe passt", "quellbaum", "\n".join(
        ["   shared_assets/PSX   10 Dateien", "   synchro   1 Dateien", "   RE2   1 Dateien"] + TUER + [Q[-1]]), 0, 2)
    f("qb/S: 2 Baumzeilen bei 'in 3 Baeumen', Summe passt", "quellbaum", _ersetze(Q, "in 2 Baeumen", "in 3 Baeumen"), 0, 2)
    f("qb/S: Summe der Baumzeilen 13 > 12", "quellbaum", _ersetze(Q, "synchro   2 Dateien", "synchro   3 Dateien"), 0, 2)
    # ---- paket: dasselbe, dazu Quelle < gleich bei passender Summe "gleich" ("11 != 10" in _faelle = Quelle > gleich)
    f("pk/S: 3 Baumzeilen bei 'in 2 Baeumen', Summe passt", "paket", "\n".join(
        ["   shared_assets/PSX   10   10", "   synchro   1   1", "   RE2   1   1"] + TUER + [P[-1]]), 0, 2)
    f("pk/S: 2 Baumzeilen bei 'in 3 Baeumen', Summe passt", "paket", _ersetze(P, "in 2 Baeumen", "in 3 Baeumen"), 0, 2)
    f("pk/S: Quelle 9 < gleich 10, Summe gleich passt", "paket", _ersetze(P, "PSX   10   10", "PSX   9   10"), 0, 2)
    f("pk/S: Summe gleich 13 > 12", "paket", _ersetze(P, "synchro   2   2", "synchro   3   3"), 0, 2)


def _faelle_nb3(f, S, A, Q, P):
    """Nachbesserung 3 (Abnahme 2, M2/M3): die drei Regeln, die sich mit EINER Zeile entfernen liessen (H10, H17, H18 der
    Abnahme), je mit dem fehlenden Fall - zusaetzlich zu den Stoerungsfaellen, die dieselben Klassen allgemein abdecken."""
    # H10: "genau n Fallzeilen" - bisher nur Fallnummern mit Luecke/doppelt/6 statt 5; jetzt lueckenlos zu WENIG bzw. zu VIEL
    f("st/N3: letzte Fallzeile fehlt (01..04 lueckenlos, 5/5)", "selbsttest",
      "\n".join(z for z in S if not z.startswith("   [ok] 05")), 0, 2)
    f("st/N3: Fallzeile zuviel (01..06 lueckenlos, 5/5)", "selbsttest",
      _ersetze(_selbsttest_log(6, 3), "6/6 Faelle", "5/5 Faelle"), 0, 2)
    # H17: "je Baum Quelle = gleich" - bisher nur EIN Baum abweichend; jetzt zwei gegenlaeufig, beide Summen gleich (12/12)
    f("pk/N3: Quelle != gleich in 2 Baeumen, Summen gleich", "paket",
      _ersetze(_ersetze(P, "PSX   10   10", "PSX   9   10").split("\n"), "synchro   2   2", "synchro   3   2"), 0, 2)
    # H18: Form der Baumzeile - genau zwei Zahlen (paket) bzw. genau eine (quellbaum); eine Zusatzspalte ist keine Baumzeile
    f("pk/N3: Baumzeile mit drei Zahlen (10 9 10)", "paket", _ersetze(P, "PSX   10   10", "PSX   10 9 10"), 0, 2)
    f("qb/N3: Baumzeile mit zwei Zahlen (10 9 Dateien)", "quellbaum", _ersetze(Q, "PSX   10 Dateien", "PSX   10 9 Dateien"), 0, 2)


# ---------------------------------------------------------------------------------------------------------------------
# STOERUNGSFAELLE (Nachbesserung 3, Abnahme 2 M2/M3): aus jeder guten Ausgabe werden Faelle ERZEUGT - jede Zeile weg, jede
# Zeile doppelt, jede Zahl -1/+1, hinter jeder Zahl dieselbe Zahl noch einmal (Zusatzspalte), je zwei Zahlen derselben
# Spalte in zwei Zeilen mit gleich vielen Zahlen gegenlaeufig (-1/+1 und +1/-1; nur Vertauschungen zweier Werte einer
# Spalte entstehen nicht - die Reihenfolge ist keine Aussage des Gates). Soll nach REGEL: 2 (keine Aussage) - es sei denn,
# die Stelle steht hier begruendet als UNGEPRUEFT (Soll 0: das Urteil liest sie absichtlich nicht). Das Soll wird NIE aus
# dem Urteil berechnet (das bestaetigte jede Aenderung selbst). Wer eine Regel entfernt, macht eine Stoerung zu "Urteil 0
# (soll 2)" = SELBSTTEST-FEHLER - so faellt die ganze Klasse der Abnahme 2 (H10 = Zeile weg, H17 = gegenlaeufiges Paar,
# H18 = Zusatzspalte) und jede kuenftige Aenderung, die eine Zahl oder Zeile der Ausgabe nicht mehr prueft. Wer eine
# Stelle als ungeprueft erklaert, schreibt sie hier mit Grund hin; die Zahl der so begruendeten Stoerungen steht in der
# Schlusszeile, und release/apk_pruefen.sh begrenzt sie (GATE_URTEIL_MAX_UNGEPRUEFT) - sichtbar im Diff.
# Eintrag: (modi ("*" = alle), Zeilenmuster (re.match auf die ORIGINAL-Zeile), Arten, Spalten (None = alle Zahlen der
# Zeile; Index der Zahl in der Zeile), Grund). Ein Paar gilt als ungeprueft, wenn beide Zahlen einzeln als "zahl"
# ungeprueft sind ODER ein Eintrag mit "paar" beide Zeilen deckt. Jeder Eintrag muss mindestens eine Stoerung decken.
UNGEPRUEFT = [
    ("*", r"== APK-Asset-Gate: ", {"weg", "doppelt"}, None,
     "Kopfzeile des Gates: sie traegt keine Aussage, das Urteil liest die Schlusszeile und die Zaehlzeilen"),
    (("apk",), r"   Baum ", {"weg", "doppelt"}, None, "Spaltenkopf der Baumtabelle: keine Zahl, keine Aussage"),
    (("apk",), r"   shared_assets/PSX ", {"weg", "doppelt", "zahl", "zusatz"}, None,
     "Modus apk: das Urteil wertet die SUMME-Zeile (Quelle = APK = gleich = Manifest = Schlusszeile), nicht ihre Summanden"),
    (("apk",), r"== APK-ASSET-GATE-OK: ", {"zahl"}, {1},
     "Modus apk: die Baumzahl 'in k Baeumen' liest das Urteil nicht - die SUMME-Zeile traegt die Zaehlung"),
    (("apk",), r"   TORSE\.VBS: ", {"zahl"}, None,
     "TORSE.VBS: das Urteil verlangt genau eine Zeile mit 'sha256 gleich'; die Bytezahlen vergleicht es nicht"),
    (("apk", "quellbaum", "paket"), r"   Tuer-Soll: ", {"weg", "doppelt", "zusatz"}, None,
     "Tuer-Soll: das Urteil verlangt mindestens eine erkannte Zeile und jede erkannte g/g; die Anzahl der Zeilen und eine "
     "unlesbare Zeile neben einer erkannten prueft es nicht (eine Zusatzzahl vor g/g faellt in '.*' des Musters)"),
    (("selbsttest",), r"   \[ok\] \d+ Fall ", {"zahl"}, {1}, "Fallbeschreibung: freier Text des Gates ((.*) im Muster)"),
    (("selbsttest",), r"   \[ok\] \d+ Fall ", {"zusatz"}, {0, 1},
     "eine Zusatzzahl hinter der Fallnummer bzw. der Beschreibungszahl gehoert zur Beschreibung (freier Text)"),
    (("selbsttest",), r"   Laufzeit: ", {"weg", "doppelt", "zahl", "zusatz"}, None, "Laufzeit: nur Information"),
    (("quellbaum",), r"   \S+\s+\d+ Dateien$", {"paar"}, None,
     "Modus quellbaum: je Baum gibt es keinen Vergleichswert (anders als paket: Quelle/gleich) - nur die Summe"),
]
_ZAHL = re.compile(r"(?<![A-Za-z0-9_])\d+(?![A-Za-z0-9_])")
_UNGEPRUEFT_GENUTZT = set()


def _ungeprueft(modus, zeile, art, spalte):
    """-> Index des ersten passenden UNGEPRUEFT-Eintrags oder None"""
    for i, (modi, muster, arten, spalten, _grund) in enumerate(UNGEPRUEFT):
        if (modi == "*" or modus in modi) and art in arten and re.match(muster, zeile) \
                and (spalten is None or spalte in spalten):
            return i
    return None


def _stoerungen(modus, L):
    """-> Liste (titel, text, soll) aller Stoerungen der guten Ausgabe L (Liste von Zeilen) - siehe UNGEPRUEFT."""
    out = []
    zahlen = [[(m.start(), m.end(), m.group(0)) for m in _ZAHL.finditer(z)] for z in L]
    kurz = {"selbsttest": "st", "apk": "apk", "quellbaum": "qb", "paket": "pk"}[modus]

    def dazu(titel, zeilen, deckung):
        if all(d is not None for d in deckung):
            _UNGEPRUEFT_GENUTZT.update(deckung)
            out.append(("%s/St: %s" % (kurz, titel), "\n".join(zeilen), 0))
        else:
            out.append(("%s/St: %s" % (kurz, titel), "\n".join(zeilen), 2))

    def neu_wert(v, d):
        w = int(v) + d
        return "%0*d" % (len(v), w) if v.startswith("0") and len(v) > 1 and w >= 0 else str(w)

    def ersetzt(z, tok, wert):
        a, e, _v = tok
        return z[:a] + wert + z[e:]

    for i, z in enumerate(L):
        kopf = "Z%d %r" % (i, z.strip()[:14])
        dazu(kopf + " weg", L[:i] + L[i + 1:], [_ungeprueft(modus, z, "weg", None)])
        dazu(kopf + " doppelt", L[:i + 1] + L[i:], [_ungeprueft(modus, z, "doppelt", None)])
        for c, tok in enumerate(zahlen[i]):
            for d in (-1, 1):
                dazu("%s Zahl %d %+d" % (kopf, c, d), L[:i] + [ersetzt(z, tok, neu_wert(tok[2], d))] + L[i + 1:],
                     [_ungeprueft(modus, z, "zahl", c)])
            dazu("%s Zahl %d Zusatz" % (kopf, c), L[:i] + [z[:tok[1]] + " " + tok[2] + z[tok[1]:]] + L[i + 1:],
                 [_ungeprueft(modus, z, "zusatz", c)])
    for i in range(len(L)):
        for j in range(i + 1, len(L)):
            if not zahlen[i] or len(zahlen[i]) != len(zahlen[j]):
                continue
            for c in range(len(zahlen[i])):
                vi, vj = int(zahlen[i][c][2]), int(zahlen[j][c][2])
                for d in (-1, 1):
                    if sorted((vi + d, vj - d)) == sorted((vi, vj)):
                        continue                  # nur eine Vertauschung zweier Werte der Spalte - keine Aussage
                    zl = list(L)
                    zl[i] = ersetzt(L[i], zahlen[i][c], neu_wert(zahlen[i][c][2], d))
                    zl[j] = ersetzt(L[j], zahlen[j][c], neu_wert(zahlen[j][c][2], -d))
                    paar = _ungeprueft(modus, L[i], "paar", c)
                    deck = [paar, _ungeprueft(modus, L[j], "paar", c)] if paar is not None else \
                        [_ungeprueft(modus, L[i], "zahl", c), _ungeprueft(modus, L[j], "zahl", c)]
                    dazu("Z%d/Z%d Spalte %d %+d/%+d" % (i, j, c, d, -d), zl, deck)
    return out


def _faelle_stoerung(f, S, A, Q, P):
    for modus, L in (("selbsttest", S), ("apk", A), ("quellbaum", Q), ("paket", P)):
        for titel, text, soll in _stoerungen(modus, L):
            f(titel, modus, text, 0, soll)


def _quellbaum_log():
    return ["== APK-Asset-Gate: nur Quellbaum ==", "   shared_assets/PSX   10 Dateien", "   synchro   2 Dateien"] + TUER + [
            "== APK-ASSET-GATE-QUELLBAUM-OK: 12 Dateien in 2 Baeumen, Tuer-Soll erfuellt =="]


def _paket_log():
    return ["== APK-Asset-Gate: PC-Paket ==", "   shared_assets/PSX   10   10", "   synchro   2   2"] + TUER + [
            "== APK-ASSET-GATE-PAKET-OK: 12 Dateien in 2 Baeumen bytegleich, nichts zusaetzlich =="]


def _mit(zeilen, anfang, neu):
    """die Zeile, die mit anfang beginnt, durch neu ersetzen (genau eine)"""
    assert sum(z.startswith(anfang) for z in zeilen) == 1, anfang
    return "\n".join(neu if z.startswith(anfang) else z for z in zeilen)


def _ersetze(zeilen, alt, neu, n=1):
    t = "\n".join(zeilen)
    assert t.count(alt) >= 1, alt
    return t.replace(alt, neu, n)


def _in_zeile(zeilen, anfang, alt, neu, alle=False):
    """in der Zeile, die mit anfang beginnt (alle=True: in JEDER solchen Zeile), das erste alt durch neu ersetzen"""
    out, n = [], 0
    for z in zeilen:
        if z.startswith(anfang) and alt in z and (alle or n == 0):
            z, n = z.replace(alt, neu, 1), n + 1
        out.append(z)
    assert n >= 1 and (alle or sum(z.startswith(anfang) for z in zeilen) == 1), (anfang, alt)
    return "\n".join(out)


def _wort_in_zeile(zeilen, anfang, wort, alle=False):
    """in der Zeile/den Zeilen mit anfang das erste ganze Wort wort durch 'XX' ersetzen (hinter dem Anfang)"""
    muster = r"(?<![A-Za-z0-9_])%s(?![A-Za-z0-9_])" % re.escape(wort)
    out, n = [], 0
    for z in zeilen:
        if z.startswith(anfang) and (alle or n == 0) and re.search(muster, z[len(anfang):]):
            z, n = anfang + re.sub(muster, "XX", z[len(anfang):], count=1), n + 1
        out.append(z)
    assert n >= 1, (anfang, wort)
    return "\n".join(out)


def _faelle_muster(f, S, A, Q, P):
    """Nachbesserung 1 (Abnahme 0, M1): Gegenbeispiele zu jedem Muster, das das Urteil liest - jedes mit festem Soll.
    Je gelesene Zeile: (a) letztes Zeichen weg bzw. falscher Schluss (das Muster verlangt die GANZE Zeile - faengt jede
    Lockerung "Rest -> .*", z.B. A5/A6/A11 der Abnahme), (b) je Literal-Wort ein anderes Wort, (c) je Zahlfeld 'x',
    (d) je Zwischenraum-Pflicht die Zeile ohne Zwischenraum. Soll 2 = keine Aussage, wo nichts anderes steht."""
    # ---- selbsttest: Schlusszeile, Innere Proben, Fallzeilen
    SCHLUSS_S = "== SELBSTTEST-OK:"
    f("st/M: Schluss ohne Klammerteil (A5)", "selbsttest", _in_zeile(S, SCHLUSS_S, " (gute APKs angenommen, jede Faelschung "
                                                                     "abgelehnt) ==", " =="), 0, 2)
    f("st/M: Schluss, Klammer nicht geschlossen", "selbsttest", _in_zeile(S, SCHLUSS_S, "abgelehnt) ==", "abgelehnt =="), 0, 2)
    f("st/M: Schluss endet ' ==X'", "selbsttest", _in_zeile(S, SCHLUSS_S, ") ==", ") ==X"), 0, 2)
    f("st/M: Schluss endet ' =' (ein = fehlt)", "selbsttest", _in_zeile(S, SCHLUSS_S, ") ==", ") ="), 0, 2)
    f("st/M: Schluss '-OK-X:'", "selbsttest", _in_zeile(S, SCHLUSS_S, "-OK:", "-OK-X:"), 0, 2)
    f("st/M: Schluss x/5 Faelle", "selbsttest", _in_zeile(S, SCHLUSS_S, "5/5 Faelle", "x/5 Faelle"), 0, 2)
    f("st/M: Schluss 5/x Faelle", "selbsttest", _in_zeile(S, SCHLUSS_S, "5/5 Faelle", "5/x Faelle"), 0, 2)
    IP = "   Innere Proben:"
    f("st/M: Innere Proben, Klammer nicht geschlossen", "selbsttest", _in_zeile(S, IP, "v2)", "v2"), 0, 2)
    f("st/M: Innere Proben ohne Klammerteil", "selbsttest", _in_zeile(S, IP, " (_ant_passt, manifest_lesen v2)", ""), 0, 2)
    f("st/M: 'Aeussere Proben'", "selbsttest", _in_zeile(S, IP, "Innere", "Aeussere"), 0, 2)
    f("st/M: 'Innere Probe:'", "selbsttest", _in_zeile(S, IP, "Proben:", "Probe:"), 0, 2)
    f("st/M: Innere Proben x/3", "selbsttest", _in_zeile(S, IP, "3/3", "x/3"), 0, 2)
    f("st/M: Innere Proben 3/x", "selbsttest", _in_zeile(S, IP, "3/3", "3/x"), 0, 2)
    f("st/M: Fallzeile 'rx=' statt 'rc='", "selbsttest", _in_zeile(S, "   [ok] 02", "rc=", "rx="), 0, 2)
    f("st/M: Fallzeile '(sol 1)'", "selbsttest", _in_zeile(S, "   [ok] 02", "(soll", "(sol"), 0, 2)
    f("st/M: Fallzeile rc=x", "selbsttest", _in_zeile(S, "   [ok] 02", "rc=1", "rc=x"), 0, 2)
    f("st/M: Fallzeile (soll x)", "selbsttest", _in_zeile(S, "   [ok] 02", "(soll 1)", "(soll x)"), 0, 2)
    # ---- Befund-Schluss (gleiches Muster in jedem Modus): Text hinter ' ==' -> keine Befund-Zeile -> keine Aussage
    f("M: FEHLER-Schluss endet ' ==X', Rueckgabe 1", "apk", "   x\n== APK-ASSET-GATE-FEHLER: 1 Befunde ==X", 1, 2)
    f("M: 'FEHLERHAFT:'-Schluss, Rueckgabe 1", "apk", "   x\n== APK-ASSET-GATE-FEHLERHAFT: 1 Befunde ==", 1, 2)
    # ---- Urteilszeilen-Zaehlung: nur MARKE-OK/-FEHLER/-ABWEICHUNG als ganzes Wort ist eine Urteilszeile (\b)
    f("M: Zeile '== SELBSTTEST-OKAY: x ==' ist keine Urteilszeile", "selbsttest",
      "\n".join(S[:-1] + ["== SELBSTTEST-OKAY: x ==", S[-1]]), 0, 0)
    f("M: Zeile '== SELBSTTEST-HINWEIS: x ==' ist keine Urteilszeile", "selbsttest",
      "\n".join(S[:-1] + ["== SELBSTTEST-HINWEIS: x ==", S[-1]]), 0, 0)
    # ---- apk: Schlusszeile
    SCHLUSS_A = "== APK-ASSET-GATE-OK:"
    f("apk/M: Schluss 'sie lies' (letztes Zeichen weg)", "apk", _in_zeile(A, SCHLUSS_A, "liest ==", "lies =="), 0, 2)
    for w in ("Dateien", "in", "Baeumen", "bytegleich", "Tuer", "Soll", "erfuellt", "Manifest", "stimmt", "ZIP",
              "Struktur", "wie", "Android", "sie"):
        f("apk/M: Schluss Wort '%s' anders" % w, "apk", _wort_in_zeile(A, SCHLUSS_A + " ", w), 0, 2)
    f("apk/M: Schluss x Dateien", "apk", _in_zeile(A, SCHLUSS_A, "OK: 12 Dateien", "OK: x Dateien"), 0, 2)
    f("apk/M: Schluss in x Baeumen", "apk", _in_zeile(A, SCHLUSS_A, "in 1 Baeumen", "in x Baeumen"), 0, 2)
    # ---- apk: SUMME (1-Datei-Log fuer die Zwischenraum-Faelle: "11" teilt sich nur bei n = 1 wieder in 1 + 1)
    A1 = _apk_log(1, 7)
    for k in range(4):
        w = ["12", "12", "12", "4096"]
        w[k] = "x"
        f("apk/M: SUMME Feld %d = x" % (k + 1), "apk", _mit(A, "   SUMME", "   SUMME   %s   %s   %s   %s" % tuple(w)), 0, 2)
    f("apk/M: SUMME ohne Zwischenraum nach SUMME", "apk", _mit(A1, "   SUMME", "   SUMME1   1   1   7"), 0, 2)
    for k, z in enumerate(("   SUMME   11   1   7", "   SUMME   1   11   7", "   SUMME   1   1   17")):
        f("apk/M: SUMME ohne Zwischenraum %d" % (k + 2), "apk", _mit(A1, "   SUMME", z), 0, 2)
    # ---- apk: Manifest-Zeile
    MF = "   Manifest:"
    f("apk/M: Manifest, Klammer nicht geschlossen", "apk", _in_zeile(A, MF, "Datei)", "Datei"), 0, 2)
    f("apk/M: Manifest ohne Klammerteil", "apk", _in_zeile(A, MF, " (Format v2, sha256 je Datei)", ""), 0, 2)
    for w in ("Manifest", "Zeilen", "Bytes"):
        f("apk/M: Manifest Wort '%s' anders" % w, "apk", _in_zeile(A, MF, w, "XX"), 0, 2)
    f("apk/M: Manifest x Zeilen", "apk", _in_zeile(A, MF, "12 Zeilen", "x Zeilen"), 0, 2)
    f("apk/M: Manifest x Bytes", "apk", _in_zeile(A, MF, "4096 Bytes", "x Bytes"), 0, 2)
    f("apk/M: Manifest ohne Zwischenraum", "apk", _in_zeile(A, MF, "Manifest:   12", "Manifest:12"), 0, 2)
    # ---- apk: Tuerarchiv-Zeilen (dasselbe Muster fuer RE2/DOOR und RE15DOOR: eine reicht je Mutant)
    D = "   RE2/DOOR:"
    for w in ("Quelle", "APK", "sha256", "gleich"):
        f("apk/M: RE2/DOOR Wort '%s' anders" % w, "apk", _wort_in_zeile(A, D, w), 0, 2)
    for k, (alt, neu) in enumerate((("Quelle 27", "Quelle x"), ("APK 27", "APK x"), ("gleich 27/", "gleich x/"),
                                    ("/27", "/x"))):
        f("apk/M: RE2/DOOR Zahl %d = x" % (k + 1), "apk", _in_zeile(A, D, alt, neu), 0, 2)
    f("apk/M: RE2/DOOR ohne Zwischenraum", "apk", _in_zeile(A, D, "RE2/DOOR:  Quelle", "RE2/DOOR:Quelle"), 0, 2)
    # ---- apk: TORSE.VBS (Wortlaut des Gates bei Abweichung: apk_asset_gate.py pruefen() "sha256 NICHT gleich/...")
    T = "   TORSE.VBS:"
    f("apk/M: TORSE.VBS 'sha256 NICHT gleich/nicht geprueft' (A11)", "apk",
      _in_zeile(A, T, "sha256 gleich", "sha256 NICHT gleich/nicht geprueft"), 0, 2)
    f("apk/M: TORSE.VBS 'sha256 gleic' (letztes Zeichen weg)", "apk", _in_zeile(A, T, "gleich", "gleic"), 0, 2)
    f("apk/M: TORSE.VBS Wort 'TORSE' anders", "apk", _in_zeile(A, T, "TORSE", "XX"), 0, 2)
    for w in ("VBS", "Quelle", "APK", "sha256"):
        f("apk/M: TORSE.VBS Wort '%s' anders" % w, "apk", _wort_in_zeile(A, "   TORSE", w), 0, 2)
    f("apk/M: TORSE.VBS Quelle x B", "apk", _in_zeile(A, T, "Quelle 1234 B", "Quelle x B"), 0, 2)
    f("apk/M: TORSE.VBS APK x B", "apk", _in_zeile(A, T, "APK 1234 B", "APK x B"), 0, 2)
    # ---- Tuer-Soll (beide Zeilen gleich veraendert - eine einzelne fremde Zeile ignoriert das Urteil)
    TS = "   Tuer-Soll:"
    f("apk/M: Tuer-Soll 'Engine-Tabelle(' (Zwischenraum weg)", "apk", _in_zeile(A, TS, "Tabelle (", "Tabelle(", True), 0, 2)
    for w in ("Tuer", "Soll", "wie", "die", "Engine", "Tabelle"):
        f("apk/M: Tuer-Soll Wort '%s' anders" % w, "apk", _wort_in_zeile(A, TS if w not in ("Tuer", "Soll") else "   ", w,
                                                                       True), 0, 2)
    TP, TR = TS + " Port", TS + " RE2"
    f("apk/M: Tuer-Soll x/n", "apk", _in_zeile(_in_zeile(A, TP, "30/30", "x/30").split("\n"), TR, "27/27", "x/27"), 0, 2)
    f("apk/M: Tuer-Soll n/x", "apk", _in_zeile(_in_zeile(A, TP, "30/30", "30/x").split("\n"), TR, "27/27", "27/x"), 0, 2)
    # ---- quellbaum
    SCHLUSS_Q = "== APK-ASSET-GATE-QUELLBAUM-OK:"
    for w in ("Dateien", "in", "Baeumen", "Tuer", "Soll"):
        f("qb/M: Schluss Wort '%s' anders" % w, "quellbaum", _wort_in_zeile(Q, SCHLUSS_Q + " ", w), 0, 2)
    f("qb/M: Schluss x Dateien", "quellbaum", _in_zeile(Q, SCHLUSS_Q, "OK: 12", "OK: x"), 0, 2)
    f("qb/M: Schluss in x Baeumen", "quellbaum", _in_zeile(Q, SCHLUSS_Q, "in 2 Baeumen", "in x Baeumen"), 0, 2)
    f("qb/M: Baumzeile '2 Dateie' (A6)", "quellbaum", _in_zeile(Q, "   synchro", "2 Dateien", "2 Dateie"), 0, 2)
    f("qb/M: Baumzeile '2 Bytes'", "quellbaum", _in_zeile(Q, "   synchro", "2 Dateien", "2 Bytes"), 0, 2)
    f("qb/M: Baumzeile x Dateien", "quellbaum", _in_zeile(Q, "   synchro", "2 Dateien", "x Dateien"), 0, 2)
    f("qb/M: Baumzeile ohne Zwischenraum", "quellbaum", _in_zeile(Q, "   synchro", "synchro   2", "synchro2"), 0, 2)
    # ---- paket
    SCHLUSS_P = "== APK-ASSET-GATE-PAKET-OK:"
    f("pk/M: Schluss 'nichts zusaetzlic'", "paket", _in_zeile(P, SCHLUSS_P, "zusaetzlich ==", "zusaetzlic =="), 0, 2)
    for w in ("Dateien", "in", "Baeumen", "bytegleich", "nichts"):
        f("pk/M: Schluss Wort '%s' anders" % w, "paket", _wort_in_zeile(P, SCHLUSS_P + " ", w), 0, 2)
    f("pk/M: Schluss x Dateien", "paket", _in_zeile(P, SCHLUSS_P, "OK: 12", "OK: x"), 0, 2)
    f("pk/M: Schluss in x Baeumen", "paket", _in_zeile(P, SCHLUSS_P, "in 2 Baeumen", "in x Baeumen"), 0, 2)
    f("pk/M: Baumzeile Quelle x", "paket", _in_zeile(P, "   synchro", "2   2", "x   2"), 0, 2)
    f("pk/M: Baumzeile gleich x", "paket", _in_zeile(P, "   synchro", "2   2", "2   x"), 0, 2)
    f("pk/M: Baumzeile ohne Zwischenraum 1", "paket", _in_zeile(P, "   synchro", "synchro   2", "synchro2"), 0, 2)
    f("pk/M: Baumzeile ohne Zwischenraum 2", "paket", _in_zeile(P, "   synchro", "2   2", "22"), 0, 2)
    # ---- (e) Text hinter der gelesenen Zeile (das Muster gilt fuer die GANZE Zeile, re.fullmatch) - je Muster einer
    for titel, modus, L, anfang, alt, neu in (
            ("st/M: Schluss '(...) X'", "selbsttest", S, SCHLUSS_S, "abgelehnt) ==", "abgelehnt) X =="),
            ("st/M: Innere Proben '(...) X'", "selbsttest", S, IP, "v2)", "v2) X"),
            ("apk/M: Schluss 'sie liest X'", "apk", A, SCHLUSS_A, "liest ==", "liest X =="),
            ("apk/M: SUMME mit Zusatz", "apk", A, "   SUMME", " 4096", " 4096 X"),
            ("apk/M: Manifest '(...) X'", "apk", A, MF, "Datei)", "Datei) X"),
            ("apk/M: RE2/DOOR '27/27 X'", "apk", A, D, "27/27", "27/27 X"),
            ("apk/M: TORSE.VBS 'sha256 gleich, aber X'", "apk", A, T, "sha256 gleich", "sha256 gleich, aber X"),
            ("qb/M: Schluss 'erfuellt X'", "quellbaum", Q, SCHLUSS_Q, "erfuellt ==", "erfuellt X =="),
            ("qb/M: Baumzeile '2 Dateien X'", "quellbaum", Q, "   synchro", "2 Dateien", "2 Dateien X"),
            ("pk/M: Schluss 'zusaetzlich X'", "paket", P, SCHLUSS_P, "zusaetzlich ==", "zusaetzlich X =="),
            ("pk/M: Baumzeile '2   2 X'", "paket", P, "   synchro", "2   2", "2   2 X")):
        f(titel, modus, _in_zeile(L, anfang, alt, neu), 0, 2)
    # ---- (f) ein Pflicht-Leerzeichen weg, das ein benachbartes .* sonst schlucken wuerde
    f("apk/M: 'Tuer-Soll:' ohne Leerzeichen dahinter", "apk", _in_zeile(A, TS, "Tuer-Soll: ", "Tuer-Soll:", True), 0, 2)
    f("M: 'FEHLER:1 Befunde' (ohne Leerzeichen), Rueckgabe 1", "apk", "   x\n== APK-ASSET-GATE-FEHLER:1 Befunde ==", 1, 2)
    f("M: '1 Befunde==' (ohne Leerzeichen), Rueckgabe 1", "apk", "   x\n== APK-ASSET-GATE-FEHLER: 1 Befunde==", 1, 2)
    f("st/M: Fallzeile '02Fall' (ohne Leerzeichen nach der Nummer)", "selbsttest", _in_zeile(S, "   [ok] 02", "02 Fall", "02Fall"),
      0, 2)
    f("st/M: Fallzeile 'Fall 2rc=' (ohne Leerzeichen vor rc=)", "selbsttest",
      _in_zeile(S, "   [ok] 02", "Fall 2" + " " * 20 + " rc=", "Fall 2rc="), 0, 2)


def _faelle():
    """(Titel, modus, Ausgabe, Rueckgabe des Gates, apk_eintraege, erwartetes Urteil)"""
    S, A, Q, P = _selbsttest_log(), _apk_log(), _quellbaum_log(), _paket_log()
    s, a, q, p = "\n".join(S), "\n".join(A), "\n".join(Q), "\n".join(P)
    F = []

    def f(titel, modus, text, rc, soll, ein=""):
        F.append((titel, modus, text, rc, ein, soll))

    # --- gute Laeufe (Urteil 0) - und dieselben mit Rueckgabe 1/2
    f("selbsttest gut", "selbsttest", s, 0, 0)
    f("selbsttest gut, CRLF", "selbsttest", s.replace("\n", "\r\n"), 0, 0)
    f("selbsttest gut, mehr Faelle und Proben als verlangt", "selbsttest", "\n".join(_selbsttest_log(7, 4)), 0, 0)
    f("apk gut", "apk", a, 0, 0)
    f("apk gut, unzip-Zaehlung = n + 1", "apk", a, 0, 0, "13")
    f("quellbaum gut", "quellbaum", q, 0, 0)
    f("paket gut", "paket", p, 0, 0)
    # Randwerte, die noch GUT sind (sonst ueberleben Mutanten "> 0" -> "> 1", "== 0" -> "== 1", "<= 0" -> "<= 1")
    f("apk gut mit 1 Datei", "apk", "\n".join(_apk_log(1, 7)), 0, 0)
    f("apk gut, Tuerarchive je 1", "apk", _ersetze(_ersetze(A, "Quelle 27, APK 27, sha256 gleich 27/27",
                                                            "Quelle 1, APK 1, sha256 gleich 1/1").split("\n"),
                                                   "Quelle 30, APK 30, sha256 gleich 30/30", "Quelle 1, APK 1, sha256 gleich 1/1"), 0, 0)
    f("apk gut, Tuer-Soll 1/1", "apk", _ersetze(A, "30/30 wie", "1/1 wie"), 0, 0)
    f("quellbaum gut, 1 Datei in 1 Baum", "quellbaum", "\n".join(["   synchro   1 Dateien"] + TUER + [
        "== APK-ASSET-GATE-QUELLBAUM-OK: 1 Dateien in 1 Baeumen, Tuer-Soll erfuellt =="]), 0, 0)
    f("paket gut, 1 Datei in 1 Baum", "paket", "\n".join(["   synchro   1   1"] + TUER + [
        "== APK-ASSET-GATE-PAKET-OK: 1 Dateien in 1 Baeumen bytegleich, nichts zusaetzlich =="]), 0, 0)
    for modus, t in (("selbsttest", s), ("apk", a), ("quellbaum", q), ("paket", p)):
        f("%s gut, Rueckgabe 1" % modus, modus, t, 1, 2)
        f("%s gut, Rueckgabe 2" % modus, modus, t, 2, 2)
    # --- Befund-Laeufe: FEHLER/ABWEICHUNG mit Rueckgabe 1 = Urteil 1, mit 0/2 = 2 (F-Y2: genau DIESER Fall)
    for modus, marke in (("selbsttest", "SELBSTTEST"), ("apk", "APK-ASSET-GATE"), ("quellbaum", "APK-ASSET-GATE-QUELLBAUM"),
                         ("paket", "APK-ASSET-GATE-PAKET")):
        for wort in ("FEHLER", "ABWEICHUNG"):
            t = "   x\n== %s-%s: 1 Befunde ==" % (marke, wort)
            f("%s %s, Rueckgabe 1" % (modus, wort), modus, t, 1, 1)
            f("%s %s, Rueckgabe 0" % (modus, wort), modus, t, 0, 2)
            f("%s %s, Rueckgabe 2" % (modus, wort), modus, t, 2, 2)
    # --- keine Aussage (2)
    f("leer", "selbsttest", "", 0, 2)
    f("nur Leerzeilen", "apk", "\n  \n\t\n", 0, 2)
    f("ohne Schlusszeile", "selbsttest", "\n".join(S[:-1]), 0, 2)
    f("Schlusszeile eines anderen Modus", "apk", s, 0, 2)
    f("quellbaum-Schluss als apk", "apk", q, 0, 2)
    f("paket-Schluss als quellbaum", "quellbaum", p, 0, 2)
    f("Traceback nach der Schlusszeile", "selbsttest", s + "\nTraceback (most recent call last):", 0, 2)
    f("Traceback im Lauf", "quellbaum", "Traceback (most recent call last):\n" + q, 0, 2)
    f("ABBRUCH im Lauf", "paket", "ABBRUCH (Rueckgabe 2): x\n" + p, 0, 2)
    f("[FEHLER] im Lauf", "apk", "   [FEHLER] x\n" + a, 0, 2)
    f("zwei Urteilszeilen", "selbsttest", "== APK-ASSET-GATE-OK: x ==\n" + s, 0, 2)
    f("zwei Urteilszeilen (FEHLER davor)", "quellbaum", "== SELBSTTEST-FEHLER: x ==\n" + q, 0, 2)
    # Nachbesserung 1: jede Alternative des Urteilszeilen-Musters einmal als fremde Zeile davor (Gegenprobe D5/R6)
    f("zwei Urteilszeilen (ABWEICHUNG davor)", "apk", "== APK-ASSET-GATE-ABWEICHUNG: x ==\n" + a, 0, 2)
    f("zwei Urteilszeilen (PAKET-OK davor)", "selbsttest", "== APK-ASSET-GATE-PAKET-OK: x ==\n" + s, 0, 2)
    f("zwei Urteilszeilen (QUELLBAUM-FEHLER davor)", "paket", "== APK-ASSET-GATE-QUELLBAUM-FEHLER: x ==\n" + p, 0, 2)
    # Nachbesserung 1: "[FEHLER]" zaehlt IRGENDWO in der Zeile, nicht nur am Anfang (Gegenprobe D1)
    f("[FEHLER] tiefer eingerueckt im Lauf", "apk", "      Probe 3: [FEHLER] x\n" + a, 0, 2)
    f("unbekannter Modus", "xyz", s, 0, 2)
    # selbsttest
    f("st: Schluss ohne n/n Faelle", "selbsttest", _ersetze(S, "5/5 Faelle (", "5/5 Fall ("), 0, 2)
    f("st: 4/5 Faelle", "selbsttest", _ersetze(S, "5/5 Faelle", "4/5 Faelle"), 0, 2)
    f("st: 6/7 Faelle, 6 Fallzeilen", "selbsttest", _ersetze(_selbsttest_log(6, 3), "6/6 Faelle", "6/7 Faelle"), 0, 2)
    f("st: n unter Mindestzahl", "selbsttest", "\n".join(_selbsttest_log(4, 3)), 0, 2)
    f("st: Innere Proben fehlen", "selbsttest", "\n".join(z for z in S if "Innere" not in z), 0, 2)
    f("st: Innere Proben doppelt", "selbsttest", "\n".join(S[:2] + S[1:]), 0, 2)
    f("st: Innere Proben 2/3", "selbsttest", _ersetze(S, "Innere Proben: 3/3", "Innere Proben: 2/3"), 0, 2)
    f("st: Innere Proben 2/2 (unter Mindestzahl)", "selbsttest", _ersetze(S, "Innere Proben: 3/3", "Innere Proben: 2/2"), 0, 2)
    f("st: Innere Proben 3/2", "selbsttest", _ersetze(S, "Innere Proben: 3/3", "Innere Proben: 3/2"), 0, 2)
    f("st: Fallzeile fehlt", "selbsttest", "\n".join(z for z in S if not z.startswith("   [ok] 03")), 0, 2)
    f("st: Fallzeile doppelt", "selbsttest", "\n".join(S[:3] + S[2:]), 0, 2)
    f("st: Fallnummer 6 statt 5", "selbsttest", _ersetze(S, "   [ok] 05 ", "   [ok] 06 "), 0, 2)
    f("st: Fallnummer 0 statt 1", "selbsttest", _ersetze(S, "   [ok] 01 ", "   [ok] 00 "), 0, 2)
    f("st: Fallzeile rc=0 (soll 1)", "selbsttest", _ersetze(S, "rc=1 (soll 1)", "rc=0 (soll 1)"), 0, 2)
    f("st: Fallzeile rc=1 (soll 0)", "selbsttest", _ersetze(S, "rc=0 (soll 0)", "rc=1 (soll 0)"), 0, 2)
    f("st: Fallzeile mit Zusatz (fehlende Meldung)", "selbsttest",
      _ersetze(S, "rc=1 (soll 1)", "rc=1 (soll 1)  fehlende Meldung: ['x']"), 0, 2)
    f("st: Fallzeile [FEHLER]", "selbsttest", _ersetze(S, "   [ok] 02", "   [FEHLER] 02"), 0, 2)
    f("st: Fallzeile [Ok] (nicht erkannt)", "selbsttest", _ersetze(S, "   [ok] 02", "   [Ok] 02"), 0, 2)
    f("st: Fallzeile rc=-1 (soll -1)", "selbsttest", _ersetze(S, "rc=1 (soll 1)", "rc=-1 (soll -1)"), 0, 0)
    # apk
    f("apk: Schluss anderer Wortlaut", "apk", _ersetze(A, "Manifest stimmt", "Manifest egal"), 0, 2)
    f("apk: 0 Dateien", "apk", "\n".join(_apk_log(0, 0)), 0, 2)
    f("apk: SUMME fehlt", "apk", "\n".join(z for z in A if "SUMME" not in z), 0, 2)
    f("apk: SUMME doppelt", "apk", "\n".join(A[:4] + A[3:]), 0, 2)
    for t_, w in (("Quelle 11", (11, 12, 12, 4096)), ("APK 11", (12, 11, 12, 4096)), ("gleich 11", (12, 12, 11, 4096)),
                  ("Bytes 4095", (12, 12, 12, 4095))):
        f("apk: SUMME " + t_, "apk", _mit(A, "   SUMME", "   SUMME %28d %7d %8d %12d" % w), 0, 2)
    f("apk: Manifest 11 Zeilen", "apk", _ersetze(A, "Manifest:   12 Zeilen", "Manifest:   11 Zeilen"), 0, 2)
    f("apk: Manifest-Zeile fehlt", "apk", "\n".join(z for z in A if "Manifest:" not in z), 0, 2)
    f("apk: Schluss 13 Dateien", "apk", _ersetze(A, "== APK-ASSET-GATE-OK: 12 ", "== APK-ASSET-GATE-OK: 13 "), 0, 2)
    for lab, zahl in (("RE2/DOOR", 27), ("RE15DOOR", 30)):
        z = "Quelle %d, APK %d, sha256 gleich %d/%d" % (zahl, zahl, zahl, zahl)
        f("apk: %s Quelle-1" % lab, "apk", _ersetze(A, z, "Quelle %d, APK %d, sha256 gleich %d/%d" % (zahl - 1, zahl, zahl, zahl)), 0, 2)
        f("apk: %s APK-1" % lab, "apk", _ersetze(A, z, "Quelle %d, APK %d, sha256 gleich %d/%d" % (zahl, zahl - 1, zahl, zahl)), 0, 2)
        f("apk: %s gleich-1" % lab, "apk", _ersetze(A, z, "Quelle %d, APK %d, sha256 gleich %d/%d" % (zahl, zahl, zahl - 1, zahl)), 0, 2)
        f("apk: %s von-1" % lab, "apk", _ersetze(A, z, "Quelle %d, APK %d, sha256 gleich %d/%d" % (zahl, zahl, zahl, zahl - 1)), 0, 2)
        f("apk: %s alles 0" % lab, "apk", _ersetze(A, z, "Quelle 0, APK 0, sha256 gleich 0/0"), 0, 2)
        f("apk: %s fehlt" % lab, "apk", "\n".join(x for x in A if not x.startswith("   %s:" % lab)), 0, 2)
    f("apk: TORSE.VBS fehlt", "apk", "\n".join(z for z in A if "TORSE" not in z), 0, 2)
    f("apk: unzip-Zaehlung = n", "apk", a, 0, 2, "12")
    f("apk: unzip-Zaehlung = n + 2", "apk", a, 0, 2, "14")
    # Nachbesserung 2 (Abnahme 1, M2): nur die Tuer-Soll-Zeilen weg - vorher filterte "Tuer-Soll" not in z auch die
    # Schlusszeile (quellbaum/apk: "... Tuer-Soll erfuellt ...") mit, und der Fall erreichte die Regel nie
    f("apk: Tuer-Soll fehlt", "apk", "\n".join(z for z in A if not z.startswith("   Tuer-Soll:")), 0, 2)
    f("apk: Tuer-Soll 29/30", "apk", _ersetze(A, "30/30 wie", "29/30 wie"), 0, 2)
    f("apk: Tuer-Soll 0/0", "apk", _ersetze(A, "27/27 wie", "0/0 wie"), 0, 2)
    # quellbaum
    f("qb: Schluss anderer Wortlaut", "quellbaum", _ersetze(Q, "Tuer-Soll erfuellt ==", "Tuer-Soll egal =="), 0, 2)
    f("qb: Baumzeile fehlt", "quellbaum", "\n".join(z for z in Q if "synchro" not in z), 0, 2)
    f("qb: Summe der Baumzeilen 11", "quellbaum", _ersetze(Q, "synchro   2 Dateien", "synchro   1 Dateien"), 0, 2)
    f("qb: Baum mit 0 Dateien", "quellbaum", _ersetze(_ersetze(Q, "synchro   2 Dateien", "synchro   0 Dateien").split("\n"),
                                                     "PSX   10 Dateien", "PSX   12 Dateien"), 0, 2)
    f("qb: 0 Dateien in 0 Baeumen", "quellbaum", "\n".join(TUER + ["== APK-ASSET-GATE-QUELLBAUM-OK: 0 Dateien in 0 Baeumen, "
                                                                   "Tuer-Soll erfuellt =="]), 0, 2)
    f("qb: Tuer-Soll fehlt", "quellbaum", "\n".join(z for z in Q if not z.startswith("   Tuer-Soll:")), 0, 2)
    # paket
    f("pk: Schluss anderer Wortlaut", "paket", _ersetze(P, "nichts zusaetzlich", "egal"), 0, 2)
    f("pk: Quelle != gleich", "paket", _ersetze(P, "PSX   10   10", "PSX   10   9"), 0, 2)
    f("pk: Quelle 11 != gleich 10, Summe gleich passt", "paket", _ersetze(P, "PSX   10   10", "PSX   11   10"), 0, 2)
    f("pk: Baumzeile fehlt", "paket", "\n".join(z for z in P if "synchro" not in z), 0, 2)
    f("pk: Summe der Baumzeilen 11", "paket", _ersetze(P, "synchro   2   2", "synchro   1   1"), 0, 2)
    f("pk: Baum mit 0/0", "paket", _ersetze(_ersetze(P, "synchro   2   2", "synchro   0   0").split("\n"),
                                            "PSX   10   10", "PSX   12   12"), 0, 2)
    f("pk: 0 Dateien in 0 Baeumen", "paket", "\n".join(TUER + ["== APK-ASSET-GATE-PAKET-OK: 0 Dateien in 0 Baeumen bytegleich, "
                                                               "nichts zusaetzlich =="]), 0, 2)
    f("pk: Tuer-Soll fehlt", "paket", "\n".join(z for z in P if not z.startswith("   Tuer-Soll:")), 0, 2)
    _faelle_muster(f, S, A, Q, P)
    _faelle_seiten(f, S, A, Q, P)
    _faelle_nb3(f, S, A, Q, P)
    _faelle_stoerung(f, S, A, Q, P)
    return F


# Mutanten, die kein Fall erkennen KANN (gleichwertig zum Original) - je Eintrag die Begruendung. Schluessel =
# Beschreibung des Mutanten (Quelltext-Stelle + Aenderung), stabil, solange die Stelle unveraendert bleibt.
AEQUIVALENT = {
    # tuer_soll: "g1 != g2 or int(g2) == 0" - hinter dem ersten Teil gilt g1 == g2, also ist int(g1) == 0 dasselbe
    "Zahl 2 -> 1: 2":
        "tuer_soll int(m.group(2)) == 0 -> int(m.group(1)) == 0: wird nur bei g1 == g2 ausgewertet (or kurzgeschlossen)",
    # m_neg mit Rueckgabe != 1: ohne dieses ende() faellt der Lauf auf "if not m_ok" - eine FEHLER/ABWEICHUNG-Schlusszeile
    # ist nie zugleich eine OK-Zeile, also endet er dort mit 2 (nur der Text des Grundes unterscheidet sich)
    'Aufruf weg: ende(2, "Schlusszeile meldet %s, aber Rueckgabe %d statt 1 - Ausgangspfad des Gates defekt" % '
    '(m_neg.group(1), rc))':
        "der naechste Schritt (not m_ok) entscheidet denselben Fall ebenfalls mit 2",
    # der paket-Zweig ist der letzte: jeder andere bekannte Modus hat vorher mit ende() geendet, ein unbekannter oben
    'if-Bedingung -> True: modus == "paket"':
        "nur modus == 'paket' erreicht diese Stelle (alle anderen Zweige enden vorher mit ende())",
    # Fallzeilen-Muster \[(ok|FEHLER)\]: jede Zeile mit "[FEHLER]" hat schon die Schleife davor ("[FEHLER]" in z) mit 2
    # beendet - die Alternative FEHLER wird nie mehr gebraucht (Nachbesserung 1, Operator R6)
    r'''Regex R6 Gruppe bei 5: Alternative 'FEHLER' weg: r" \[(ok|FEHLER)\] (\d+) (.*) rc=(-?\d+) \(soll (-?\d+)\)(.*)"''':
        "eine [FEHLER]-Fallzeile enthaelt '[FEHLER]' und endet schon in der Schleife davor mit 2",
    # Nachbesserung 2 (Abnahme 1, M1): die neuen Ordnungs-Mutanten von == / != - drei sind auf dem Wertebereich gleich.
    # tuer_soll: g2 kommt aus (\d+) - nur Ziffern, int() >= 0; "<= 0" ist dort dasselbe wie "== 0"
    "Vergleich 0 Eq -> LtE: int(m.group(2)) == 0":
        "g2 = int((\\d+)) ist nie negativ: <= 0 und == 0 sind auf jedem moeglichen Wert gleich",
    # paket: qq kommt aus (\d+) - dasselbe
    "Vergleich 0 Eq -> LtE: qq == 0":
        "qq = int((\\d+)) ist nie negativ: <= 0 und == 0 sind auf jedem moeglichen Wert gleich",
    # Urteilszeilen: an dieser Stelle hat m_ok die letzte Zeile als '== <MARKE>-OK: ... ==' erkannt, und jede solche Zeile
    # passt auch auf das Urteilszeilen-Muster (MARKE ist eine seiner Alternativen, hinter 'OK' folgt ':' = \b) - die
    # Liste hat also mindestens 1 Eintrag, und "> 1" ist dasselbe wie "!= 1"
    "Vergleich 0 NotEq -> Gt: len(urteile) != 1":
        "die OK-Schlusszeile (m_ok) ist selbst eine Urteilszeile, len(urteile) >= 1: > 1 und != 1 sind gleich",
}


# ---------------------------------------------------------------------------------------------------------------------
def _regex_stuecke(p):
    """Zerlegt ein Regex-Muster (die hier benutzte Teilmenge der re-Syntax) in Stuecke AUF OBERSTER EBENE:
    -> Liste (anfang, ende, art). art: "wort" (Literal aus Buchstaben/Ziffern/_ ab 2 Zeichen, ohne Quantor), "zeichen"
    (sonstiges Literal), "esc" (\\x), "klasse" ([...]), "gruppe" ((...), samt Inhalt), "punkt" (.), "fmt" (%s);
    ein Quantor (*, +, ?, {m,n}, auch lazy) gehoert zum Stueck davor (art + "+q")."""
    out, i, n = [], 0, len(p)
    while i < n:
        a, c = i, p[i]
        if c == "\\":
            i, art = i + 2, "esc"
        elif c == "[":
            j = i + 1
            if j < n and p[j] == "^":
                j += 1
            if j < n and p[j] == "]":
                j += 1
            while j < n and p[j] != "]":
                j += 2 if p[j] == "\\" else 1
            i, art = j + 1, "klasse"
        elif c == "(":
            tiefe, j = 0, i
            while j < n:
                if p[j] == "\\":
                    j += 2
                    continue
                if p[j] == "[":
                    j += 1
                    while j < n and p[j] != "]":
                        j += 2 if p[j] == "\\" else 1
                elif p[j] == "(":
                    tiefe += 1
                elif p[j] == ")":
                    tiefe -= 1
                    if tiefe == 0:
                        break
                j += 1
            i, art = j + 1, "gruppe"
        elif c == "%" and i + 1 < n and p[i + 1] in "s%":
            i, art = i + 2, "fmt"
        elif c == ".":
            i, art = i + 1, "punkt"
        elif c.isalnum() or c == "_":
            while i < n and (p[i].isalnum() or p[i] == "_"):
                i += 1
            if i < n and p[i] in "*+?{" and i - a > 1:
                i -= 1                            # Quantor bindet nur das letzte Zeichen: "ab+" = "a" + "b+"
            art = "wort" if i - a > 1 else "zeichen"
        else:
            i, art = i + 1, "zeichen"
        q = re.match(r"(?:[*+?]|\{\d*(?:,\d*)?\})\??", p[i:])
        if q:
            i += q.end()
            art += "+q"
        out.append((a, i, art))
    return out


@functools.lru_cache(maxsize=None)
def _regex_lockerungen(p, voll):
    """Die Lockerungen eines Musters, die der Selbsttest als Mutanten einsetzt (Abnahme 0, M1):
      R1 "Rest ab Stueck k -> .*"   je Stueck auf oberster Ebene (die Abnahme-Aenderungen A5, A6, A11 sind von dieser Art)
      R2 "Wort k -> .*"             je Literal-Wort auf oberster Ebene (ein Wort wird nicht mehr verlangt)
      R3 "\\d+ Nr. k -> .*"         je Vorkommen, auch in Gruppen (eine Zahl wird nicht mehr verlangt)
      R4 "\\s+ Nr. k -> \\s*"       je Vorkommen (ein Zwischenraum wird nicht mehr verlangt)
      R5 "Zeichen k weg"            je einzelnes Literal-Zeichen auf oberster Ebene (Leerzeichen, Satzzeichen ...)
      R6 "Alternative weg"          je Gruppe mit mindestens zwei Alternativen (auf jeder Tiefe) je eine Alternative weg
      R0 "Muster + .*"              nur fuer re.fullmatch/genau_eine: Text hinter dem Muster wird erlaubt
    -> Liste (beschreibung, neues_muster); jedes neue Muster nur einmal, das Original nie."""
    st = _regex_stuecke(p)
    out, gesehen = [], {p}
    if voll and not p.endswith((".*", "(.*)")):   # endet es schon mit (.*), bliebe ein angehaengtes .* immer leer
        out.append(("Regex R0 Muster + .*", p + ".*"))
        gesehen.add(p + ".*")

    def dazu(was, neu):
        if neu not in gesehen:
            gesehen.add(neu)
            out.append((was, neu))
    for k, (a, _e, _art) in enumerate(st):
        if p[a:] != ".*":
            dazu("Regex R1 Rest ab Stueck %d %r -> .*" % (k, p[a:a + 24]), p[:a] + ".*")
    for k, (a, e, art) in enumerate(st):
        if art == "wort":
            dazu("Regex R2 Wort %d %r -> .*" % (k, p[a:e]), p[:a] + ".*" + p[e:])
    for k, m in enumerate(re.finditer(r"(?<!\\)\\d\+", p)):
        dazu("Regex R3 \\d+ Nr. %d -> .*" % k, p[:m.start()] + ".*" + p[m.end():])
    for k, m in enumerate(re.finditer(r"(?<!\\)\\s\+", p)):
        dazu("Regex R4 \\s+ Nr. %d -> \\s*" % k, p[:m.start()] + "\\s*" + p[m.end():])
    for k, (a, e, art) in enumerate(st):
        if art == "zeichen":
            dazu("Regex R5 Zeichen %d %r weg" % (k, p[a:e]), p[:a] + p[e:])
    for a, alt_anf, alt_end in _regex_alternativen(p):
        for j, (x, y) in enumerate(zip(alt_anf, alt_end)):
            rest = [p[u:v] for i, (u, v) in enumerate(zip(alt_anf, alt_end)) if i != j]
            dazu("Regex R6 Gruppe bei %d: Alternative %r weg" % (a, p[x:y]), p[:alt_anf[0]] + "|".join(rest) + p[alt_end[-1]:])
    return out


def _regex_alternativen(p):
    """-> je Gruppe (auf jeder Tiefe) mit mindestens zwei Alternativen: (Position der Klammer, Anfaenge, Enden)"""
    out, stapel, i, n = [], [], 0, len(p)
    while i < n:
        c = p[i]
        if c == "\\":
            i += 2
            continue
        if c == "[":
            i += 1
            while i < n and p[i] != "]":
                i += 2 if p[i] == "\\" else 1
        elif c == "(":
            stapel.append((i, [i + 1]))
        elif c == "|" and stapel:
            stapel[-1][1].append(i + 1)
        elif c == ")" and stapel:
            a, anf = stapel.pop()
            if len(anf) > 1:
                out.append((a, anf, [x - 1 for x in anf[1:]] + [i]))
        i += 1
    return out


class _Mutierer(ast.NodeTransformer):
    """Erzeugt je Lauf genau EINE Aenderung an der Stelle nr (Zaehlung in Besuchsreihenfolge). Operatoren (Stand
    Nachbesserung 2, Abnahme 1 M1): Vergleich == und != -> jeder andere der sechs Vergleiche (darunter die einseitigen
    Lockerungen != -> < / >, == -> <= / >=), < <-> <=, > <-> >=, < <-> >, <= <-> >= (keine Ordnungs-Operatoren, wo eine
    Seite eine Zeichenkette ist), in/not in, is/is not; and/or; not weg; if-Bedingung -> False/True; ganze Zahl +-1;
    Ausdrucks-Aufruf weg (auch jede Pruef-Anweisung wie tuer_soll()); JEDE Zeichenkette ausserhalb von Meldungstexten
    (Regex-Muster, MARKE-Tabelle, Modusnamen, Pruef-Literale wie "[FEHLER]"/"ABBRUCH"/"ok"/"\\r") -> + Zeichen U+00A7;
    jedes Regex-Muster (erstes Argument von re.fullmatch/re.match/re.search/genau_eine, auch links von %) zusaetzlich
    die Lockerungen R0-R6 aus _regex_lockerungen. Meldungstext = zweites Argument von ende() und von genau_eine() - nur
    dort wird nichts mutiert; deshalb darf keine PRUEFUNG im Meldungstext stehen (Nachbesserung 2, M2: tuer_soll())."""
    # Nachbesserung 2 (Abnahme 1, M1): je Vergleichsoperator MEHRERE Ersatz-Operatoren. == und != werden durch JEDEN
    # anderen Vergleich ersetzt - darunter die einseitigen Lockerungen in BEIDE Richtungen (!= -> < und != -> >,
    # == -> <= und == -> >=), die vorher kein Mutant waren (TAUSCH bildete != nur auf == ab); <, <=, >, >= bekommen die
    # Grenzverschiebung (< <-> <=, > <-> >=) und die umgekehrte Richtung (< <-> >, <= <-> >=). Ordnungs-Operatoren
    # werden NICHT eingesetzt, wenn eine Seite eine Zeichenkette ist (eine Textordnung ist keine Lockerung einer Zahl).
    TAUSCH = {ast.Eq: (ast.NotEq, ast.LtE, ast.GtE, ast.Lt, ast.Gt), ast.NotEq: (ast.Eq, ast.Lt, ast.Gt, ast.LtE, ast.GtE),
              ast.Lt: (ast.LtE, ast.Gt), ast.LtE: (ast.Lt, ast.GtE), ast.Gt: (ast.GtE, ast.Lt), ast.GtE: (ast.Gt, ast.LtE),
              ast.In: (ast.NotIn,), ast.NotIn: (ast.In,), ast.Is: (ast.IsNot,), ast.IsNot: (ast.Is,)}
    ORDNUNG = (ast.Lt, ast.LtE, ast.Gt, ast.GtE)
    REGEX_RUFE = {"fullmatch", "match", "search", "genau_eine"}

    def __init__(self, quelle, ziel):
        self.quelle, self.ziel, self.nr, self.beschreibung, self.zeile = quelle, ziel, -1, None, 0
        self.meldung = 0                  # > 0: innerhalb eines Meldungstextes (nur Text, nie das Urteil)
        self.regex = 0                    # 1: Regex-Muster fuer die ganze Zeile (fullmatch), 2: nur Anfang (match)
        self.zaehler = {}                 # Operator -> Anzahl Stellen (fuer die Ausgabe des Selbsttests)

    def visit(self, node):                # nach dem Treffer bleibt der Rest des Baums unberuehrt
        return node if self.beschreibung is not None else ast.NodeTransformer.visit(self, node)

    def _in(self, feld, node, wert=None):
        alt = getattr(self, feld)
        setattr(self, feld, alt + 1 if wert is None else wert)
        try:
            return self.visit(node)
        finally:
            setattr(self, feld, alt)

    def visit_Call(self, node):
        f = node.func
        name = f.id if isinstance(f, ast.Name) else (
            f.attr if isinstance(f, ast.Attribute) and isinstance(f.value, ast.Name) and f.value.id == "re" else None)
        if name in ("ende", "genau_eine") and len(node.args) == 2 and not node.keywords:
            # ende(code, grund) / genau_eine(muster, name): grund und name sind nur Meldungstext
            node.args[0] = self._in("regex", node.args[0], 1) if name == "genau_eine" else self.visit(node.args[0])
            node.args[1] = self._in("meldung", node.args[1])
            return node
        if name in self.REGEX_RUFE and node.args:
            node.args[0] = self._in("regex", node.args[0], 1 if name in ("fullmatch", "genau_eine") else 2)
            node.args[1:] = [self._in("regex", x, 0) for x in node.args[1:]]
            return node
        node.func = self._in("regex", node.func, 0)
        node.args = [self._in("regex", x, 0) for x in node.args]
        return node

    def visit_BinOp(self, node):          # r"... %s ..." % re.escape(x): links bleibt Muster, rechts nicht
        node.left = self.visit(node.left) if isinstance(node.op, ast.Mod) else self._in("regex", node.left, 0)
        node.right = self._in("regex", node.right, 0)
        return node

    def _treffer(self, node, was):
        if self.meldung:
            return False
        op = " ".join(was.split(" ")[:2]) if was.startswith("Regex") else was.split(" ")[0]
        self.zaehler[op] = self.zaehler.get(op, 0) + 1
        self.nr += 1
        if self.nr == self.ziel:
            seg = _segment(self.quelle, node).replace("\n", " ")
            self.beschreibung = "%s: %s" % (was, re.sub(r"\s+", " ", seg))
            self.zeile = getattr(node, "lineno", 0)
            return True
        return False

    def visit_Compare(self, node):
        self.generic_visit(node)
        seiten = [node.left] + list(node.comparators)
        for i, op in enumerate(node.ops):
            text = any(isinstance(s, ast.Constant) and isinstance(s.value, str) for s in seiten[i:i + 2])
            for neu in self.TAUSCH.get(type(op), ()):
                if text and neu in self.ORDNUNG:
                    continue
                if self._treffer(node, "Vergleich %d %s -> %s" % (i, type(op).__name__, neu.__name__)):
                    node.ops[i] = neu()
                    return node
        return node

    def visit_BoolOp(self, node):
        self.generic_visit(node)
        if self._treffer(node, "BoolOp %s -> %s" % (type(node.op).__name__, "Or" if isinstance(node.op, ast.And) else "And")):
            node.op = ast.Or() if isinstance(node.op, ast.And) else ast.And()
        return node

    def visit_UnaryOp(self, node):
        self.generic_visit(node)
        if isinstance(node.op, ast.Not) and self._treffer(node, "not weg"):
            return node.operand
        return node

    def visit_If(self, node):
        self.generic_visit(node)
        if self._treffer(node.test, "if-Bedingung -> False"):
            node.test = ast.copy_location(ast.Constant(value=False), node.test)
        elif self._treffer(node.test, "if-Bedingung -> True"):
            node.test = ast.copy_location(ast.Constant(value=True), node.test)
        return node

    def visit_Constant(self, node):
        if isinstance(node.value, str):
            if self.meldung:
                return node
            if self._treffer(node, "Zeichenkette + U+00A7"):
                return ast.copy_location(ast.Constant(value=node.value + "\u00a7"), node)
            if self.regex:
                for was, neu in _regex_lockerungen(node.value, self.regex == 1):
                    if self._treffer(node, was):
                        return ast.copy_location(ast.Constant(value=neu), node)
            return node
        if isinstance(node.value, bool) or not isinstance(node.value, int):
            return node
        if self._treffer(node, "Zahl %d -> %d" % (node.value, node.value + 1)):
            return ast.copy_location(ast.Constant(value=node.value + 1), node)
        if node.value != 0 and self._treffer(node, "Zahl %d -> %d" % (node.value, node.value - 1)):
            return ast.copy_location(ast.Constant(value=node.value - 1), node)
        return node

    def visit_Expr(self, node):
        self.generic_visit(node)
        if isinstance(node.value, ast.Call) and self._treffer(node, "Aufruf weg"):
            return ast.copy_location(ast.Pass(), node)
        return node


def _segment(zeilen, node):
    """wie ast.get_source_segment, aber auf vorab zerlegten Zeilen (Bytes; col_offset zaehlt UTF-8-Bytes)"""
    if getattr(node, "end_lineno", None) is None:
        return "?"
    a, e = node.lineno - 1, node.end_lineno - 1
    if a == e:
        return zeilen[a][node.col_offset:node.end_col_offset].decode("utf-8")
    teile = [zeilen[a][node.col_offset:]] + zeilen[a + 1:e] + [zeilen[e][:node.end_col_offset]]
    return b"".join(teile).decode("utf-8")


def _urteil_quelle():
    """-> (Quelltext NUR der Funktion urteil, Zeilenversatz in der Datei). Jeder Mutant parst diesen Text neu (kein
    deepcopy - Nachbesserung 1: mit den Zeichenketten-Mutanten waere der Selbsttest sonst ~3x langsamer)."""
    with open(__file__, encoding="utf-8") as f:
        text = f.read()
    fn = next(n for n in ast.parse(text).body if isinstance(n, ast.FunctionDef) and n.name == "urteil")
    return ast.get_source_segment(text, fn), fn.lineno - 1


def _meldung_regel(text, versatz):
    """Nachbesserung 2 (Abnahme 1, M2): kein Meldungstext (zweites Argument von ende()/genau_eine()) darf eine Funktion
    rufen, die im Urteil selbst definiert ist (ende, genau_eine, tuer_soll, ...). Meldungstexte werden nicht mutiert -
    eine Pruefung darin (so stand tuer_soll() bis Nachbesserung 2 im Grund von ende(0, ...)) liesse sich ohne Signal
    entfernen. -> Liste der Verstoesse ("Zeile n: name()")."""
    fn = ast.parse(text).body[0]
    eigene = {n.name for n in ast.walk(fn) if isinstance(n, ast.FunctionDef)}
    out = []
    for c in ast.walk(fn):
        if (isinstance(c, ast.Call) and isinstance(c.func, ast.Name) and c.func.id in ("ende", "genau_eine")
                and len(c.args) == 2):
            for k in ast.walk(c.args[1]):
                if isinstance(k, ast.Call) and isinstance(k.func, ast.Name) and k.func.id in eigene:
                    out.append("Zeile %d: %s()" % (k.lineno + versatz, k.func.id))
    return out


def _mutant(text, versatz, nr):
    neu = ast.parse(text).body[0]
    m = _Mutierer(text.encode("utf-8").splitlines(keepends=True), nr)
    neu = m.visit(neu)
    if m.beschreibung is None:
        return None, None
    modul = ast.Module(body=[neu], type_ignores=[])       # jede neue Ausdrucks-Stelle traegt copy_location
    ns = {"re": re, "_Urteil": _Urteil}
    exec(compile(modul, "<mutant %d>" % nr, "exec"), ns)    # noqa: S102 - eigener Code, im Selbsttest
    return ns["urteil"], (m.beschreibung, m.zeile + versatz)


def selbsttest():
    print("== gate_urteil.py: Selbsttest (feste Gate-Ausgaben + Mutanten des Urteils) ==")
    faelle = _faelle()
    falsch = 0
    original = []                                 # (Urteil, Art) je Fall - der Massstab fuer die Mutanten
    for nr, (titel, modus, text, rc, ein, soll) in enumerate(faelle, 1):
        art = []
        code, grund = urteil_rufen(urteil, modus, text, rc, apk_eintraege=ein, art=art)
        original.append((code, art[0]))
        gut = code == soll and art[0] == "ende"   # das Original entscheidet JEDEN Fall begruendet (kein Absturz)
        falsch += not gut
        print("   [%s] %03d %-52s Urteil %d (soll %d)%s" % ("ok" if gut else "FEHLER", nr, titel[:52], code, soll,
                                                         "" if gut else "  %s: %s" % (art[0], grund[:100])))
    n_f = len(faelle)
    # Nachbesserung 3: Stoerungsfaelle - Umfang, begruendet ungepruefte (Soll 0), und jeder UNGEPRUEFT-Eintrag deckt etwas
    n_st = sum("/St: " in t[0] for t in faelle)
    n_ug = sum("/St: " in t[0] and t[5] == 0 for t in faelle)
    print("   Stoerungen: %d erzeugt, %d mit Soll 2, %d begruendet ungeprueft (Soll 0, %d UNGEPRUEFT-Eintraege)"
          % (n_st, n_st - n_ug, n_ug, len(UNGEPRUEFT)))
    for i, e in enumerate(UNGEPRUEFT):
        if i not in _UNGEPRUEFT_GENUTZT:
            print("   [FEHLER] UNGEPRUEFT-Eintrag deckt keine Stoerung mehr (entfernen): %r %r" % (e[0], e[1]))
            falsch += 1
    if n_st == 0:
        print("   [FEHLER] keine Stoerungsfaelle erzeugt")
        falsch += 1
    text, versatz = _urteil_quelle()
    for v in _meldung_regel(text, versatz):
        print("   [FEHLER] Pruefung im Meldungstext (wird nicht mutiert - als eigene Anweisung davor schreiben): " + v)
        falsch += 1
    n_m = erkannt = gleich = 0
    ueberlebt, aeq_gesehen = [], set()
    nr = 0
    while True:
        mfn, besch = _mutant(text, versatz, nr)
        nr += 1
        if mfn is None:
            break
        n_m += 1
        tot = False
        for (titel, modus, t, rc, ein, soll), (o_code, o_art) in zip(faelle, original):
            art = []
            if (urteil_rufen(mfn, modus, t, rc, apk_eintraege=ein, art=art)[0], art[0]) != (o_code, o_art):
                tot = True
                break
        besch, zeile = besch
        if tot:
            erkannt += 1
        elif besch in AEQUIVALENT:
            gleich += 1
            aeq_gesehen.add(besch)
        else:
            ueberlebt.append("Zeile %d: %s" % (zeile, besch))
    for b in ueberlebt:
        print("   [FEHLER] Mutant UEBERLEBT (kein Fall erkennt ihn - Fall ergaenzen oder in AEQUIVALENT begruenden): " + b)
    veraltet = sorted(set(AEQUIVALENT) - aeq_gesehen)
    for b in veraltet:
        print("   [FEHLER] AEQUIVALENT-Eintrag passt zu keinem gleichwertigen Mutanten mehr (entfernen): " + b)
    # Umfang je Operator (ein Zaehllauf ohne Treffer) - steht im Log, damit der gemessene Umfang nachlesbar ist
    zaehl = _Mutierer(text.encode("utf-8").splitlines(keepends=True), -2)
    zaehl.visit(ast.parse(text).body[0])
    print("   Mutanten je Operator: " + ", ".join("%s %d" % kv for kv in sorted(zaehl.zaehler.items())))
    if sum(zaehl.zaehler.values()) != n_m:
        print("   [FEHLER] Zaehllauf %d Stellen, erzeugt %d Mutanten" % (sum(zaehl.zaehler.values()), n_m))
        falsch += 1
    print("   Mutanten: %d erzeugt, %d erkannt, %d als gleichwertig begruendet, %d ueberlebt" % (n_m, erkannt, gleich,
                                                                                         len(ueberlebt)))
    if falsch or ueberlebt or veraltet or n_m == 0:
        print("== URTEIL-SELBSTTEST-FEHLER: %d von %d Faellen falsch, %d Mutanten ueberlebt, %d veraltete Eintraege =="
              % (falsch, n_f, len(ueberlebt), len(veraltet)))
        return 1
    print("== URTEIL-SELBSTTEST-OK: %d/%d Faelle, %d/%d Mutanten erkannt, %d als gleichwertig begruendet, "
          "%d Stoerungen begruendet ungeprueft ==" % (n_f, n_f, erkannt, n_m, gleich, n_ug))
    return 0


def main(argv):
    if argv[1:] == ["--selbsttest"]:
        return selbsttest()
    if len(argv) not in (6, 7):
        print("Aufruf: gate_urteil.py <modus> <ausgabe> <rueckgabe> <min_faelle> <min_innen> [<apk_eintraege>] | --selbsttest")
        return 2
    modus, log, rc = argv[1], argv[2], int(argv[3])
    text = open(log, "rb").read().decode("utf-8", "replace")
    code, grund = urteil_rufen(urteil, modus, text, rc, int(argv[4]), int(argv[5]), argv[6] if len(argv) > 6 else "")
    print("   Gate-Urteil (%s, Rueckgabe %d): %s" % (modus, rc, grund))
    return code


if __name__ == "__main__":
    sys.exit(main(sys.argv))
