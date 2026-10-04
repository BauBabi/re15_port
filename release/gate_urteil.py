#!/usr/bin/env python3
# =============================================================================
# release/gate_urteil.py — Urteil ueber einen Lauf von release/apk_asset_gate.py (Runde 35 Spur N, 2026-10-03)
# =============================================================================
# WARUM eine eigene Datei: bis Runde 35 stand dieser Code als Heredoc in release/apk_pruefen.sh (gate_urteil) und wurde
# von NIEMANDEM geprueft. Gegenpruefung R4-2, Befund F-Y2 (analysis/befunde_runde34_android/pruefer_umgehung_r4_2.md
# Abschnitt 2): EIN Zeichen - `ende(1, "das Gate meldet ...` -> `ende(0, ...` - machte aus der richtigen
# APK-ASSET-GATE-ABWEICHUNG des echten, gepinnten Gates ein ANDROID-GATES-OK fuer ein falsches Tuerarchiv. Eine
# nachsichtige Regression im Urteil ist STILL: jeder gute Lauf bleibt gruen. Nutzer (Runde 35): "Kuenftige Aenderungen
# am Pruefskript muessen dessen Urteilslogik selbst sorgfaeltig mittesten." Deshalb:
#   * --selbsttest prueft das Urteil an FESTEN Ausgaben (gute Laeufe je Modus, und je Regel ein Gegenbeispiel) UND an
#     MUTANTEN des eigenen Urteilscodes: jede Ein-Stellen-Aenderung (Vergleich, und/oder, not, Zahl, Rueckgabewert,
#     weggelassener Aufruf) muss von mindestens einem Fall erkannt werden - sonst ist sie als gleichwertig begruendet
#     (AEQUIVALENT unten) oder der Selbsttest scheitert. Wer das Urteil aendert und keinen Fall dazu schreibt,
#     bekommt einen ueberlebenden Mutanten = SELBSTTEST-FEHLER.
#   * release/apk_pruefen.sh haelt diese Datei wie das Gate fest (Pin release/gate_urteil.sha256, private Kopie),
#     laesst den Selbsttest vor JEDER Nutzung laufen und prueft dessen Schlusszeile in bash (Mindestzahlen dort), und
#     verlangt fuer ein OK-Urteil zusaetzlich - unabhaengig von diesem Code - Rueckgabe 0 des Gates (gate_laufen).
#   * ctest unit_r35_android_pruefkette laesst den Selbsttest und Negativ-Kontrollen mit dem echten Gate bei jedem
#     Suite-Lauf laufen.
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
        t = [m for m in (re.fullmatch(r"   Tuer-Soll: .* (\d+)/(\d+) wie die Engine-Tabelle .*", z) for z in zeilen) if m]
        if not t or any(m.group(1) != m.group(2) or int(m.group(2)) == 0 for m in t):
            ende(2, "Tuer-Soll-Zeilen fehlen oder melden nicht g/g - keine Aussage")
        return "%d Tuer-Soll-Zeilen g/g" % len(t)

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
            if f.group(1) != "ok" or f.group(4) != f.group(5) or f.group(6).strip():
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
        zusatz = ""
        if apk_eintraege:
            if int(apk_eintraege) != a + 1:
                ende(2, "unzip zaehlt %s Eintraege unter assets/, das Gate %d Asset-Dateien + Manifest" % (apk_eintraege, a))
            zusatz = " = unzip-Zaehlung - 1"
        ende(0, "APK-ASSET-GATE-OK: %d Dateien, Quelle = APK%s = gleich = Manifestzeilen, RE2/DOOR + RE15DOOR + TORSE.VBS "
                "gleich, %s" % (n, zusatz, tuer_soll()))
    if modus == "quellbaum":
        m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen, Tuer-Soll erfuellt", rest)
        if not m:
            ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
        n, k = int(m.group(1)), int(m.group(2))
        baum = [int(x.group(1)) for x in (re.fullmatch(r"   \S+\s+(\d+) Dateien", z) for z in zeilen) if x]
        if n <= 0 or len(baum) != k or sum(baum) != n or 0 in baum:
            ende(2, "Baumzeilen %r passen nicht zu %d Dateien in %d Baeumen" % (baum, n, k))
        ende(0, "APK-ASSET-GATE-QUELLBAUM-OK: %d Dateien in %d Baeumen (Summe der Baumzeilen), %s" % (n, k, tuer_soll()))
    if modus == "paket":
        m = re.fullmatch(r"(\d+) Dateien in (\d+) Baeumen bytegleich, nichts zusaetzlich", rest)
        if not m:
            ende(2, "Schlusszeile nicht im erwarteten Wortlaut - keine Aussage")
        n, k = int(m.group(1)), int(m.group(2))
        baum = [(int(x.group(1)), int(x.group(2))) for x in (re.fullmatch(r"   \S+\s+(\d+)\s+(\d+)", z) for z in zeilen) if x]
        if n <= 0 or len(baum) != k or any(qq != gg or qq == 0 for qq, gg in baum) or sum(gg for _qq, gg in baum) != n:
            ende(2, "Baumzeilen (Quelle, gleich) %r passen nicht zu %d Dateien in %d Baeumen" % (baum, n, k))
        ende(0, "APK-ASSET-GATE-PAKET-OK: %d Dateien in %d Baeumen, je Baum Quelle = gleich, %s" % (n, k, tuer_soll()))
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
    return ["== APK-Asset-Gate: pruefe x.apk gegen den Quellbaum ==",
            "   %-28s %7s %7s %8s %12s" % ("Baum", "Quelle", "APK", "gleich", "Bytes"),
            "   %-28s %7d %7d %8d %12d" % ("shared_assets/PSX", n, n, n, b),
            "   SUMME %28d %7d %8d %12d" % (n, n, n, b),
            "   Manifest:   %d Zeilen / %d Bytes (Format v2, sha256 je Datei)" % (n, b),
            "   RE2/DOOR:  Quelle 27, APK 27, sha256 gleich 27/27",
            "   RE15DOOR:  Quelle 30, APK 30, sha256 gleich 30/30",
            "   TORSE.VBS: Quelle 1234 B, APK 1234 B, sha256 gleich"] + TUER + [
            "== APK-ASSET-GATE-OK: %d Dateien in 1 Baeumen bytegleich, Tuer-Soll erfuellt, Manifest stimmt, "
            "ZIP-Struktur wie Android sie liest ==" % n]


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
    f("apk: Tuer-Soll fehlt", "apk", "\n".join(z for z in A if "Tuer-Soll" not in z), 0, 2)
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
    f("qb: Tuer-Soll fehlt", "quellbaum", "\n".join(z for z in Q if "Tuer-Soll" not in z), 0, 2)
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
    f("pk: Tuer-Soll fehlt", "paket", "\n".join(z for z in P if "Tuer-Soll" not in z), 0, 2)
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
}


# ---------------------------------------------------------------------------------------------------------------------
class _Mutierer(ast.NodeTransformer):
    """Erzeugt je Lauf genau EINE Aenderung an der Stelle nr (Zaehlung in Besuchsreihenfolge)."""
    TAUSCH = {ast.Eq: ast.NotEq, ast.NotEq: ast.Eq, ast.Lt: ast.LtE, ast.LtE: ast.Lt, ast.Gt: ast.GtE, ast.GtE: ast.Gt,
              ast.In: ast.NotIn, ast.NotIn: ast.In, ast.Is: ast.IsNot, ast.IsNot: ast.Is}

    def __init__(self, quelle, ziel):
        self.quelle, self.ziel, self.nr, self.beschreibung = quelle, ziel, -1, None
        self.meldung = 0                  # > 0: innerhalb eines Meldungstextes (nur Text, nie das Urteil)

    def _als_meldung(self, node):
        self.meldung += 1
        try:
            return self.visit(node)
        finally:
            self.meldung -= 1

    def visit_BinOp(self, node):          # "text %d" % (...): die Argumente formen nur den Text
        if isinstance(node.op, ast.Mod) and isinstance(node.left, ast.Constant) and isinstance(node.left.value, str):
            node.right = self._als_meldung(node.right)
            return node
        self.generic_visit(node)
        return node

    def visit_Call(self, node):           # ende(code, grund): grund ist nur Text; code wird mutiert
        if isinstance(node.func, ast.Name) and node.func.id == "ende" and len(node.args) == 2:
            node.args[0] = self.visit(node.args[0])
            node.args[1] = self._als_meldung(node.args[1])
            return node
        self.generic_visit(node)
        return node

    def _treffer(self, node, was):
        if self.meldung:
            return False
        self.nr += 1
        if self.nr == self.ziel:
            seg = (ast.get_source_segment(self.quelle, node) or "?").replace("\n", " ")
            self.beschreibung = "%s: %s" % (was, re.sub(r"\s+", " ", seg))
            self.zeile = getattr(node, "lineno", 0)
            return True
        return False

    def visit_Compare(self, node):
        self.generic_visit(node)
        for i, op in enumerate(node.ops):
            neu = self.TAUSCH.get(type(op))
            if neu and self._treffer(node, "Vergleich %d %s -> %s" % (i, type(op).__name__, neu.__name__)):
                node.ops[i] = neu()
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
            node.test = ast.Constant(value=False)
        elif self._treffer(node.test, "if-Bedingung -> True"):
            node.test = ast.Constant(value=True)
        return node

    def visit_Constant(self, node):
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


def _urteil_quelle():
    with open(__file__, encoding="utf-8") as f:
        text = f.read()
    baum = ast.parse(text)
    fn = next(n for n in baum.body if isinstance(n, ast.FunctionDef) and n.name == "urteil")
    return text, fn


def _mutant(text, fn, nr):
    import copy
    neu = copy.deepcopy(fn)
    m = _Mutierer(text, nr)
    neu = m.visit(neu)
    if m.beschreibung is None:
        return None, None
    ast.fix_missing_locations(neu)
    modul = ast.Module(body=[neu], type_ignores=[])
    ns = {"re": re, "_Urteil": _Urteil}
    exec(compile(modul, "<mutant %d>" % nr, "exec"), ns)    # noqa: S102 - eigener Code, im Selbsttest
    return ns["urteil"], (m.beschreibung, m.zeile)


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
    text, fn = _urteil_quelle()
    n_m = erkannt = gleich = 0
    ueberlebt, aeq_gesehen = [], set()
    nr = 0
    while True:
        mfn, besch = _mutant(text, fn, nr)
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
    print("   Mutanten: %d erzeugt, %d erkannt, %d als gleichwertig begruendet, %d ueberlebt" % (n_m, erkannt, gleich,
                                                                                         len(ueberlebt)))
    if falsch or ueberlebt or veraltet or n_m == 0:
        print("== URTEIL-SELBSTTEST-FEHLER: %d von %d Faellen falsch, %d Mutanten ueberlebt, %d veraltete Eintraege =="
              % (falsch, n_f, len(ueberlebt), len(veraltet)))
        return 1
    print("== URTEIL-SELBSTTEST-OK: %d/%d Faelle, %d/%d Mutanten erkannt, %d als gleichwertig begruendet =="
          % (n_f, n_f, erkannt, n_m, gleich))
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
